// Andrey Kitsen
// Networks - Project 1 Chat Client
// Resources Used:
// Worked with Connor Riva, Peter Mata, Alex Standke

// http://www.linuxhowtos.org/data/6/client.c
// Youtube Socket Videos
// http://www.thiyagaraaj.com/tutorials/unix-network-programming-example-programs/tcp-chat-programming-in-unix-using-c-programmin
// http://www.cprogramming.com/
// http://www.thegeekstuff.com/2011/12/c-socket-programming/
// http://www.stackoverflow.com


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h> 
// #include <unp.h>

    // takes two params, argc which is an int.
    // *argv is a char, and is a pointer.
int main(int argc, char *argv[])
{

    //struct sockaddr_in    servaddr;
    //struct sockaddr SA;
    /*
    void err_sys(const char* x){
        perror(x)
        exit(1);
    }
    */

    // defined variables
    int sockfd;
    int portnumber;
    int n;

    // defined structures.
    struct sockaddr_in serv_addr;
    struct hostent *server;
    struct timeval wait_time;

    // defines our buffer size.
    char buffer[256];
    

    if (argc != 2)
        printf("usage: a.out <IPaddress>");

    // throws error if argc < 3, exits.
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port ", argv[0]);
       exit(0);
    }

    // atoi is converting string -> integer.
    portnumber = atoi(argv[2]);

    // listens if a socket is being opened.
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    // throws error if socket is incorrectly being opened.
    if (sockfd < 0) 
        error("ERROR opening socket");
    
    // defines server as the hostname(pilot.westmont.edu)
    server = gethostbyname(argv[1]);
    
    // checks if there is a server host, prints error if no server.
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    
    // Zero's out the server address for connection.
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portnumber);
    

    // checks if the connection is working. Must be a 1 to work.
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");


    n = read(sockfd, buffer, 255);
    printf("%s\n", buffer);

    // choose a username
    printf("Choose a name! - ");
    // zeros the buffer.
    bzero(buffer,256);

    // reads a limited number of characters from
    // given file stream source into an array of characters
    fgets(buffer,255,stdin);

    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);

    // delays time for response. 
    // Explained and walked through in class by Prof. Rodkey.
    wait_time.tv_sec = 0;    
    wait_time.tv_usec = 10000; 

    // system call, 32 = maximum file descriptors, wait time.
    select(32, NULL, NULL, NULL, &wait_time);
    n = read(sockfd, buffer, 255);
    printf("%s\n",buffer);
    bzero(buffer,256);

    while (1) {

        // Scans to see if data is ready to be read.
        int count;
        ioctl(sockfd, FIONREAD, &count);

        // If the count is NOT 0,
        // Zero's out buffer, and reads the data in buffer.
        // prints out buffer.
        if (count != 0) {
            bzero(buffer,256);
            n = read(sockfd, buffer, 255);
            printf("%s\n",buffer);
        }
        
        // Clears the buffer, and waits for new input.
        printf("> ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);

        // if length of buffer is > 1, writes the message.
        if (strlen(buffer) > 1) { 
            n = write(sockfd,buffer,strlen(buffer));
            bzero(buffer,256);

            // Time delay
            wait_time.tv_sec = 0;    
            wait_time.tv_usec = 10000;  
            select(32, NULL, NULL, NULL, &wait_time);
        }
    }

    // closes the open socket.
    close(sockfd);

    // exits out with 0.
    return 0;
}
