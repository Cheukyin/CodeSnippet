#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#include "srt_server.h"

//create a tcp connection to the client
int overlay_start() {
	int tcpserv_sd;
	struct sockaddr_in tcpserv_addr;
	int connection;
	struct sockaddr_in tcpclient_addr;
	socklen_t tcpclient_addr_len;

	tcpserv_sd = socket(AF_INET, SOCK_STREAM, 0); 
	if(tcpserv_sd<0) 
		return -1;
	memset(&tcpserv_addr, 0, sizeof(tcpserv_addr));
	tcpserv_addr.sin_family = AF_INET;
	tcpserv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	tcpserv_addr.sin_port = htons(PORT);

	if(bind(tcpserv_sd, (struct sockaddr *)&tcpserv_addr, sizeof(tcpserv_addr))< 0)
		return -1; 
	if(listen(tcpserv_sd, 1) < 0) 
		return -1;
	printf("waiting for connection\n");
	connection = accept(tcpserv_sd,(struct sockaddr*)&tcpclient_addr,&tcpclient_addr_len);
	return connection;
}

//close tcp connection to client
void overlay_stop(int connection) {
	close(connection);
}

int main() {
	//random seed for segment loss
	srand(time(NULL));

	//create a tcp connection to client
	int overlay_conn = overlay_start();
	if(overlay_conn<0) {
		printf("can not start overlay\n");
	}

	//initialize srt server
	srt_server_init(overlay_conn);

	/*one server*/	
	//create a srt server sock at port 88 
	int sockfd= srt_server_sock(88);
	if(sockfd<0) {
		printf("can't create srt server\n");
		exit(1);
	}
	//listen and accept connection from a srt client 
	srt_server_accept(sockfd);

	/*another server*/
	//create a srt server sock at port 90 
	int sockfd2= srt_server_sock(90);
	if(sockfd2<0) {
		printf("can't create srt server\n");
		exit(1);
	}
	//listen and accept connection from a srt client 
	srt_server_accept(sockfd2);


	sleep(10);

	//close srt client 
	if(srt_server_close(sockfd)<0) {
		printf("can't destroy srt server\n");
		exit(1);
	}				
	if(srt_server_close(sockfd2)<0) {
		printf("can't destroy srt server\n");
		exit(1);
	}				

	//close tcp connection to the client
	overlay_stop(overlay_conn);
}
