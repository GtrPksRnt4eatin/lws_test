all: ws gtk-example

CFLAGS += -Wall -fno-common

BUILD_DIR ?= build/

GTK_FLAGS = $(shell pkg-config --cflags --libs gtk+-3.0)

OBJS = $(addprefix $(BUILD_DIR), ws_callbacks.o ws_msgs.o ws.o )

build_folders:
	@echo ""
	mkdir -p $(BUILD_DIR)
	mkdir -p bin/

$(BUILD_DIR)%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -lssl -lcrypto $(GTK_FLAGS) -c -o $@ $<

ws: build_folders ws_msgs.h ws_callbacks.h $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(GTK_FLAGS) -luuid -L/usr/local/lib -lssl -lcrypto -lwebsockets -lcjson -o bin/$@

gtk-example: build_folders ws_msgs.o build/gtk_example.o
	$(CC) $(CFLAGS) build/ws_msgs.o build/gtk_example.o $(GTK_FLAGS) -luuid -L/usr/local/lib -lwebsockets -lcjson -o bin/$@

clean:
	rm -f -r bin
	rm -f -r build