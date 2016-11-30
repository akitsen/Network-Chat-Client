// Author: Andrey Kitsen
// Networks - Project 2 Chat Client

// BUILT ON TOP OF CLIENT1 project. Code commented out is from previous project part 1.

// Multiple sites and reference were used, including in class discussions, lecture notes, online resources, and working together in groups in the CS Lounge.

// Worked with/Recieved Help From: Connor Riva, Peter Mata, Alex Standke, Mason, Adam

// Resources Used:
// http://www.mkssoftware.com/docs/man3/select.3.asp
// http://www.tutorialspoint.com/c_standard_library/c_function_memcpy.htm
// http://www.linuxhowtos.org/data/6/client.c
// Youtube Socket Videos
// http://www.thiyagaraaj.com/tutorials/unix-network-programming-example-programs/tcp-chat-programming-in-unix-using-c-programmin
// http://www.cprogramming.com/
// http://www.thegeekstuff.com/2011/12/c-socket-programming/
// http://www.stackoverflow.com

// Included C libraries
#include <sys/select.h>
#include <netinet/in.h>
#include <readline/readline.h>
#include <errno.h>
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/socket.h>

static char local_buffer[1024+1];

    // takes two params, argc which is an int.
    // *argv is a char, and is a pointer.
int main(int argc, char *argv[]) // main begins
{
    
    // defined variables
    // commonly used variable names in C.
    int sockfd;
    int portnumber;
    int n;
    int FD_ISSET(int sockfd, fd_set *set); // pulled from tutorialspoint.com

    // defined structures.
    struct sockaddr_in serv_addr; // socket address = server address.
    struct hostent *server; // hostent data struct, named server: pointer to memory location.
    struct timeval wait_time; // time delay (DONE IN CLASS TOGETHER)
    fd_set readfd;
    fd_set readcpyfd;

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
        fprintf(stderr,"No Host Server\n");
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
        error("There was a Socket Connection Issue");

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

    //wait_time.tv_sec = 0;    
    //wait_time.tv_usec = 10000; 
    // system call, 32 = maximum file descriptors, wait time.
    // DESCRIBED IN CLASS.
    //select(32, NULL, NULL, NULL, &wait_time);
    n = read(sockfd, buffer, 255);
    printf("%s\n",buffer);
    bzero(buffer,256);

    // all the following are MACROS provided by SELECT.
    ///////////////////////////////////////////////////////////////////////

    // first SELECT uses two macros to clear the buffer from previous SETS.
    FD_ZERO(&readfd); // clears the readfd 
    FD_ZERO(&readcpyfd); //clears read/copyfd

    // SELECT has now cleared out the buffer, and now will start listening for new inputs to set.
    FD_SET(sockfd,&readfd); // listens to the sockfd and readfd
    FD_SET(0,&readfd);    // uses the stdin value and listens to the sockedfd


  while(1)
  {
    // first , fflush will flush the current buffer prior to using it agian.    
    fflush(stdin); // fflush clears the input buffer (idea from :http://stackoverflow.com/questions/2187474/i-am-not-able-to-flush-stdin)
    // could not get c != EOF working

    n = select((sizeof(fd_set)),&readcpyfd,NULL,NULL,NULL); // similar to the select we used in proj. 1, but now uses the maximum buffer and thee read/copy fd.

    if(n == -1){
      error("Error with n\n"); //reports error of n = -1
    }

    if (FD_ISSET(sockfd,&readcpyfd)) // checks to see if there is already something SET, and what n is set to.
    {
      n = recv(sockfd,local_buffer,1024,0); // Allows for n to receive input from the socket with the local buffer.

      if(n == -1)
      {
        error("ERROR OCCURED");
      }

      else if (n == 0) // Exits
      {
        printf("Goodbye\n");
        exit(0); // previewed exit by Profesor Thomas Cantrell in class. "graceful close"
      }

    }
      } 
  } 
} 
// closes the socket we have been using, prior to exiting program client.
    close(sockfd);

    // exits out by returning 0.
    return 0;
}
