all: server client

server: server.c serverside.c apiwrap.c sockfdarr.c
	gcc -g server.c serverside.c apiwrap.c sockfdarr.c -o server

client: client.c ittycli.c
	gcc client.c ittycli.c -o client

clean:
	rm server client
