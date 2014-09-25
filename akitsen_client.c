// Author: Andrey Kitsen
// Networks - Project 1 Chat Client
// Worked with/Recieved Help From: Connor Riva, Peter Mata, Alex Standke

// Resources Used:
// http://www.linuxhowtos.org/data/6/client.c
// Youtube Socket Videos
// http://www.thiyagaraaj.com/tutorials/unix-network-programming-example-programs/tcp-chat-programming-in-unix-using-c-programmin
// http://www.cprogramming.com/
// http://www.thegeekstuff.com/2011/12/c-socket-programming/
// http://www.stackoverflow.com

// #include <unp.h>
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

    // takes two params, argc which is an int.
    // *argv is a char, and is a pointer.
int main(int argc, char *argv[])
{
    
    // defined variables
    // commonly used variable names in C.
    int sockfd;
    int portnumber;
    int n;

    // defined structures.
    struct sockaddr_in serv_addr; // socket address = server address.
    struct hostent *server; // hostent data struct, named server: pointer to memory location.
    struct timeval wait_time; // time delay (DONE IN CLASS TOGETHER)

    // defines our buffer size.
    char buffer[256];

    // atoi is converting string -> integer.
    portnumber = atoi(argv[2]);

    // listens if a socket is being opened.
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // throws error if argc < 3, exits.
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port ", argv[0]);
       exit(0);
    }
    
    // throws error if socket is incorrectly being opened.
    if (sockfd < 0) 
        error("There was an error opening the socket!");
    
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
    bcopy((char *)server->h_addr, // copys the bytes from the source.
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length); // takes up block of memory with h_length
    serv_addr.sin_port = htons(portnumber);
    

    // checks if the connection is working. Must = 1 to work.
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    // reads to see if socket is opened.
    n = read(sockfd, buffer, 255);
    // prints NewLine and calls the buffer size.
    printf("%s\n", buffer);

    // choose a username
    printf("Choose a name! - ");
    // zeros the buffer.
    bzero(buffer,256);

    // reads a limited number of characters from
    // given file stream source into an array of characters
    fgets(buffer,255,stdin);

    // writes the listened socket, with the buffer being the length of the string.
    n = write(sockfd,buffer,strlen(buffer));

    // if socket being written is less than 0, throws error.
    if (n < 0) 
         error("There was a problem writing to the socket...SORRY!");
    bzero(buffer,256);


    // FOLLOW WAS DONE IN CLASS 9/22/14
    // delays time for response. 
    // Explained and walked through in class by Prof. Rodkey.
    wait_time.tv_sec = 0;    
    wait_time.tv_usec = 10000; 

    // system call, 32 = maximum file descriptors, wait time.
    // DESCRIBED IN CLASS.
    select(32, NULL, NULL, NULL, &wait_time);
    n = read(sockfd, buffer, 255);
    printf("%s\n",buffer);
    bzero(buffer,256);

    // IDEA WAS GIVEN IN CLASS BY ALEX AND CONTRIBUTED TO BY OTHERS.
    // keeps the socket port open when equal to 1.
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

        printf(" :--> ");

        // Clears the buffer, and waits for new input.
        bzero(buffer,256); // clears full buffer 256

        fgets(buffer,255,stdin); // set to 255, with 1 left over.

        // if length of buffer is > 1, begins writing message.
        if (strlen(buffer) > 1) { 
            n = write(sockfd,buffer,strlen(buffer));
            bzero(buffer,256);

            // Time delayed again.
            // called again, because DID NOT WORK IN CLASS!!!
            wait_time.tv_sec = 0;    
            wait_time.tv_usec = 10000;  
            select(32, NULL, NULL, NULL, &wait_time);
        }
    }

    // closes the socket we have been using.
    close(sockfd);

    // exits out by returning 0.
    return 0;
}
