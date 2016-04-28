// FILE: srt_server.h
//
// Description: this file contains server states' definition, some important
// data structures and the server SRT socket interface definitions. You need 
// to implement all these interfaces
//
// Date: April 18, 2008
//       April 21, 2008 **Added more detailed description of prototypes fixed ambiguities** ATC
//       April 26, 2008 **Added GBN descriptions
//       May 1, 2009    ** Clarified that srt_server_recv blocks until all the requested data is ready ** ATC
//

#ifndef SRTSERVER_H
#define SRTSERVER_H

#include <pthread.h>
#include "../common/seg.h"
#include "../common/constants.h"

//server states used in FSM
#define	CLOSED 1
#define	LISTENING 2
#define	CONNECTED 3
#define	CLOSEWAIT 4


//server transport control block. the server side of a SRT connection uses this data structure to keep track of the connection information.
typedef struct svr_tcb {
	unsigned int svr_nodeID;        //node ID of server, similar as IP address, currently unused
	unsigned int svr_portNum;       //port number of server
	unsigned int client_nodeID;     //node ID of client, similar as IP address, currently unused
	unsigned int client_portNum;    //port number of client
	unsigned int state;         	//state of server
	unsigned int expect_seqNum;     //the server's expecting data sequence number	
	char* recvBuf;                  //a pointer pointing to the receive buffer
	unsigned int  usedBufLen;       //size of the received data in receive buffer
	pthread_mutex_t* bufMutex;      //a pointer pointing to the mutex which is used for receive buffer access
} svr_tcb_t;


//
//
//  SRT socket API for the server side application. 
//  ===================================
//
//  In what follows, we provide the prototype definition for each call and limited pseudo code representation
//  of the function. This is not meant to be comprehensive - more a guideline. 
// 
//  You are free to design the code as you wish.
//
//  NOTE: When designing all functions you should consider all possible states of the FSM using
//  a switch statement (see the Lab4 assignment for an example). Typically, the FSM has to be
// in a certain state determined by the design of the FSM to carry out a certain action. 
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//

void srt_server_init(int conn);

// This function initializes the TCB table marking all entries NULL. It also initializes 
// a global variable for the overlay TCP socket descriptor ``conn'' used as input parameter
// for snp_sendseg and snp_recvseg. Finally, the function starts the seghandler thread to 
// handle the incoming segments. There is only one seghandler for the server side which
// handles call connections for the client.
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//

int srt_server_sock(unsigned int port);

// This function looks up the client TCB table to find the first NULL entry, and creates
// a new TCB entry using malloc() for that entry. All fields in the TCB are initialized 
// e.g., TCB state is set to CLOSED and the server port set to the function call parameter 
// server port.  The TCB table entry index should be returned as the new socket ID to the server 
// and be used to identify the connection on the server side. If no entry in the TCB table  
// is available the function returns -1.

//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//

int srt_server_accept(int sockfd);

// This function gets the TCB pointer using the sockfd and changes the state of the connection to 
// LISTENING. It then starts a timer to ``busy wait'' until the TCB's state changes to CONNECTED 
// (seghandler does this when a SYN is received). It waits in an infinite loop for the state 
// transition before proceeding and to return 1 when the state change happens, dropping out of
// the busy wait loop. You can implement this blocking wait in different ways, if you wish.
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//

int srt_server_recv(int sockfd, void* buf, unsigned int length);

// Receive data from a srt client. Recall this is a unidirectional transport
// where DATA flows from the client to the server. Signaling/control messages
// such as SYN, SYNACK, etc.flow in both directions. 
// This function keeps polling the receive buffer every RECVBUF_POLLING_INTERVAL
// until the requested data is available, then it stores the data and returns 1
// If the function fails, return -1 
//
// Note that srt_server_recv blocked waiting for the user requested number
// of bytes (i.e., length) are at the server before returning data to the application
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//

int srt_server_close(int sockfd);

// This function calls free() to free the TCB entry. It marks that entry in TCB as NULL
// and returns 1 if succeeded (i.e., was in the right state to complete a close) and -1 
// if fails (i.e., in the wrong state).
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//

void* seghandler(void* arg);

// This is a thread  started by srt_server_init(). It handles all the incoming 
// segments from the client. The design of seghanlder is an infinite loop that calls snp_recvseg(). If
// snp_recvseg() fails then the overlay connection is closed and the thread is terminated. Depending
// on the state of the connection when a segment is received  (based on the incoming segment) various
// actions are taken. See the client FSM for more details.
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//

#endif
