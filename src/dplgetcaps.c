/*
 * print backend capabilities
 */

#include <droplet.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>

void
usage()
{
  fprintf(stderr, "usage: getcap [-p profile]\n");
  exit(1);
}

int
main(int argc,
     char **argv)
{
  int ret;
  dpl_ctx_t *ctx;
  char opt;
  char *profile = NULL;

  while ((opt = getopt(argc, argv, "p:")) != -1)
    switch (opt)
      {
      case 'p':
        profile = strdup(optarg);
        assert(NULL != profile);
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

  ret = dpl_print_capabilities(ctx);
  if (DPL_SUCCESS != ret)
    {
      fprintf(stderr, "get cap failed: %s (%d)\n", dpl_status_str(ret), ret);
      exit(1);
    }
  
  dpl_ctx_free(ctx);

  dpl_free();

  exit(0);
}
