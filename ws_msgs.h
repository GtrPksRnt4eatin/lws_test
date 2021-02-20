#ifndef WS_MSGS_H
#define WS_MSGS_H

#include <libwebsockets.h>

int lws_send_json(struct lws *wsi, char *message);

char *generate_hello_msg();
char *generate_offer_msg();

int outgoing_ws_push(char *msg);
int outgoing_ws_shift(char *buf);

#define WS_OFFER_TEMPLATE "\
{                           \
  \"body\": {                \
    \"name\": null,           \
    \"participant\": {         \
      \"revision\": 1,          \
       \"tracks\":[]             \
    },                            \
    \"peer_connections\": [        \
      { \"description\": {          \
          \"type\": \"offer\",       \
          \"revision\": 1,           \
          \"sdp\": \"???\"           \
        },                           \
        \"id\": \"???\"              \
      }                              \
    ],                               \
    \"type\": \"connect\",           \
    \"version\": 2,                  \
    \"ice_servers\": \"success\",    \
    \"publisher\": {                  \
      \"name\": \"twilio-video.js\",   \
      \"sdk_version\": \"2.4.0\",       \
      \"user_agent\": \"Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.183 Safari/537.36\" }, \
      \"bandwidth_profile\": {             \
        \"video\": {                        \
          \"render_dimensions\":{            \
            \"standard\": {                   \
              \"width\":640,                   \
              \"height\":360                    \
            },                                   \
            \"low\": {                            \
              \"width\":160,                       \
              \"height\":90                         \
            },                                       \
            \"high\": {                               \
              \"width\": 1280,                         \
              \"height\":720                            \
            }                                            \
          },                                              \
          \"mode\": \"collaboration\",                     \
          \"active_speaker_priority\":\"high\"              \
        }                                                   \
      },                                                    \
      \"media_signaling\": {                                \
          \"active_speaker\": {                             \
            \"transports\": [{\"type\": \"data-channel\"}]  \
          },                                                \
          \"network_quality\": {                            \
            \"transports\":[{\"type\":\"data-channel\"}]    \
          },                                                \
          \"track_priority\":{                              \
            \"transports\":[{\"type\":\"data-channel\"}]    \
          },                                                \
          \"track_switch_off\":{                            \
            \"transports\":[{\"type\":\"data-channel\"}]    \
          }                                                 \
        },                                                  \
        \"subscribe\": {                                    \
          \"rules\": [{\"type\":\"include\",\"all\":true}], \
          \"revision\":1                                    \
        },                                                  \
        \"format\":\"unified\",                             \
        \"token\":\"???\"},                                 \
        \"type\": \"msg\"                                   \
      }                                                     \
    }                                                       \
  }                                                         \
"

#endif