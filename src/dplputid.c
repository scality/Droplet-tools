/*
 * put a file by ID
 */

#include <droplet.h>
#include <getopt.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

void usage()
{
  fprintf(stderr, "usage: dplputid [-p profile] [-s range_start] [-e range_end] [-B bucket] [-C conditions] [-M metadata] [-O options] file id\n");
  exit(1);
}

int
main(int argc,
     char **argv)
{
  char opt;
  int ret;
  dpl_ctx_t *ctx;
  char *data_buf = NULL;
  u_int data_len;
  dpl_range_t range, *rangep = NULL;
  char *id = NULL;
  char *profile = NULL;
  struct stat st;
  char *fname;
  int fd = -1;
  char *bucket = NULL;
  dpl_condition_t cond, *condp = NULL;
  dpl_dict_t *metadata = NULL;
  dpl_option_t option, *optionp = NULL;

  memset(&range, 0, sizeof (range));

  while ((opt = getopt(argc, argv, "p:s:e:B:C:M:O:")) != -1)
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
      case '?':
      default:
        usage();
      }
  argc -= optind;
  argv += optind;

  if (argc != 2)
    usage();

  fname = argv[0];
  id = argv[1];

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

  ret = stat(fname, &st);
  if (-1 == ret)
    {
      perror("stat");
      exit(1);
    }

  data_buf = malloc(st.st_size);
  if (NULL == data_buf)
    {
      perror("malloc");
      exit(1);
    }

  data_len = st.st_size;

  fd = open(fname, O_RDONLY);
  if (-1 == fd)
    {
      perror("open");
      exit(1);
    }
  
  ret = read(fd, data_buf, data_len);
  if (-1 == ret)
    {
      perror("read");
      exit(1);
    }

  if (data_len != ret)
    {
      fprintf(stderr, "short read");
      exit(1);
    }

  ret = dpl_put_id(ctx,
                   bucket,
                   id,
                   optionp,
                   DPL_FTYPE_REG, 
                   condp,        
                   rangep,        
                   metadata,      
                   NULL,          //no sysmd
                   data_buf,
                   data_len);
  if (DPL_SUCCESS != ret)
    {
      fprintf(stderr, "dpl_put_id failed: %s (%d)\n", dpl_status_str(ret), ret);
      exit(1);
    }

  if (NULL != metadata)
    dpl_dict_free(metadata);

  free(data_buf);

  close(fd);

  dpl_ctx_free(ctx); //free context

  dpl_free();        //free droplet library

  exit(0);
}
