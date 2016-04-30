#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
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
    tcbUsedList = (tcb_t*)malloc( sizeof(tcb_t) );
    tcbUsedList->prev = tcbUsedList->next = NULL;
    tcbFreeList = (tcb_t*)malloc( sizeof(tcb_t) );
    tcbFreeList->prev = tcbFreeList->next = NULL;

    bzero(port2sock, MAX_PORT_NUM * sizeof(sock_t));

    overlayConn = conn;

    pthread_t pid;
    if(pthread_create(&pid, NULL, seghandler, NULL) < 0)
    {
        printf("Thread creation failed!\n");
        exit(1);
    }
    if(pthread_detach(pid) < 0)
    {
        printf("Thread detach failed!\n");
        exit(1);
    }
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
    tcb_t *node = takeFromListHead(tcbFreeList);
    if(!node) return NULL;
    assert(node->isFree);

    node->state = CLOSED;
    node->client_portNum = client_port;
    inserIntoList(tcbUsedList, node);
    node->isFree = 0;

    port2sock[client_port] = node;

    return node;
}

int try_send(tcb_t *tcb, int tryMaxNum, seg_t *seg,
             unsigned int midState, unsigned int destState, int interval)
{
    int tryNum = tryMaxNum;

    while(--tryNum)
    {
        printf("try: %d\n", tryNum);
        sendseg(overlayConn, seg);
        tcb->state = midState;

        if(usleep( interval/(unsigned int)1e3 ) < 0)
            printf("usleep error!\n");

        if(tcb->state == destState)
            return 0;
    }

    tcb->state = CLOSED;
    printf("ACK LOST!\n");
    return -1;
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
    tcb_t *tcb = sock2TCB(sockfd);
    if(!tcb || tcb->isFree)
    {
        printf("tcb is not in used!\n");
        return -1;
    }

    if(tcb->state != CLOSED)
    {
        printf("socket closed in a connect state: %d!\n", tcb->state);
        return -1;
    }

    printf("Connect!\n");

    tcb->svr_portNum = server_port;

    seg_t seg;
    seg.header.type = SYN;
    seg.header.src_port = tcb->client_portNum;
    seg.header.dest_port = server_port;

    return try_send(tcb, SYN_MAX_RETRY, &seg, SYNSENT, CONNECTED, SYNSEG_TIMEOUT_NS);
}

// Send data to a srt server
//
// Send data to a srt server. You do not need to implement this for Lab4.
// We will use this in Lab5 when we implement a Go-Back-N sliding window.
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//

int srt_client_send(sock_t sockfd, void* data, unsigned int length)
{
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

int srt_client_disconnect(sock_t sockfd)
{
    tcb_t *tcb = sock2TCB(sockfd);
    if(!tcb || tcb->isFree) return -1;

    if(tcb->state != CONNECTED)
    {
        printf("srt_client_disconnect invoked in a connect state: %d!\n", tcb->state);
        return -1;
    }

    printf("Disconnect!\n");
    seg_t seg;
    seg.header.type = FIN;
    seg.header.src_port = tcb->client_portNum;
    seg.header.dest_port = tcb->svr_portNum;

    return try_send(tcb, FIN_MAX_RETRY, &seg, FINWAIT, CLOSED, FINSEG_TIMEOUT_NS);
}


// Close srt client

// This function calls free() to free the TCB entry. It marks that entry in TCB as NULL
// and returns 1 if succeeded (i.e., was in the right state to complete a close) and -1
// if fails (i.e., in the wrong state).
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//

int srt_client_close(sock_t sockfd)
{
    tcb_t *node = sock2TCB(sockfd);

    if(!node || node->isFree) return -1;
    removeFromList(node);
    inserIntoList(tcbFreeList, node);
    node->isFree = 1;

    if(node->state != CLOSED)
    {
        printf("socket closed in a connect state: %d!\n", node->state);
        return -1;
    }

	return 1;
}

// The thread handles incoming segments
//
// This is a thread  started by srt_client_init(). It handles all the incoming
// segments from the server. The design of seghanlder is an infinite loop that calls snp_recvseg(). If
// snp_recvseg() fails then the overlay connection is closed and the thread is terminated. Depending
// on the state of the connection when a segment is received  (based on the incoming segment) various
// actions are taken. See the client FSM for more details.
//

void *seghandler(void* arg)
{
    seg_t seg;

    while(recvseg(overlayConn, &seg) > 0)
    {
        int destPort = seg.header.dest_port;
        unsigned int segType = seg.header.type;
        tcb_t *node = sock2TCB( port2sock[destPort] );
        if(!node || node->isFree) continue;

        switch(node->state)
        {
            case CLOSED:
                printf("Wrong seg rcvd in CLOSED state!\n");
                continue;

            case SYNSENT:
                if(segType == SYNACK)
                    node->state = CONNECTED;
                else
                {
                    printf("Wrong seg rcvd in SYNSENT state!\n");
                    continue;
                }
                break;

            case CONNECTED:
                break;

            case FINWAIT:
                if(segType == FINACK)
                    node->state = CLOSED;
                else
                {
                    printf("Wrong seg rcvd in FINWAIT state!\n");
                    continue;
                }
                break;

            default:
                printf("In an unknown state!\n");
                continue;
        }
    }

    return 0;
}