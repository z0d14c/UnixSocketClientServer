#include <iostream>
#include <iomanip>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define UNIX_PATH_MAX    108

using namespace std;

int connection_handler(int connection_fd)
{
	int nbytes;
	 char buffer[256];
 	 char choice = '0';
	 while(choice != '5'){
	// while(nbytes = read(connection_fd, buffer, 256) != 0){
	 nbytes = read(connection_fd, buffer, 256); //reads, tells it to wait until 256 bytes have been read
	 	cout<<"server side buffer"<<endl;
	 	choice = buffer[0];
	 	cout<<"user choice is "<<choice<<endl;
		 buffer[nbytes] = 0; // (start after read bytes) to be sent to client

		 printf("MESSAGE FROM CLIENT: %s\n", buffer); //read from client
		 nbytes = snprintf(buffer, 256, "hello from the server"); //print to buffer (send to client)
		 write(connection_fd, buffer, nbytes);
 	// }
		}
	cout<<"exiting connection handler"<<endl;
 close(connection_fd);
 return 0;
}

int main(void)
{
 struct sockaddr_un address;
 int socket_fd, connection_fd;
 socklen_t address_length;
 pid_t child;
 cout<<"Project 3 server starting."<<endl;

 socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
 if(socket_fd < 0)
 {
  printf("socket() failed\n");
  return 1;
 } 

 unlink("./demo_socket"); //delete demo socket file

 /* start with a clean address structure */
 memset(&address, 0, sizeof(struct sockaddr_un));

 address.sun_family = AF_UNIX;
 snprintf(address.sun_path, UNIX_PATH_MAX, "./demo_socket"); 

 if(bind(socket_fd, 
         (struct sockaddr *) &address, 
         sizeof(struct sockaddr_un)) != 0)
 {
  printf("bind() failed\n");
  return 1;
 }

 if(listen(socket_fd, 5) != 0)
 {
  printf("listen() failed\n");
  return 1;
 }

 while((connection_fd = accept(socket_fd, 
                               (struct sockaddr *) &address,
                               &address_length)) > -1)
 {
  child = fork();
  if(child == 0)
  {
   /* now inside newly created connection handling process */
   connection_handler(connection_fd);
  }

  /* still inside server process */
  //close(connection_fd);
 }

 close(socket_fd);
 unlink("./demo_socket");
 return 0;
}