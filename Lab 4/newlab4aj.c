/*
	Gonzalez, Danilo: 28017253
	Orozco, Jonathan: 66888405
	Almazan, Adrian: 19437291
*/
	
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PROXY_LOG "proxy.log"
FILE* logfile;

void process_request(int connfd, struct sockaddr_in* clientaddr);
int parse_uri(char* uri, char* hostname, char* pathname, int* port);
void get_hostname(char* uri, char* hostname);
void get_pathname(char* uri, char* hostname, char* pathname)
void get_port(char* hostname, int* port);
void format_log_entry(char* log_entry, struct sockaddr_in* sockaddr, char* uri, int size);
int checkArguments(int argc);
int checkPortNumber(char* number);
int checkSocket();
void sigchld_handler(int sig);
// int extract_url(char* request, char* url);
int extract_hostname(char* request, char* hostname);
// int extract_port(struct request *request, char *port_char);
// int extract_pathname();

/* Functions declared below were taken from csapp.h */
#define	MAXLINE	 8192  
#define MAXBUF   8192  

typedef struct {
    int rio_fd;                
    int rio_cnt;               
    char *rio_bufptr;          
    char rio_buf[8192];
} rio_t;

typedef struct sockaddr SA;

void error(const char *msg);
void unix_error(char *msg);
void dns_error(char *msg);
void rio_readinitb(rio_t *rp, int fd);
void Rio_readinitb(rio_t *rp, int fd);
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);
ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n);
ssize_t rio_writen(int fd, void *usrbuf, size_t n);
ssize_t Rio_readn(int fd, void *ptr, size_t nbytes);
ssize_t rio_readn(int fd, void *usrbuf, size_t n);
int open_clientfd(char *hostname, int port);
int Open_clientfd(char *hostname, int port);
int open_listenfd(char *port);
/* Functions declared above were taken from csapp.h */

int main(int argc, char ** argv) {
	int listenfd;
	int port;
	int clientlen;
	int connfd;
	struct sockaddr_in clientaddr;
	struct sockaddr_in serv_addr, cli_addr;

	if (checkArguments(argc) < 1){
       exit(1);
    }
    if ((port = checkPortNumber(argv[1])) < 0){
       exit(1);
    }
    if ((listenfd = open_listenfd(argv[1])) < 0){
        exit(1);
    }

    signal(SIGCHLD, sigchld_handler);
	signal(SIGPIPE,SIG_IGN);

	// bzero((char *) &serv_addr, sizeof(serv_addr));
 //    serv_addr.sin_family = AF_INET;
 //    serv_addr.sin_addr.s_addr = INADDR_ANY;
 //    serv_addr.sin_port = htons(port);

 //    if (bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
 //       error("ERROR on binding");
 //    }
 //    listen(listenfd, 5);

	logfile = fopen(PROXY_LOG,"a");
	printf("Attempting to process requests in port: [%d]\n\n", port);

	while(1)
	{			
		clientlen = sizeof(clientaddr);
		printf("Attempting to connect... Please wait.\n");
		connfd = accept(listenfd, (SA *)&clientaddr, (socklen_t *)&clientlen);
		if (connfd < 0){
			error("Error with client accepting socket\n");
		}
		printf("Attempting to process GET request...\n");
		// close(listenfd);
		// process_request(connfd, &clientaddr);

		/* errors?? why... come back later? */
		if (fork() == 0) {
			// close(listenfd);
			process_request(connfd, &clientaddr);
		}
		else{
			close(connfd);
		}
	}

	fclose(logfile);
	exit(0);
} 

void process_request(int connfd, struct sockaddr_in* clientaddr) {	
	int serverfd; 
	char *request; 
	char *request_uri; 
	char *request_uri_end; 
	char *rest_of_request; 
	int request_len;
	int i,n; 
	char hostname[MAXLINE]; 
	char pathname[MAXLINE];
	int port;

	char log_entry[MAXLINE];

	rio_t rio;
	char buf[MAXLINE];

	request = (char*)malloc(MAXLINE);
	request[0] = '\0';
	request_len = 0;
	Rio_readinitb(&rio, connfd);
	while(1) {
		if((n = rio_readlineb(&rio, buf, MAXLINE)) <= 0) {
			printf("Client attempted a bad request... Skipping this request.\n");
			close(connfd);
			free(request);
			return;
		}
		strcat(request, buf);
		request_len += n;

		if((strcmp(buf, "\r\n") == 0)){
			break;
		}
	}
	if(strncmp(request, "GET ", strlen("get "))) {
		printf("Received non-GET request... skipping this request.\n");
		close(connfd);
		free(request);
		return;
	}
	printf("%s\n", request);
	request_uri = request + 4;
	request_uri_end = NULL;
	for(i = 0; i < request_len; ++i) {
		if(request_uri[i] == ' ') {
			request_uri[i] = '\0';
			request_uri_end = &request_uri[i]; 
			break;
		}
	}

	rest_of_request = request_uri_end + strlen("HTTP/1.0\r\n") + 1;

	parse_uri(request_uri, hostname, pathname, &port);

	serverfd = open_clientfd(hostname, port);

	// write(serverfd, request, strlen(request));
	// write(serverfd, "GET /", strlen("GET /"));
	// write(serverfd, pathname, strlen(pathname));
	// write(serverfd, " HTTP/1.0\r\n", strlen(" HTTP/1.0\r\n"));
	// write(serverfd, rest_of_request, strlen(rest_of_request));
	rio_writen(serverfd, "GET /", strlen("GET /"));
	rio_writen(serverfd, pathname, strlen(pathname));
	rio_writen(serverfd, " HTTP/1.0\r\n", strlen(" HTTP/1.0\r\n"));
	rio_writen(serverfd, rest_of_request, strlen(rest_of_request));

	Rio_readinitb(&rio, serverfd);
	int response_len = 0;

	while((n = recv(serverfd, buf, MAXLINE, 0)) > 0) {
		response_len += n;
		rio_writen(connfd, buf, n);
		bzero(buf, MAXLINE);
	}

	format_log_entry(log_entry, clientaddr, request_uri, response_len);
	fprintf(logfile, "%s\n", log_entry);
	fflush(logfile);

	close(connfd);
	close(serverfd);
	free(request);
}

