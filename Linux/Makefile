SPATH=Server-side
CPATH=Client-side

all: server client

server: $(SPATH)/server.c $(SPATH)/serverside.c $(SPATH)/apiwrap.c $(SPATH)/sockfdarr.c
	gcc $^ -o $@

client: $(CPATH)/client.c $(CPATH)/ittycli.c
	gcc $^ -o $@

clean:
	rm server client
