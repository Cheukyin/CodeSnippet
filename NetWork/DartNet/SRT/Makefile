all: cs78

cs78: server/app_server.o client/app_client.o common/seg.o server/srt_server.o client/srt_client.o 
	gcc -std=c99 -g -pthread server/app_server.o common/seg.o server/srt_server.o -o server/app_server 
	gcc -std=c99 -g -pthread client/app_client.o common/seg.o client/srt_client.o -o client/app_client
client/app_client.o: client/app_client.c 
	gcc -std=c99 -pthread -g -c client/app_client.c -o client/app_client.o 
server/app_server.o: server/app_server.c 
	gcc -std=c99 -pthread -g -c server/app_server.c -o server/app_server.o
common/seg.o: common/seg.c common/seg.h
	gcc -std=c99 -g -c common/seg.c -o common/seg.o
client/srt_client.o: client/srt_client.c client/srt_client.h 
	gcc -std=c99 -pthread -g -c client/srt_client.c -o client/srt_client.o
server/srt_server.o: server/srt_server.c server/srt_server.h common/list.h
	gcc -std=c99 -pthread -g -c server/srt_server.c -o server/srt_server.o
clean:
	rm -rf common/*.o
	rm -rf client/*.o
	rm -rf server/*.o
	rm -rf client/app_client
	rm -rf server/app_server