void get_hostname(char* uri, char* hostname){
	char *hbegin;
	char *end;

	if (strcmp(uri,"htpp://",) !=0){
		hostname[0]= '\0';
		return;
	}

	begin = uri + 7;
	end = strchr(begin, " :/\r\n" );

	strcpy(hostname, begin);
	if (end ==  NULL){
		hostname[strlen(begin)]= '\0'
	} else{
		hostname[end-begin] = '\0'
			}

}

void get_pathname(char* uri, char* hostname, char* pathname){
	char *begin;

	begin = uri + 7;
	begin = strchr(begin, '/');
	if(begin == NULL){
		pathname[0]= '\0';

	}else{
		begin++;
		strcpy(pathname,begin);
	}

}

void get_port(char* uri, char* hostname, int* port){
	*port = 80;
	char *begin;

	begin = uri + 7;
	char* end = strchr(begin, " :/\r\n" );
	if (end != NULL && *end == ':') {
		*port = atoi(hostend + 1);
	}


}
int parse_uri(char* uri, char* hostname, char* pathname, int* port) {
	// char *hostbegin;
	// char *hostend;
	// char *pathbegin;
	// int len;

	// if(strncasecmp(uri, "http://", 7) !=0) {
	// 	hostname[0] = '\0';
	// 	return -1;
	// }
	get_hostname(uri, hostname);

	if(hostname[0]== '\0';){
		return -1;
	}



	// hostbegin = uri + 7;
	// hostend = strpbrk(hostbegin, " :/\r\n");

	// len = (hostend == NULL ? strlen(hostbegin) : hostend - hostbegin);
	// strncpy(hostname, hostbegin, len); 

	// hostname[len] = '\0';

	// *port = 80;
	// if (hostend != NULL && *hostend == ':') {
	// 	*port = atoi(hostend + 1);
	// }
	get_port(uri,hostname,port);

	get_pathname(uri,hostname,pathname)
	// pathbegin = strchr(hostbegin, '/');
	// if(pathbegin == NULL) {
	// 	pathname[0] = '\0';
	// } else {
	// 	pathbegin++;
	// 	strcpy(pathname, pathbegin);
	// }
	// return 0;

	return 0;
}

void format_log_entry(char* log_entry, struct sockaddr_in* sockaddr, char* uri, int response_len){
	time_t now;
	char time_str[MAXLINE];
	unsigned long host;
	unsigned char a, b, c, d;

	now = time(NULL);
	strftime(time_str, MAXLINE, "%a %d %b %Y %H:%M:%S %Z", localtime(&now));

	host = ntohl(sockaddr->sin_addr.s_addr);
	a = host >> 24;
	b = (host >> 16) & 0xff;
	c = (host >> 8) & 0xff;
	d = host & 0xff; 
	sprintf(log_entry, "%s: %d.%d.%d.%d %s %d", time_str, a, b, c, d, uri, response_len);
}


void sigchld_handler(int sig){
    int status;
    while (waitpid((pid_t)(-1), &status, WNOHANG) > 0) {
        ;
    }
}

int checkArguments(int argc){
    if (argc == 2){
        return 1;
    }
    else if (argc == 1){
        error("Port number not specified.\n");
        return -1;
    }
    else{
        error("Too many arguments!\n");
        return -1;
    }
}

int checkPortNumber(char* number){
    int charToNumber = atoi(number);
    if (charToNumber < 1024 || charToNumber > 65536){
        error("Port is out of range!\n");
        return -1;
    }
    else{
        return charToNumber;
    }
}

int checkSocket(){
  int socketChecker = socket(AF_INET, SOCK_STREAM, 0);
  if (socketChecker < 0) {
    error("Error with the socket!\n");
  }
  return socketChecker;
}

/* Functions provided by Professor Harris. They're located within csapp.c */
void error(const char *msg){
    perror(msg);
    exit(1);
}

