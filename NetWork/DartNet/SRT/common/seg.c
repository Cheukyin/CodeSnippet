#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include "seg.h"

int sendseg(int connection, seg_t* segPtr) {
	char bufstart[2];
	char bufend[2];
	bufstart[0] = '!';
	bufstart[1] = '&';
	bufend[0] = '!';
	bufend[1] = '#';
	if (send(connection, bufstart, 2, 0) < 0) {
		return -1;
	}
	if(send(connection,segPtr,sizeof(seg_t),0)<0) {
		return -1;
	}
	if(send(connection,bufend,2,0)<0) {
		return -1;
	}
	return 1;
}

int recvseg(int connection, seg_t* segPtr) {
	char buf[sizeof(seg_t)+2]; 
	char c;
	int idx = 0;
	// state can be 0,1,2,3; 
	// 0 starting point 
	// 1 '!' received
	// 2 '&' received, start receiving segment
	// 3 '!' received,
	// 4 '#' received, finish receiving segment 
	int state = 0; 
	while(recv(connection,&c,1,0)>0) {
		if (state == 0) {
		        if(c=='!')
				state = 1;
		}
		else if(state == 1) {
			if(c=='&') 
				state = 2;
			else
				state = 0;
		}
		else if(state == 2) {
			if(c=='!') {
				buf[idx]=c;
				idx++;
				state = 3;
			}
			else {
				buf[idx]=c;
				idx++;
			}
		}
		else if(state == 3) {
			if(c=='#') {
				buf[idx]=c;
				idx++;
				state = 0;
				idx = 0;
				if(seglost()>0) {
                                	printf("seg lost!!!\n");
                                	continue;
                        	}
				memcpy(segPtr,buf,sizeof(seg_t));
				return 1;
			}
			else if(c=='!') {
				buf[idx]=c;
				idx++;
			}
			else {
				buf[idx]=c;
				idx++;
				state = 2;
			}
		}
	}
	return -1;
}

//if lost, return 1; otherwise return 0
int seglost() {
	int random = rand()%100;
	if(random<LOSS_RATE*100)
		return 1;
	else
		return 0;
}

