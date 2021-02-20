#include <gtk/gtk.h>
#include <libwebsockets.h>

#include "ws_msgs.h"
#include "ws_callbacks.h"

#define LWS_WITH_SYS_STATE

static const struct lws_protocols protocols[];
static void sigint_handler(int sig);

static struct lws *client_wsi;

GMainLoop *loop;

static int system_notify_cb(lws_state_manager_t *mgr, lws_state_notify_link_t *link, int current, int target)
{
  printf("System Notify Callback current: %d -> target -> %d\n", current, target);
  struct lws_context *context = mgr->parent;
  struct lws_client_connect_info i;

  if (current != LWS_SYSTATE_OPERATIONAL || target != LWS_SYSTATE_OPERATIONAL)
    return 0;

  lwsl_notice("%s: operational\n", __func__);

  memset(&i, 0, sizeof i);
  i.context = context;
  i.port = 443;
  i.address = "global.vss.twilio.com";
  i.path = "/signaling";
  i.host = i.address;
  i.origin = i.address;
  i.ssl_connection = LCCSCF_USE_SSL;
  i.protocol = protocols[0].name;
  i.pwsi = &client_wsi;

  return !lws_client_connect_via_info(&i);
}

int main(int argc, const char **argv)
{
  lws_state_notify_link_t notifier = { {0}, system_notify_cb, "app" };
  lws_state_notify_link_t *na[] = { &notifier, NULL };

  struct lws_context_creation_info info;
  struct lws_context *context;
  void *foreign_loops[1];

  loop = g_main_loop_new(NULL, FALSE);

  signal(SIGINT, sigint_handler);

  lws_set_log_level(LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_DEBUG | LLL_INFO | LLL_PARSER | LLL_EXT | LLL_HEADER, NULL);

  memset(&info, 0, sizeof info);
  info.port = CONTEXT_PORT_NO_LISTEN;
  info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT | LWS_SERVER_OPTION_GLIB;
  info.protocols = protocols;
  info.client_ssl_ca_filepath = "/etc/ssl/certs/ca-certificates.crt";
  info.timeout_secs = 10;
  info.connect_timeout_secs = 30;
  foreign_loops[0] = (void *)loop;
  info.foreign_loops = foreign_loops;
  info.register_notifier_list = na;
  info.fd_limit_per_thread = 1 + 1 + 1;

  context = lws_create_context(&info);

  if (!context) {
          lwsl_err("lws init failed\n");
          return -1;
  }

  /*
    * We created the lws_context and bound it to this thread's main loop,
    * let's run the thread's main loop now...
    */

  printf("running loop!\n");
  g_main_loop_run(loop);
  g_main_loop_unref(loop);

  lws_context_destroy(context);

  return 0;
}

/*
int
main(int argc, char **argv)
{
        GMainContext *t1_mc = g_main_context_new();
        GtkApplication *app;
        GThread *t1;
        int status;

        t1 = g_thread_new ("t1", t1_main, g_main_context_ref (t1_mc));
        (void)t1;

        app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
        g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

        status = g_application_run(G_APPLICATION(app), argc, argv);
        g_object_unref(app);

        return status;
}

*/
///////////////////////////////////////////////////////////////////////


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
  g_main_loop_quit( loop );
}