void unix_error(char *msg) /* Unix-style error */
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}

void dns_error(char *msg) /* DNS-style error */
{
    fprintf(stderr, "%s: DNS error %d\n", msg, h_errno);
    exit(0);
}

int open_clientfd(char *hostname, int port) 
{
    int clientfd;
    struct hostent *hp;
    struct sockaddr_in serveraddr;

    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	return -1; /* Check errno for cause of error */

    /* Fill in the server's IP address and port */
    if ((hp = gethostbyname(hostname)) == NULL)
	return -2; /* Check h_errno for cause of error */
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

int open_listenfd(char *port) 
{
    struct addrinfo hints, *listp, *p;
    int listenfd, rc, optval=1;

    /* Get a list of potential server addresses */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;             /* Accept connections */
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; /* ... on any IP address */
    hints.ai_flags |= AI_NUMERICSERV;            /* ... using port number */
    if ((rc = getaddrinfo(NULL, port, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo failed (port %s): %s\n", port, gai_strerror(rc));
        return -2;
    }

    /* Walk the list for one that we can bind to */
    for (p = listp; p; p = p->ai_next) {
        /* Create a socket descriptor */
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) 
            continue;  /* Socket failed, try the next */

        /* Eliminates "Address already in use" error from bind */
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,    //line:netp:csapp:setsockopt
                   (const void *)&optval , sizeof(int));

        /* Bind the descriptor to the address */
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break; /* Success */
        if (close(listenfd) < 0) { /* Bind failed, try the next */
            fprintf(stderr, "open_listenfd close failed: %s\n", strerror(errno));
            return -1;
        }
    }


    /* Clean up */
    freeaddrinfo(listp);
    if (!p) /* No address worked */
        return -1;

    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, 1024) < 0) {
        close(listenfd);
	return -1;
    }
    return listenfd;
}

int Open_clientfd(char *hostname, int port) 
{
    int rc;

    if ((rc = open_clientfd(hostname, port)) < 0) {
	if (rc == -1)
	    unix_error("Open_clientfd Unix error");
	else        
	    dns_error("Open_clientfd DNS error");
    }
    return rc;
}

void rio_readinitb(rio_t *rp, int fd) 
{
    rp->rio_fd = fd;  
    rp->rio_cnt = 0;  
    rp->rio_bufptr = rp->rio_buf;
}

void Rio_readinitb(rio_t *rp, int fd)
{
    rio_readinitb(rp, fd);
} 

ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen) 
{
    int n, rc;
    char c, *bufp = usrbuf;

    for (n = 1; n < maxlen; n++) { 
        if ((rc = rio_read(rp, &c, 1)) == 1) {
	    *bufp++ = c;
	    if (c == '\n') {
                n++;
     		break;
            }
	} else if (rc == 0) {
	    if (n == 1)
		return 0; /* EOF, no data read */
	    else
		break;    /* EOF, some data was read */
	} else
	    return -1;	  /* Error */
    }
    *bufp = 0;
    return n-1;
}

ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n)
{
    int cnt;

    while (rp->rio_cnt <= 0) {  /* Refill if buf is empty */
	rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, 
			   sizeof(rp->rio_buf));
	if (rp->rio_cnt < 0) {
	    if (errno != EINTR) /* Interrupted by sig handler return */
		return -1;
	}
	else if (rp->rio_cnt == 0)  /* EOF */
	    return 0;
	else 
	    rp->rio_bufptr = rp->rio_buf; /* Reset buffer ptr */
    }

    /* Copy min(n, rp->rio_cnt) bytes from internal buf to user buf */
    cnt = n;          
    if (rp->rio_cnt < n)   
	cnt = rp->rio_cnt;
    memcpy(usrbuf, rp->rio_bufptr, cnt);
    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;
    return cnt;
}

ssize_t rio_writen(int fd, void *usrbuf, size_t n) 
{
    size_t nleft = n;
    ssize_t nwritten;
    char *bufp = usrbuf;

    while (nleft > 0) {
	if ((nwritten = write(fd, bufp, nleft)) <= 0) {
	    if (errno == EINTR)  /* Interrupted by sig handler return */
		nwritten = 0;    /* and call write() again */
	    else
		return -1;       /* errno set by write() */
	}
	nleft -= nwritten;
	bufp += nwritten;
    }
    return n;
}

ssize_t Rio_readn(int fd, void *ptr, size_t nbytes) 
{
    ssize_t n;
  
    if ((n = rio_readn(fd, ptr, nbytes)) < 0)
	unix_error("Rio_readn error");
    return n;
}

ssize_t rio_readn(int fd, void *usrbuf, size_t n) 
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = usrbuf;

    while (nleft > 0) {
	if ((nread = read(fd, bufp, nleft)) < 0) {
	    if (errno == EINTR) /* Interrupted by sig handler return */
		nread = 0;      /* and call read() again */
	    else
		return -1;      /* errno set by read() */ 
	} 
	else if (nread == 0)
	    break;              /* EOF */
	nleft -= nread;
	bufp += nread;
    }
    return (n - nleft);         /* return >= 0 */
}