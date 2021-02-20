#include <libwebsockets.h>
#include <cjson/cJSON.h>
#include <uuid/uuid.h>
#include <string.h>
#include <signal.h>

#include "ws_msgs.h"
#include "ws_callbacks.h"

static const struct lws_protocols protocols[];
static void sigint_handler(int sig);

static int interrupted, rx_seen, test;
static struct lws *client_wsi;

int main(int argc, const char **argv)
{
  struct lws_context_creation_info info;
  struct lws_client_connect_info i;
  struct lws_context *context;
  const char *p;
  int n = 0;

  signal(SIGINT, sigint_handler);
  if ((p = lws_cmdline_option(argc, argv, "-d")))
    logs = atoi(p);

  lws_set_log_level(LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE, NULL);
  lwsl_user("LWS minimal ws client rx [-d <logs>] [--h2] [-t (test)]\n");

  memset(&info, 0, sizeof info); /* otherwise uninitialized garbage */
  info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
  info.port = CONTEXT_PORT_NO_LISTEN; /* we do not run any server */
  info.protocols = protocols;
  info.timeout_secs = 10;
  info.connect_timeout_secs = 30;

  /*
    * since we know this lws context is only ever going to be used with
    * one client wsis / fds / sockets at a time, let lws know it doesn't
    * have to use the default allocations for fd tables up to ulimit -n.
    * It will just allocate for 1 internal and 1 (+ 1 http2 nwsi) that we
    * will use.
    */
  info.fd_limit_per_thread = 1 + 1 + 1;

  context = lws_create_context(&info);
  if (!context) {
          lwsl_err("lws init failed\n");
          return 1;
  }

  memset(&i, 0, sizeof i); /* otherwise uninitialized garbage */
  i.context = context;
  i.port = 443;
  i.address = "global.vss.twilio.com";
  i.path = "/signaling";
  i.host = i.address;
  i.origin = i.address;
  i.ssl_connection = LCCSCF_USE_SSL;
  i.protocol = protocols[0].name; /* "dumb-increment-protocol" */
  i.pwsi = &client_wsi;

  if (lws_cmdline_option(argc, argv, "--h2"))
          i.alpn = "h2";

  lws_client_connect_via_info(&i);

  while (n >= 0 && client_wsi && !interrupted)
      n = lws_service(context, 0);

  lws_context_destroy(context);

  lwsl_user("Completed %s\n", rx_seen > 10 ? "OK" : "Failed");

  return rx_seen > 10;
}

///////////////////////////////////////////////////////////////////////////

static const struct lws_protocols protocols[] = {
  {
    "twillio-ws",
    ws_callbacks,
    0,
    0,
  },
  { NULL, NULL, 0, 0 }
};

static void sigint_handler(int sig)
{
  interrupted = 1;
}