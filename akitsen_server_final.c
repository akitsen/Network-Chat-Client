// Author : Andrey Kitsen


// akitsen_server.c

// Code examples taken from www.linuxhowtos.org
// Example Sever built from : http://www.gnu.org/software/libc/manual/html_node/Server-Example.html
// Stackoverflow.com
// http://www.linuxhowtos.org/C_C++/socket.htm
// http://www.thegeekstuff.com/2011/12/c-socket-programming/
// http://www.tenouk.com/Module41.html
//
// Used many examples of client servers.

// Received help from: Connor Riva, Peter Mata, Alex Standke, Mason, Adam.




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>

  int sockfd;
  int newsockfd;
  int portno;
  int n;
  int connections;
  int connfds[128];
  int i;
  int nbytes;

////////////////////////

  char buffer[512];
  char input[512];
  char usernames[1000][20];
  char name;

////////////////////////

  struct sockaddr_in serv_addr, cli_addr;
  struct sockaddr_in clientname;
  
// Top Level Error Handling
void error(const char *msg)
{
  perror(msg);
  exit(1);
}

bool starts_with(const char *a, const char *b)
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}

int main(int argc, char *argv[])
{

  fprintf(stderr, "Server: connecting at %s", argv[1]);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);


  bzero((char *) &serv_addr, sizeof(serv_addr)); // clears server address

  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  // Bind the connecting socket 
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    error("ERROR on binding");

  listen(sockfd,5);

  fd_set active_fd_set, read_fd_set;
  
  socklen_t size;
  
  FD_ZERO (&active_fd_set);
  FD_SET (sockfd, &active_fd_set);

  fprintf(stderr, "Server is Ready, Please continue...\n");

  while (1) {
    read_fd_set = active_fd_set;

    // use SELECT to make connection, and continue on.
    if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
      error("ERROR in select");
    }

    // The server will go through every possible connection port to find a possible port.
    for (i = 0; i < FD_SETSIZE; i++) {
      if (FD_ISSET(i, &read_fd_set)) {
        
        // Makes a connection
        if (i == sockfd) {
          int new;
          size = sizeof (clientname);
          new = accept (sockfd, (struct sockaddr *) &clientname, &size);
          if (new < 0) {
            error("ERROR in accept");
          }
          fprintf (stderr,
                   "Server: connection from %i, at port # %hd",
                   inet_ntoa (clientname.sin_addr),
                   ntohs (clientname.sin_port));

          FD_SET (new, &active_fd_set);

        } 

        
        // Handles any new info/inputs from an already connected user.

        else {
          bzero(buffer, 512);
          bzero(input, 512);

          // closes the socket if error or user disconnects.
          if (read (i, buffer, 512) <= 0) {
            strcpy(input, usernames[i]);
            close (i);
            FD_CLR (i, &active_fd_set);
          } 

          else {

            // Successful new user to chat server.

            if (strcmp(usernames[i], "Connection Made") == 0) {
              strcpy(usernames[i], strtok(buffer, "\n"));
              strcpy(input, usernames[i]);
              strcat(input, " Now part of Chat\n");
              write(i, "Successfully connected, please begin chatting. Help = /h.\n", 512);
            }

            // Handles inputs from user.
            else {

              // Any server commands?

            	// Ping to server. Cool server trick shown by Alex.
            	// What ever user wanted Ping info, sends back info.
              if (starts_with(buffer, "/")) {
                if (starts_with(buffer, "/ping")) {
                  fprintf(stderr, "Server: Ping from %s\n", usernames[i]);
                  write(i,"Pong!\n",6);
                  continue;
                }

                // shows info about the user.
                else if (starts_with(buffer, "/me ")) {
                  strcpy(input, usernames[i]);
                  strcat(input, " ");
                  memmove(buffer, buffer+4, strlen(buffer) - 4 + 1);
                  strcat(input, buffer);
                }
               
                else {
                  strcpy(input, "Server: Commands available:\n");
                  strcat(input, " /ping\t\tPong!\n");
                  strcat(input, " /me <input>\tDo actions to yourself.\n");
                  continue;
                }
              }

              // Inputs made by users.
              else {
                strcpy(input, usernames[i]);
                strcat(input, ": ");
                strcat(input, buffer);
              }
            }
          }

          // Pushes input to all other clients connected.
          fprintf(stderr, "%s", input);
          for (n = 0; n < FD_SETSIZE; n++) {
            if (FD_ISSET(n, &active_fd_set) && n != sockfd) {
              write(n,input,512);
            }
          }
        }
      }
    }
  }

  close(sockfd);
  return 0; 
}