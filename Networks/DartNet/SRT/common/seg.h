#ifndef SEG_H
#define SEG_H

#include "constants.h"
enum seg_type_t {
	SYN,
	SYNACK,
	FIN,
	FINACK,
	DATA,
	DATAACK
};

typedef struct srt_hdr {
	unsigned int src_port;
	unsigned int dest_port;
	unsigned int seq_num;
	unsigned int ack_num;
	unsigned short int length;
	unsigned short int  type;
	unsigned short int  rcv_win;
	unsigned short int checksum;
} srt_hdr_t;

typedef struct segment {
	srt_hdr_t header;
	char data[MAX_SEG_LEN];
} seg_t;


int sendseg(int connection, seg_t* segPtr);
int recvseg(int connection, seg_t* segPtr);
int seglost();
#endif
