#include <stdlib.h>
#include <sys/socket.h>
#include <strings.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
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

static inline void destroyList(node_t *lh)
{
    while(!lh)
    {
        node_t *next = lh->next;
        free(lh);
        lh = next;
    }
}

static inline void exitDestroyList()
{
    printf("Destroy List!\n");
    destroyList(tcbUsedList);
    destroyList(tcbFreeList);
}

// srt server initialization
//
// This function initializes the TCB table marking all entries NULL. It also initializes
// a global variable for the overlay TCP socket descriptor ‘‘conn’’ used as input parameter
// for snp_sendseg and snp_recvseg. Finally, the function starts the seghandler thread to
// handle the incoming segments. There is only one seghandler for the server side which
// handles call connections for the client.
//

static int overlayConn;

void srt_server_init(int conn)
{
    tcbUsedList = (tcb_t*)malloc( sizeof(tcb_t) );
    tcbUsedList->prev = tcbUsedList->next = NULL;
    tcbFreeList = (tcb_t*)malloc( sizeof(tcb_t) );
    tcbFreeList->prev = tcbFreeList->next = NULL;

    bzero(port2sock, MAX_PORT_NUM * sizeof(sock_t));

    atexit(exitDestroyList);

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

// Create a server sock
//
// This function looks up the client TCB table to find the first NULL entry, and creates
// a new TCB entry using malloc() for that entry. All fields in the TCB are initialized
// e.g., TCB state is set to CLOSED and the server port set to the function call parameter
// server port.  The TCB table entry index should be returned as the new socket ID to the server
// and be used to identify the connection on the server side. If no entry in the TCB table
// is available the function returns -1.

sock_t srt_server_sock(unsigned int port)
{
    tcb_t *node = takeFromListHead(tcbFreeList);
    if(!node) return NULL;
    assert(node->isFree);

    node->state = CLOSED;
    node->svr_portNum = port;
    inserIntoList(tcbUsedList, node);
    node->isFree = 0;

    port2sock[port] = node;

    return node;
}

// Accept connection from srt client
//
// This function gets the TCB pointer using the sockfd and changes the state of the connetion to
// LISTENING. It then starts a timer to ‘‘busy wait’’ until the TCB’s state changes to CONNECTED
// (seghandler does this when a SYN is received). It waits in an infinite loop for the state
// transition before proceeding and to return 1 when the state change happens, dropping out of
// the busy wait loop. You can implement this blocking wait in different ways, if you wish.
//

int srt_server_accept(sock_t sockfd)
{
    tcb_t *node = sock2TCB(sockfd);
    node->state = LISTENING;
    while(node->state != CONNECTED);
    printf("Accepted!\n");
	return 1;
}

// Receive data from a srt client
//
// Receive data to a srt client. Recall this is a unidirectional transport
// where DATA flows from the client to the server. Signaling/control messages
// such as SYN, SYNACK, etc.flow in both directions. You do not need to implement
// this for Lab4. We will use this in Lab5 when we implement a Go-Back-N sliding window.
//
int srt_server_recv(sock_t sockfd, void* buf, unsigned int length)
{
	return 1;
}

// Close the srt server
//
// This function calls free() to free the TCB entry. It marks that entry in TCB as NULL
// and returns 1 if succeeded (i.e., was in the right state to complete a close) and -1
// if fails (i.e., in the wrong state).
//

static inline void freeNode(tcb_t *node)
{
    removeFromList(node);
    inserIntoList(tcbFreeList, node);
    node->isFree = 1;
}

int srt_server_close(sock_t sockfd)
{
    tcb_t *node = sock2TCB(sockfd);
    if(!node || node->isFree) return -1;

    printf("close!\n");

    if(node->state == CLOSEWAIT)
        sleep(CLOSEWAIT_TIME);

    freeNode(node);

    if(node->state == CLOSEWAIT || node->state == CLOSED)
        return 1;

    printf("socket closed in a wrong state: %d!\n", node->state);

	return -1;
}

// Thread handles incoming segments
//
// This is a thread  started by srt_server_init(). It handles all the incoming
// segments from the client. The design of seghanlder is an infinite loop that calls snp_recvseg(). If
// snp_recvseg() fails then the overlay connection is closed and the thread is terminated. Depending
// on the state of the connection when a segment is received  (based on the incoming segment) various
// actions are taken. See the client FSM for more details.
//

void *seghandler(void* arg)
{
    seg_t seg;
    while(recvseg(overlayConn, &seg) > 0)
    {
        int cliPort = seg.header.src_port;
        int svrPort = seg.header.dest_port;
        unsigned int segType = seg.header.type;

        tcb_t *node = sock2TCB( port2sock[svrPort] );
        if(!node || node->isFree) continue;

        seg_t sendSeg;
        sendSeg.header.src_port = node->svr_portNum;
        sendSeg.header.dest_port = node->client_portNum;

        switch(node->state)
        {
            case CLOSED:
                printf("Wrong seg rcvd in CLOSED state!\n");
                continue;

            case LISTENING:
                if(segType == SYN)
                {
                    sendSeg.header.type = SYNACK;
                    sendseg(overlayConn, &sendSeg);

                    node->state = CONNECTED;
                    node->client_portNum = cliPort;
                }
                else
                {
                    printf("Wrong seg rcvd in LISTENING state!\n");
                    continue;
                }
                break;

            case CONNECTED:
                if(segType == SYN)
                {
                    sendSeg.header.type = SYNACK;
                    sendseg(overlayConn, &sendSeg);
                    node->state = CONNECTED;
                }
                else if(segType == FIN)
                {
                    sendSeg.header.type = FINACK;
                    sendseg(overlayConn, &sendSeg);
                    node->state = CLOSEWAIT;
                }
                else
                {
                    printf("Wrong seg rcvd in CONNECTED state!\n");
                    continue;
                }
                break;

            case CLOSEWAIT:
                if(segType == FIN)
                {
                    sendSeg.header.type = FINACK;
                    sendseg(overlayConn, &sendSeg);
                    node->state = CLOSEWAIT;
                }
                else
                {
                    printf("Wrong seg rcvd in CLOSEWAIT state!\n");
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