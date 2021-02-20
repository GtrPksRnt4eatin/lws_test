#ifndef WS_CALLBACKS_H
#define WS_CALLBACKS_H

#include <stddef.h>

int ws_callbacks(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);

#endif
