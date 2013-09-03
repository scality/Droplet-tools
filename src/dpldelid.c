/*
 * delete a file by ID
 */

#include <droplet.h>
#include <getopt.h>
#include <assert.h>
#include <fcntl.h>

void usage()
{
  fprintf(stderr, "usage: dpldelid [-p profile] [-B bucket] [-C conditions] [-O options] id\n");
  exit(1);
}

int
main(int argc,
     char **argv)
{
  char opt;
  int ret;
  dpl_ctx_t *ctx;
  char *id = NULL;
  char *profile = NULL;
  char *bucket = NULL;
  dpl_condition_t cond, *condp = NULL;
  dpl_option_t option, *optionp = NULL;

  while ((opt = getopt(argc, argv, "p:B:C:O:")) != -1)
    switch (opt)
      {
      case 'p':
        profile = strdup(optarg);
        assert(NULL != profile);
        break ;
      case 'B':
        bucket = strdup(optarg);
        assert(NULL != bucket);
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
      case 'O':
        ret = dpl_parse_option(optarg, &option);
        if (DPL_SUCCESS != ret)
          {
            fprintf(stderr, "error in parsing option: %s (%d)\n", dpl_status_str(ret), ret);
            exit(1);
          }
        optionp = &option;
        break ;
      case '?':
      default:
        usage();
      }
  argc -= optind;
  argv += optind;

  if (argc != 1)
    usage();

  id = argv[0];

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

  ret = dpl_delete_id(ctx,
                      bucket,
                      id,
                      optionp,
                      DPL_FTYPE_REG, 
                      condp);
  if (DPL_SUCCESS != ret)
    {
      fprintf(stderr, "dpl_delete_id failed: %s (%d)\n", dpl_status_str(ret), ret);
      exit(1);
    }

  dpl_ctx_free(ctx); //free context

  dpl_free();        //free droplet library

  exit(0);
}
