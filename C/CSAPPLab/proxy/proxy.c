/*
 * proxy.c - CS:APP Web proxy
 *
 * TEAM MEMBERS:
 *     Andrew Carnegie, ac00@cs.cmu.edu
 *     Harry Q. Bovik, bovik@cs.cmu.edu
 *
 * IMPORTANT: Give a high level description of your code here. You
 * must also provide a header comment at the beginning of each
 * function that describes what that function does.
 */

#include "csapp.h"

/*
 * Function prototypes
 */
int parse_uri(char *uri, char *target_addr, char *path, int  *port);
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, char *uri, int size);

static FILE *log_file;
static sem_t log_mutex;

static sem_t dns_mutex;

/*
 * open_clientfd_ts - thread-safe, open connection to server at <hostname, port>
 *   and return a socket descriptor ready for reading and writing.
 *   Returns -1 and sets errno on Unix error.
 *   Returns -2 and sets h_errno on DNS (gethostbyname) error.
 */
/* $begin open_clientfd */
int open_clientfd_ts(char *hostname, int port)
{
    int clientfd;
    struct hostent *shared_hp, *hp;
    struct sockaddr_in serveraddr;

    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1; /* check errno for cause of error */

    hp = Malloc( sizeof(struct hostent) );
    P(&dns_mutex);
    shared_hp = gethostbyname(hostname);
    *hp = *shared_hp;
    V(&dns_mutex);

    /* Fill in the server's IP address and port */
    if ((hp = gethostbyname(hostname)) == NULL)
        return -2; /* check h_errno for cause of error */

    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)hp->h_addr_list[0],
          (char *)&serveraddr.sin_addr.s_addr, hp->h_length);
    serveraddr.sin_port = htons(port);

    /* Establish a connection with the server */
    if (connect(clientfd, (SA *) &serveraddr, sizeof(serveraddr)) < 0)
	return -1;
    return clientfd;
}

// return client_fd
int rcvd_forward_request(int server_fd, char *uri_d)
{
    rio_t rio;
    Rio_readinitb(&rio, server_fd);

    char buf[MAXLINE];
    char _[MAXLINE];
    char uri[MAXLINE];

    // request line: <method> <uri> <version>
    Rio_readlineb(&rio, buf, MAXLINE);
    sscanf(buf, "%s %s %s", _, uri, _);
    printf("%s", buf);

    char hostname[MAXLINE];
    int port;
    parse_uri(uri, hostname, _, &port);
    printf("host: %s\n", hostname);

    strncpy(uri_d, uri, MAXLINE);

    int client_fd = Open_clientfd(hostname, port);

    // send request line
    Rio_writen(client_fd, buf, MAXLINE);

    while(1)
    {
        int byte_read = Rio_readlineb(&rio, buf, MAXLINE);
        if( !strcmp(buf, "\r\n") )
            break;
        printf("%s", buf);

        // send http line
        Rio_writen(client_fd, buf, byte_read);
    }

    Rio_writen(client_fd, "\r\n", 2);

    printf("\nSend Request Over!");

    return client_fd;
}


int rcvd_forward_reply(int client_fd, int server_fd)
{
    char buf[MAXLINE];
    int byte_read;

    int total_bytes = 0;

    while( (byte_read = rio_readn(client_fd, buf, MAXLINE)) > 0 )
    {
        /* printf("%s", buf); */

        // send back http line
        Rio_writen(server_fd, buf, byte_read);

        total_bytes += byte_read;
    }

    Rio_writen(client_fd, "\r\n", 2);

    printf("\nReply Over!\n\n\n");

    return total_bytes;
}


