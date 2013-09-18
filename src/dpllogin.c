/*
 * authentificate
 */

#include <droplet.h>
#include <getopt.h>
#include <assert.h>
#include <unistd.h>

void usage()
{
  fprintf(stderr, "usage: dpllogin [-p profile]\n");
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

  dpl_ctx_free(ctx); //free context

  dpl_free();        //free droplet library

  exit(0);
}
