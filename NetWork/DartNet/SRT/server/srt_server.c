#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "srt_server.h"
#include "../common/constants.h"


/*interfaces to application layer*/

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
//  GOAL: Your job is to design and implement the prototypes below - fill in the code.
//

// srt server initialization
//
// This function initializes the TCB table marking all entries NULL. It also initializes
// a global variable for the overlay TCP socket descriptor ‘‘conn’’ used as input parameter
// for snp_sendseg and snp_recvseg. Finally, the function starts the seghandler thread to
// handle the incoming segments. There is only one seghandler for the server side which
// handles call connections for the client.
//

void srt_server_init(int conn) {
  return;
}

// Create a server sock
//
// This function looks up the client TCB table to find the first NULL entry, and creates
// a new TCB entry using malloc() for that entry. All fields in the TCB are initialized
// e.g., TCB state is set to CLOSED and the server port set to the function call parameter
// server port.  The TCB table entry index should be returned as the new socket ID to the server
// and be used to identify the connection on the server side. If no entry in the TCB table
// is available the function returns -1.

int srt_server_sock(unsigned int port) {
	return 0;
}

// Accept connection from srt client
//
// This function gets the TCB pointer using the sockfd and changes the state of the connetion to
// LISTENING. It then starts a timer to ‘‘busy wait’’ until the TCB’s state changes to CONNECTED
// (seghandler does this when a SYN is received). It waits in an infinite loop for the state
// transition before proceeding and to return 1 when the state change happens, dropping out of
// the busy wait loop. You can implement this blocking wait in different ways, if you wish.
//

int srt_server_accept(int sockfd) {
	return 0;
}

// Receive data from a srt client
//
// Receive data to a srt client. Recall this is a unidirectional transport
// where DATA flows from the client to the server. Signaling/control messages
// such as SYN, SYNACK, etc.flow in both directions. You do not need to implement
// this for Lab4. We will use this in Lab5 when we implement a Go-Back-N sliding window.
//
int srt_server_recv(int sockfd, void* buf, unsigned int length) {
	return 1;
}

// Close the srt server
//
// This function calls free() to free the TCB entry. It marks that entry in TCB as NULL
// and returns 1 if succeeded (i.e., was in the right state to complete a close) and -1
// if fails (i.e., in the wrong state).
//

int srt_server_close(int sockfd) {
	return 0;
}

// Thread handles incoming segments
//
// This is a thread  started by srt_server_init(). It handles all the incoming
// segments from the client. The design of seghanlder is an infinite loop that calls snp_recvseg(). If
// snp_recvseg() fails then the overlay connection is closed and the thread is terminated. Depending
// on the state of the connection when a segment is received  (based on the incoming segment) various
// actions are taken. See the client FSM for more details.
//

void *seghandler(void* arg) {
  return 0;
}
