CC = gcc
CFLAGS = -std=c11 -Wall -Wextra

INCLUDEDIR = include
SRCDIR = src
LIBDIR = lib
BUILDDIR = build

SERVERSOURCE := $(SRCDIR)/server/server.c
SERVEROBJECT := $(SERVERSOURCE:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
SERVERTARGET := $(SERVERSOURCE:$(SRCDIR)/%.c=$(BUILDDIR)/%)

CLIENTSOURCE := $(SRCDIR)/client/client.c
CLIENTOBJECT := $(CLIENTSOURCE:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
CLIENTTARGET := $(CLIENTSOURCE:$(SRCDIR)/%.c=$(BUILDDIR)/%)

SOURCES := $(filter-out $(SERVERSOURCE) $(CLIENTSOURCE), $(shell find $(SRCDIR) -name '*.c'))
OBJECTS := $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))

LIBSOURCES := $(shell find lib -name '*.c')
LIBOBJECTS := $(patsubst $(LIBDIR)/%.c, $(BUILDDIR)/%.o, $(LIBSOURCES))


all: $(SERVERTARGET) $(CLIENTTARGET)

server: $(SERVERTARGET)

client: $(CLIENTTARGET)

$(SERVERTARGET): $(SERVEROBJECT) $(OBJECTS) $(LIBOBJECTS)
	mkdir -p $(dir $@)
	$(CXX) -o $@ $^

$(CLIENTTARGET): $(CLIENTOBJECT) $(OBJECTS) $(LIBOBJECTS)
	mkdir -p $(dir $@)
	$(CXX) -o $@ $^

$(LIBOBJECTS): $(BUILDDIR)/%.o: $(LIBDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I $(INCLUDEDIR) -c -o $@ $^

$(OBJECTS): $(BUILDDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I $(INCLUDEDIR) -c -o $@ $^

$(CLIENTOBJECT): $(CLIENTSOURCE)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I $(INCLUDEDIR) -c -o $@ $^

$(SERVEROBJECT): $(SERVERSOURCE)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I $(INCLUDEDIR) -c -o $@ $^

clean:
	rm -rf $(BUILDDIR)

.PHONY: all server client clean
