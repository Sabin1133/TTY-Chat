SPATH=Server-side
CPATH=Client-side

all: server client

#server: server.c serverside.c apiwrap.c sockfdarr.c
#	gcc server.c serverside.c apiwrap.c sockfdarr.c -o server

server: $(SPATH)/server.c $(SPATH)/serverside.c $(SPATH)/apiwrap.c $(SPATH)/sockfdarr.c
	gcc $^ -o $@

#client: client.c ittycli.c
#	gcc client.c ittycli.c -o client

client: $(CPATH)/client.c $(CPATH)/ittycli.c
	gcc $^ -o $@

clean:
	rm server client
