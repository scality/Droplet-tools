/*
 * retrieve a file by ID
 */

#include <droplet.h>
#include <getopt.h>
#include <assert.h>
#include <unistd.h>

void usage()
{
  fprintf(stderr, "usage: dplgetid [-p profile] [-s range_start] [-e range_end] [-B bucket] [-C conditions] [-O options] id\n");
  exit(1);
}

int
main(int argc,
     char **argv)
{
  char opt;
  int ret;
  dpl_ctx_t *ctx;
  char *data_buf_returned = NULL;
  u_int data_len_returned;
  dpl_range_t range, *rangep = NULL;
  char *id = NULL;
  char *profile = NULL;
  ssize_t cc;
  char *bucket = NULL;
  dpl_condition_t cond, *condp = NULL;
  dpl_dict_t *metadata = NULL;
  dpl_sysmd_t sysmd;
  dpl_option_t option, *optionp = NULL;

  memset(&range, 0, sizeof (range));

  while ((opt = getopt(argc, argv, "p:s:e:B:C:")) != -1)
    switch (opt)
      {
      case 'p':
        profile = strdup(optarg);
        assert(NULL != profile);
        break ;
      case 's':
        range.start = atoi(optarg);
        rangep = &range;
        break ;
      case 'e':
        range.end = atoi(optarg);
        rangep = &range;
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

  ret = dpl_get_id(ctx,
                   bucket,
                   id,
                   optionp,
                   DPL_FTYPE_REG,
                   condp,
                   rangep,
                   &data_buf_returned,
                   &data_len_returned,
                   &metadata,
                   &sysmd);
  if (DPL_SUCCESS != ret)
    {
      fprintf(stderr, "dpl_get_id failed: %s (%d)\n", dpl_status_str(ret), ret);
      exit(1);
    }

  cc = write(1, data_buf_returned, data_len_returned);
  if (-1 == cc)
    {
      perror("write");
      exit(1);
    }

  if (cc != data_len_returned)
    {
      fprintf(stderr, "short write\n");
      exit(1);
    }
  
  if (NULL != metadata)
    dpl_dict_print(metadata, stderr, 0);

  dpl_sysmd_print(&sysmd, stderr);

  free(data_buf_returned);

  if (NULL != metadata)
    dpl_dict_free(metadata);

  dpl_ctx_free(ctx); //free context

  dpl_free();        //free droplet library

  exit(0);
}
