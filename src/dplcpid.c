/*
 * put a file by ID
 */

#include <droplet.h>
#include <getopt.h>
#include <assert.h>
#include <fcntl.h>

void usage()
{
  fprintf(stderr, "usage: dplputid [-p profile] [-S src_bucket] [-D dst_bucket] [-C conditions] [-M metadata] [-O options] [-c copy_directive] file id\n");
  exit(1);
}

int
main(int argc,
     char **argv)
{
  char opt;
  int ret;
  dpl_ctx_t *ctx;
  char *src_id = NULL;
  char *dst_id = NULL;
  char *profile = NULL;
  char *src_bucket = NULL;
  char *dst_bucket = NULL;
  dpl_condition_t cond, *condp = NULL;
  dpl_dict_t *metadata = NULL;
  dpl_option_t option, *optionp = NULL;
  dpl_copy_directive_t copy_directive = DPL_COPY_DIRECTIVE_UNDEF;

  while ((opt = getopt(argc, argv, "p:S:D:C:M:O:c:")) != -1)
    switch (opt)
      {
      case 'p':
        profile = strdup(optarg);
        assert(NULL != profile);
        break ;
      case 'S':
        src_bucket = strdup(optarg);
        assert(NULL != src_bucket);
        break ;
      case 'D':
        dst_bucket = strdup(optarg);
        assert(NULL != dst_bucket);
        break ;
      case 'C':
        ret = dpl_parse_condition(optarg, &cond);
        if (DPL_SUCCESS != ret)
          {
            fprintf(stderr, "error in parsing condition: %s (%d)\n", dpl_status_str(ret), ret);
            exit(1);
          }
        condp = &cond;
        break ;
      case 'M':
        metadata = dpl_parse_metadata(optarg);
        assert(NULL != metadata);
        break ;
      case 'O':
        ret = dpl_parse_option(optarg, &option);
        if (DPL_SUCCESS != ret)
          {
            fprintf(stderr, "error in parsing option: %s (%d)\n", dpl_status_str(ret), ret);
            exit(1);
          }
        optionp = &option;
        break ;
      case 'c':
        copy_directive = dpl_copy_directive(optarg);
        break ;
      case '?':
      default:
        usage();
      }
  argc -= optind;
  argv += optind;

  if (argc != 2)
    usage();

  src_id = argv[0];
  dst_id = argv[1];

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

  ret = dpl_copy_id(ctx,
                    src_bucket,
                    src_id,
                    dst_bucket,
                    dst_id,
                    optionp,
                    DPL_FTYPE_REG, 
                    copy_directive,
                    metadata,      
                    NULL,          //no sysmd
                    condp);
  if (DPL_SUCCESS != ret)
    {
      fprintf(stderr, "dpl_copy_id failed: %s (%d)\n", dpl_status_str(ret), ret);
      exit(1);
    }

  if (NULL != metadata)
    dpl_dict_free(metadata);

  dpl_ctx_free(ctx); //free context

  dpl_free();        //free droplet library

  exit(0);
}
