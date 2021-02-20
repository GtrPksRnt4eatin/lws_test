#include <uuid/uuid.h>
#include <cjson/cJSON.h>

#include "ws_msgs.h"

int lws_send_json(struct lws *wsi, char *message)
{
  char buf[LWS_PRE + 256];
  memset(&buf[LWS_PRE], 0, 256);
  lws_strncpy(&buf[LWS_PRE], message, strlen(message)+1);
  lwsl_user("sending: %s\n", &buf[LWS_PRE]);
  return lws_write(wsi, (unsigned char*)&buf[LWS_PRE], strlen(message), LWS_WRITE_TEXT); 
}

///////////////////////////////////////////////////////////

static uuid_t binuuid;
static char uuid[UUID_STR_LEN];

char *generate_hello_msg()
{
  static char msg[256];
  uuid_generate_random(binuuid);
  uuid_unparse_lower(binuuid, uuid);
  sprintf(msg, "{ \"id\": \"%s\", \"timeout\": 5000, \"type\": \"hello\" }", uuid);
  return msg;
}

char *generate_offer_msg()
{
  static char msg[1024];
  cJSON *json = cJSON_Parse(WS_OFFER_TEMPLATE);
  char *str = cJSON_Print(json);
  strcpy(msg, str);
  lwsl_user("OFFER RX: \n%s\n", msg);
  return msg;
}

////////////////////////////////////////////////////////////

#define OUTGOING_WS_MAX 20

static char outgoing_ws_buf[OUTGOING_WS_MAX][256];
static int  outgoing_ws_count;

int outgoing_ws_push(char *msg)
{
  if(outgoing_ws_count>=OUTGOING_WS_MAX)
    return -1;
  
  strcpy(outgoing_ws_buf[outgoing_ws_count], msg);
  outgoing_ws_count++;
  return 0;
}

int outgoing_ws_shift(char *buf)
{
  if(outgoing_ws_count<=0)
    return -1;
  
  strcpy(buf, outgoing_ws_buf[0]);
  memmove(outgoing_ws_buf[0],outgoing_ws_buf[1],256*(OUTGOING_WS_MAX-1));
  outgoing_ws_count--;
  return 0;
}

///////////////////////////////////////////////////////////////