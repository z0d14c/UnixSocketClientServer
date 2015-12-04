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
//display menu to da user
char menu() {
	string input = "";
	cout<<"CLIENT MENU:"<<endl;
	cout<<"1. Option 1"<<endl;
	cout<<"2. Option 2"<<endl;
	cout<<"3. Option 3"<<endl;
	cout<<"4. Option 4"<<endl;
	cout<<"0. Exit"<<endl;
	getline(cin, input);
	return input[0];
}

int main(int argc, char* args[]){
	struct sockaddr_un address; //sockaddr_un means a unix domain socket (instead of internet domain)
	int  socket_fd, nbytes;
	cout<<"Project 3 client started."<<endl;

	//SOCK_STREAM is socket communication type, SOCK_STREAM two-way, reliable connection
	socket_fd = socket(PF_UNIX, SOCK_STREAM, 0); // arg1 is communication domain, PF_UNIX is domain for local communication

	 if(socket_fd < 0) //check for socket opening failure
	 {
	  printf("socket() failed\n");
	  return 1;
	 }

	 memset(&address, 0, sizeof(struct sockaddr_un)); //sets up clean memory
	 
	 address.sun_family = AF_UNIX; //set sun_family to AF_UNIX 
	 snprintf(address.sun_path, 100, "./demo_socket");

	 if(connect(socket_fd, 
	            (struct sockaddr *) &address, 
	            sizeof(struct sockaddr_un)) != 0)
	 {
	  printf("connect() failed\n");
	  return 1;
	 }
	char choice = '0'; // initialize to 0
	while(choice != '5') {
		choice = menu();
		char buffer[256];
		 nbytes = snprintf(buffer, 256, "%c", choice);
		 write(socket_fd, buffer, nbytes);
		 
		 nbytes = read(socket_fd, buffer, 256); //reads, tells it to wait until 256 bytes have been read
		 buffer[nbytes] = 0;

		 printf("MESSAGE FROM SERVER: %s\n", buffer);
	 }
	 close(socket_fd); //close socket connection
	 cout<<"socket fd "<<socket_fd<<endl;

	 return 0;
}

