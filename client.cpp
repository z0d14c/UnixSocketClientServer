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

void listFiles(string input) {
  int delimLoc = input.find("!!!");
  int endDelimLoc = input.find("###");
  int initLoc = 0;
  int numFiles = 0;
  cout << "**** LIST OF FILES ****" << endl;
  while (delimLoc != endDelimLoc - 3) {
    numFiles++;
    string fileName = input.substr(initLoc, delimLoc);
    delimLoc = input.find("!!!", initLoc);
    initLoc = delimLoc + 3;
    cout << numFiles << ". " << fileName << endl;
  }
}

//interprets server response and outputs relevant info
string interpret(char command, string input) {
  switch (command) {
  case '1':
    listFiles(input);
    break;
  case '2':

    break;
  case '3':
    break;
  case '4':
    break;
  case '5':
    break;
  default:
    break;
  }
  return "lol";
}

//display menu to da user
char menu() {
  string input = "";
  cout << "CLIENT MENU:" << endl;
  cout << "1. Display names of files" << endl;
  cout << "2. Get a file" << endl;
  cout << "3. Remove a file" << endl;
  cout << "4. Add a new file" << endl;
  cout << "0. Exit" << endl;
  getline(cin, input);
  return input[0];
}

//allows user to add parameters, if applicable, to the command
string processMenuChoice(char command) {
  switch (command) {
  case '1':
    return "1";
    break;
  case '2':

    break;
  case '3':
    break;
  case '4':
    break;
  case '5':
    break;
  default:
    break;
  }
}

int main(int argc, char* args[]) {
  struct sockaddr_un address; //sockaddr_un means a unix domain socket (instead of internet domain)
  int  socket_fd, nbytes;
  cout << "Project 3 client started." << endl;

  //SOCK_STREAM is socket communication type, SOCK_STREAM two-way, reliable connection
  socket_fd = socket(PF_UNIX, SOCK_STREAM, 0); // arg1 is communication domain, PF_UNIX is domain for local communication

  if (socket_fd < 0) //check for socket opening failure
  {
    printf("socket() failed\n");
    return 1;
  }

  memset(&address, 0, sizeof(struct sockaddr_un)); //sets up clean memory

  address.sun_family = AF_UNIX; //set sun_family to AF_UNIX
  snprintf(address.sun_path, 100, "./demo_socket");

  if (connect(socket_fd,
              (struct sockaddr *) &address,
              sizeof(struct sockaddr_un)) != 0)
  {
    printf("connect() failed\n");
    return 1;
  }
  char choice = '0'; // initialize to 0
  while (choice != '5') {
    choice = menu();
    char buffer[256];
    nbytes = snprintf(buffer, 256, "%c", choice);
    write(socket_fd, buffer, nbytes);

    nbytes = read(socket_fd, buffer, 256); //reads, tells it to wait until 256 bytes have been read
    interpret(choice, buffer);
    buffer[nbytes] = 0;

    //printf("MESSAGE FROM SERVER: %s\n", buffer);
  }
  close(socket_fd); //close socket connection
  cout << "socket fd " << socket_fd << endl;

  return 0;
}

