#include "calljailbreakd.h"

int jailbreakd_sockfd = -1;
struct sockaddr_in jailbreakd_serveraddr;
int jailbreakd_serverlen;
struct hostent *jailbreakd_server;

void openjailbreakdsocket(){

    const char *hostname = "127.0.0.1";
    int portno = 5;
    
    jailbreakd_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (jailbreakd_sockfd < 0)
        printf("calljailbreakd: ERROR opening socket\n");
    
    /* gethostbyname: get the server's DNS entry */
    jailbreakd_server = gethostbyname(hostname);
    if (jailbreakd_server == NULL) {
        printf("ERROR, no such host as %s\n",hostname);
        return;
    }
    
    /* build the server's Internet address */
    bzero((char *) &jailbreakd_serveraddr, sizeof(jailbreakd_serveraddr));
    jailbreakd_serveraddr.sin_family = AF_INET;
    bcopy((char *)jailbreakd_server->h_addr,
          (char *)&jailbreakd_serveraddr.sin_addr.s_addr, jailbreakd_server->h_length);
    jailbreakd_serveraddr.sin_port = htons(portno);
    
    jailbreakd_serverlen = sizeof(jailbreakd_serveraddr);
}

void calljailbreakd(const char *dylib) {
    
    if (jailbreakd_sockfd == -1) {
        openjailbreakdsocket();
    }
    
	#define BUFSIZE 2000
    
    int n;
    char buf[BUFSIZE];
    
    /* get a message from the user */
    bzero(buf, BUFSIZE);
    
    struct JAILBREAKD_FIXUP_DYLIB dylibPacket;
    dylibPacket.Command = JAILBREAKD_COMMAND_FIXUP_DYLIB;
    strcpy(dylibPacket.dylib, dylib);
    
    memcpy(buf, &dylibPacket, sizeof(dylibPacket));
    
    n = sendto(jailbreakd_sockfd, buf, sizeof(struct JAILBREAKD_FIXUP_DYLIB), 0, (const struct sockaddr *)&jailbreakd_serveraddr, jailbreakd_serverlen);
    if (n < 0){
        printf("calljailbreakd: Error in sendto\n");
    }
}

void closejailbreakfd(void) {
    close(jailbreakd_sockfd);
    jailbreakd_sockfd = -1;
}
