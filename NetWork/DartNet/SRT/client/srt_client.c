#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include "srt_client.h"


/*interfaces to application layer*/

//
//
//  SRT socket API for the client side application.
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
//  GOAL: Your goal for this assignment is to design and implement the 
//  protoypes below - fill the code.
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


static client_tcb_t *tcbUsedList;
static client_tcb_t *tcbFreeList;

static inline void inserIntoList(client_tcb_t *lh, client_tcb_t *newNode)
{
    assert(lh); assert(newNode);

    client_tcb_t *tcbNode = lh->next;

    lh->next = newNode;
    newNode->next = tcbNode;
    newNode->prev = lh;
    if(tcbNode) tcbNode->prev = newNode;
}

static inline void allocTCB()
{
    for(int i=0; i < 10; i++)
    {
        client_tcb_t *newNode = (client_tcb_t*)malloc( sizeof(client_tcb_t) );
        if(!newNode) return;
        inserIntoList(tcbFreeList, newNode);
    }
}

static inline client_tcb_t* removeFromList(client_tcb_t *node)
{
    assert(node);

    node->prev->next = node->next;
    if(node->next)
        node->next->prev = node->prev;
    return node;
}

static inline client_tcb_t* takeFromListHead(client_tcb_t *lh)
{
    if(!lh->next) allocTCB();
    if(!lh->next) return NULL;
    return removeFromList(lh->next);
}


// srt client initialization
//
// This function initializes the TCB table marking all entries NULL. It also initializes
// a global variable for the overlay TCP socket descriptor ‘‘conn’’ used as input parameter
// for snp_sendseg and snp_recvseg. Finally, the function starts the seghandler thread to
// handle the incoming segments. There is only one seghandler for the client side which
// handles call connections for the client.
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//

static int overlayConn;

void srt_client_init(int conn)
{
    tcbUsedList = (client_tcb_t*)malloc( sizeof(client_tcb_t) );
    tcbUsedList->prev = tcbUsedList->next = NULL;
    tcbFreeList = (client_tcb_t*)malloc( sizeof(client_tcb_t) );
    tcbFreeList->prev = tcbFreeList->next = NULL;

    overlayConn = conn;
}

// Create a client tcb, return the sock
//
// This function looks up the client TCB table to find the first NULL entry, and creates
// a new TCB entry using malloc() for that entry. All fields in the TCB are initialized
// e.g., TCB state is set to CLOSED and the client port set to the function call parameter
// client port.  The TCB table entry index should be returned as the new socket ID to the client
// and be used to identify the connection on the client side. If no entry in the TC table
// is available the function returns -1.
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//

sock_t srt_client_sock(unsigned int client_port)
{
    client_tcb_t *node = takeFromListHead(tcbFreeList);
    if(!node) return NULL;
    node->state = CLOSED;
    node->client_portNum = client_port;
    inserIntoList(tcbUsedList, node);
    return node;
}

// Connect to a srt server
//
// This function is used to connect to the server. It takes the socket ID and the
// server’s port number as input parameters. The socket ID is used to find the TCB entry.
// This function sets up the TCB’s server port number and a SYN segment to send to
// the server using snp_sendseg(). After the SYN segment is sent, a timer is started.
// If no SYNACK is received after SYNSEG_TIMEOUT timeout, then the SYN is
// retransmitted. If SYNACK is received, return 1. Otherwise, if the number of SYNs
// sent > SYN_MAX_RETRY,  transition to CLOSED state and return -1.
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//


int srt_client_connect(sock_t sockfd, unsigned int server_port)
{
    return 0;
}

// Send data to a srt server
//
// Send data to a srt server. You do not need to implement this for Lab4.
// We will use this in Lab5 when we implement a Go-Back-N sliding window.
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//

int srt_client_send(sock_t sockfd, void* data, unsigned int length) {
	return 1;
}

// Disconnect from a srt server
//
// This function is used to disconnect from the server. It takes the socket ID as
// an input parameter. The socket ID is used to find the TCB entry in the TCB table.
// This function sends a FIN segment to the server. After the FIN segment is sent
// the state should transition to FINWAIT and a timer started. If the
// state == CLOSED after the timeout the FINACK was successfully received. Else,
// if after a number of retries FIN_MAX_RETRY the state is still FINWAIT then
// the state transitions to CLOSED and -1 is returned.

int srt_client_disconnect(sock_t sockfd) {
  return 0;
}


// Close srt client

// This function calls free() to free the TCB entry. It marks that entry in TCB as NULL
// and returns 1 if succeeded (i.e., was in the right state to complete a close) and -1
// if fails (i.e., in the wrong state).
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//

int srt_client_close(sock_t sockfd) {
	return 0;
}

// The thread handles incoming segments
//
// This is a thread  started by srt_client_init(). It handles all the incoming
// segments from the server. The design of seghanlder is an infinite loop that calls snp_recvseg(). If
// snp_recvseg() fails then the overlay connection is closed and the thread is terminated. Depending
// on the state of the connection when a segment is received  (based on the incoming segment) various
// actions are taken. See the client FSM for more details.
//

void *seghandler(void* arg) {
  return 0;
}



