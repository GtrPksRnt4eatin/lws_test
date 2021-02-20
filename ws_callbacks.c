#include <libwebsockets.h>
#include <cjson/cJSON.h>

#include "ws_msgs.h"
#include "ws_callbacks.h"

///////////////////////////////////////////////////////////////////////////

int handle_ws_rx(char *in);

///////////////////////////////////////////////////////////////////////////

int ws_callbacks(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
  char msg[256];
  
  printf("WS Callback reason: %d\n", reason);

  switch (reason) {

    case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS:      
      SSL_CTX_load_verify_locations((SSL_CTX *)user, "/etc/ssl/certs/ca-certificates.crt", "/etc/ssl/certs/");
      break;

    case LWS_CALLBACK_CLIENT_ESTABLISHED:
      lwsl_user("WS Connected\n");
      outgoing_ws_push(generate_hello_msg());
      lws_callback_on_writable(wsi);
      break;

    case LWS_CALLBACK_CLIENT_RECEIVE:
      printf("WS Receive\n");
      handle_ws_rx(in);
      break;

    case LWS_CALLBACK_CLIENT_WRITEABLE:
      if(outgoing_ws_shift(msg) == 0)
        lws_send_json(wsi,msg);
      lws_callback_on_writable(wsi);              
      break;

    case LWS_CALLBACK_CLIENT_CLOSED:
      printf("WS Closed\n");
      //client_wsi = NULL;
      break;

    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
      lwsl_err("CLIENT_CONNECTION_ERROR: %s\n", in ? (char *)in : "(null)");
      //client_wsi = NULL;
      break;

    default:
      break;
  }

  return lws_callback_http_dummy(wsi, reason, user, in, len);
}

////////////////////////////////////////////////////////////////////////////////

int handle_ws_rx(char *in)
{
  char msg[256];
  strcpy(msg,in);

  cJSON *json = cJSON_Parse(msg);
  if (json == NULL)
  {
    const char *error_ptr = cJSON_GetErrorPtr();
    if(error_ptr != NULL)
      fprintf(stderr, "JSON Error before: %s\n", error_ptr);
    return -1;
  }

  //char *string = cJSON_Print(json);
  //lwsl_user("JSON RX: \n%s\n", string);

  cJSON *type = cJSON_GetObjectItemCaseSensitive(json, "type");
  
  lwsl_user("RX type: %s\n", type->valuestring);

  if( strcmp(type->valuestring,"heartbeat") == 0 )
    outgoing_ws_push( "{\"type\":\"heartbeat\"}" );
  else if( strcmp(type->valuestring,"welcome") == 0 )
    outgoing_ws_push( generate_offer_msg() );

  return 0;
}