/*
 * Key generation.
 */

#include <droplet.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>

void
usage()
{
  fprintf(stderr, "usage: genkey [-p profile] [-s storage_class] [-c custom]\n");
  exit(1);
}

int
main(int argc,
     char **argv)
{
  int ret;
  dpl_ctx_t *ctx;
  char opt;
  dpl_storage_class_t storage_class = DPL_STORAGE_CLASS_STANDARD;
  char *profile = NULL;
  char *custom = NULL;
  char id_buf[DPL_SYSMD_ID_SIZE+1];

  while ((opt = getopt(argc, argv, "p:s:c:")) != -1)
    switch (opt)
      {
      case 'p':
        profile = strdup(optarg);
        assert(NULL != profile);
        break ;
      case 's':
        storage_class = dpl_storage_class(optarg);
        if (-1 == storage_class)
          {
            fprintf(stderr, "bad storage class\n");
            exit(1);
          }
        break ;
      case 'c':
        custom = strdup(optarg);
        assert(NULL != custom);
        break ;
      case '?':
      default:
        usage();
      }
  argc -= optind;
  argv += optind;

  if (argc != 0)
    usage();

  ret = dpl_init();
  if (DPL_SUCCESS != ret)
    {
      fprintf(stderr, "dpl_init failed\n");
      exit(1);
    }

  ctx = dpl_ctx_new(NULL, profile);
  if (NULL == ctx)
    {
      fprintf(stderr, "dpl_ctx_new failed\n");
      exit(1);
    }

  ret = dpl_gen_random_key(ctx, storage_class, custom, id_buf, sizeof (id_buf));
  if (DPL_SUCCESS != ret)
    {
      fprintf(stderr, "key gen failed: %s (%d)\n", dpl_status_str(ret), ret);
      exit(1);
    }
  
  printf("%s\n", id_buf);
  
  ret = 0;

  dpl_ctx_free(ctx);

  dpl_free();

  exit(0);
}