void *http_thread(void *vargp)
{
    Pthread_detach( Pthread_self() );

    int server_fd = *(int *)vargp;
    struct sockaddr_in *serversock = (struct sockaddr_in *)((char *)vargp + sizeof(int));

    char uri[MAXLINE];

    int client_fd = rcvd_forward_request(server_fd, uri);

    int bytes_rcvd = rcvd_forward_reply(client_fd, server_fd);

    char logstring[MAXLINE];
    format_log_entry(logstring, serversock, uri, bytes_rcvd);

    P(&log_mutex);
    fprintf(log_file, "%s\n", logstring);
    fflush(log_file);
    V(&log_mutex);

    Close(client_fd);
    Close(server_fd);
    Free(vargp);

    return NULL;
}


/*
 * main - Main routine for the proxy program
 */
int main(int argc, char **argv)
{
    /* Check arguments */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port number>\n", argv[0]);
        exit(0);
    }

    log_file = fopen("proxy-log", "a");
    Sem_init(&log_mutex, 0, 1);

    Sem_init(&dns_mutex, 0, 1);

    int port = atoi( argv[1] );

    int listenfd;
    if( (listenfd = open_listenfd(port)) < 0 )
        unix_error("listenfd open failed!");

    unsigned int serversock_len = sizeof( struct sockaddr_in );

    while(1)
    {
        void *serverfd_sock = Malloc( sizeof(int) + serversock_len );
        int *server_fd = (int *)serverfd_sock;
        struct sockaddr_in *serversock = (struct sockaddr_in *)(server_fd + 1);

        *server_fd = Accept(listenfd, (SA *)&serversock, &serversock_len);

        pthread_t tid;
        Pthread_create(&tid, NULL, http_thread, serverfd_sock);
    }

    fclose(log_file);
    Close(listenfd);
    exit(0);
}


/*
 * parse_uri - URI parser
 *
 * Given a URI from an HTTP proxy GET request (i.e., a URL), extract
 * the host name, path name, and port.  The memory for hostname and
 * pathname must already be allocated and should be at least MAXLINE
 * bytes. Return -1 if there are any problems.
 */
int parse_uri(char *uri, char *hostname, char *pathname, int *port)
{
    char *hostbegin = uri;
    char *hostend;
    char *pathbegin;
    int len;

    if (strncasecmp(uri, "http://", 7) == 0)
        hostbegin = uri + 7;
    else if(strncasecmp(uri, "https://", 7) == 0)
        hostbegin = uri + 8;

    /* Extract the host name */
    hostend = strpbrk(hostbegin, " :/\r\n\0");
    len = hostend - hostbegin;
    strncpy(hostname, hostbegin, len);
    hostname[len] = '\0';

    /* Extract the port number */
    *port = 80; /* default */
    if (*hostend == ':')
        *port = atoi(hostend + 1);

    /* Extract the path */
    pathbegin = strchr(hostbegin, '/');
    if (pathbegin == NULL) {
        pathname[0] = '\0';
    }
    else {
        pathbegin++;
        strcpy(pathname, pathbegin);
    }

    return 0;
}

/*
 * format_log_entry - Create a formatted log entry in logstring.
 *
 * The inputs are the socket address of the requesting client
 * (sockaddr), the URI from the request (uri), and the size in bytes
 * of the response from the server (size).
 */
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr,
                      char *uri, int size)
{
    time_t now;
    char time_str[MAXLINE];
    unsigned long host;
    unsigned char a, b, c, d;

    /* Get a formatted time string */
    now = time(NULL);
    strftime(time_str, MAXLINE, "%a %d %b %Y %H:%M:%S %Z", localtime(&now));

    /*
     * Convert the IP address in network byte order to dotted decimal
     * form. Note that we could have used inet_ntoa, but chose not to
     * because inet_ntoa is a Class 3 thread unsafe function that
     * returns a pointer to a static variable (Ch 13, CS:APP).
     */
    host = ntohl(sockaddr->sin_addr.s_addr);
    a = host >> 24;
    b = (host >> 16) & 0xff;
    c = (host >> 8) & 0xff;
    d = host & 0xff;


    /* Return the formatted log entry string */
    sprintf(logstring, "%s: %d.%d.%d.%d %s", time_str, a, b, c, d, uri);
}