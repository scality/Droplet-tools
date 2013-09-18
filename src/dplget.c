/*
 * retrieve a file by ID
 */

#include <droplet.h>
#include <getopt.h>
#include <assert.h>
#include <unistd.h>

void usage()
{
  fprintf(stderr, "usage: dpllist [-p profile] [-b bucket]\n");
  exit(1);
}

int
main(int argc,
     char **argv)
{
  char opt;
  int ret;
  dpl_ctx_t *ctx;
  char *profile = NULL;
  ssize_t cc;
  char *resource = NULL;
  dpl_dict_t *metadata = NULL;
  dpl_vec_t *objectsp;
  dpl_vec_t *common_prefixesp;

  char *buf;
  int len;

  while ((opt = getopt(argc, argv, "p:b:")) != -1)
    switch (opt)
      {
      case 'p':
        assert(NULL != optarg);
        profile = strdup(optarg);
        assert(NULL != profile);
        break ;
      case 'b':
        assert(NULL != optarg);
        resource = strdup(optarg);
        assert(NULL != resource);
        break ;
      case '?':
      default:
        usage();
      }
  argc -= optind;
  argv += optind;

  /* if (argc != 1) */
  /*   usage(); */

  ret = dpl_init();           //init droplet library
  if (DPL_SUCCESS != ret)
    {
      fprintf(stderr, "dpl_init failed\n");
      exit(1);
    }

  //open default profile
  ctx = dpl_ctx_new(NULL, profile);
  if (NULL == ctx)
    {
      fprintf(stderr, "dpl_ctx_new failed\n");
      exit(1);
    }

  ret = dpl_login(ctx);
  if (DPL_SUCCESS != ret)
    {
      fprintf(stderr, "dpl_login failed: %s (%d)\n", dpl_status_str(ret), ret);
      exit(1);
    }

  /*
    ctx
    const char *bucket,
    const char *resource,
    const char *subresource,
    const dpl_option_t *option,
    dpl_ftype_t object_type,
    const dpl_condition_t *condition,
    const dpl_range_t *range,
    char **data_bufp,
    unsigned int *data_lenp,
    dpl_dict_t **metadatap,
    dpl_sysmd_t *sysmdp,
    char **locationp)
  */
  ret = dpl_get(ctx,
		NULL,
		resource,
		NULL,
		DPL_FTYPE_ANY,
		NULL,
		NULL,
		&buf,
		&len,
		NULL,
		NULL);
  if (DPL_SUCCESS != ret)
    {
      fprintf(stderr, "dpl_list_bucket failed: %s (%d)\n", dpl_status_str(ret), ret);
      exit(1);
    }

  write(1, buf, len);

  if (NULL != metadata)
    dpl_dict_print(metadata, stderr, 0);

  if (NULL != metadata)
    dpl_dict_free(metadata);

  dpl_ctx_free(ctx); //free context

  dpl_free();        //free droplet library

  exit(0);
}
