#include <iostream>
#include <iomanip>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define UNIX_PATH_MAX    108

using namespace std;

//list files on server
void listFiles(string input) {
  int delimLoc = input.find("!!!");
  int endDelimLoc = input.find("###");
  int initLoc = 0;
  int numFiles = 0;
  cout << "**** LIST OF FILES ****" << endl;
  if (endDelimLoc > 1) {
    do {
      numFiles++;
      string fileName = input.substr(initLoc, delimLoc);
      delimLoc = input.find("!!!", initLoc);
      initLoc = delimLoc + 3;
      cout << numFiles << ". " << fileName << endl;
    } while (delimLoc != endDelimLoc - 3 && endDelimLoc > 1);
  }
}

void saveFile(string input){
  int endDelimLoc = input.find("###");
  int separatorLoc = input.find("!!!");
  string fileName = input.substr(0, separatorLoc);
  string fileContent = input.substr(separatorLoc+3, endDelimLoc);
  ofstream outputFile(fileName.c_str());
  outputFile << fileContent;
  outputFile.close();
}

//interprets server response and outputs relevant info
string interpret(char command, string input) {
  cout<<"command "<<command<<endl;
  cout<<"input "<<input<<endl;
  switch (command) {
  case '1': //list files
    listFiles(input);
    break;
  case '2': //getFile
    saveFile(input);
    break;
  case '3': //remove file
    break;
  case '4':
    break;
  case '5': //quit
    break;
  default:
    break;
  }
  return "lol";
}

//accepts input from user on what to deletes
char deleteFile() {
  char fileSelection;
  cout << "Please enter the file number you would like to delete" << endl;
  cin >> fileSelection;
  return fileSelection;
}

//accepts input from user on what to deletes
char getFile() {
  char fileSelection;
  cout << "Please enter the file number you would like to get" << endl;
  cin >> fileSelection;
  return fileSelection;
}

//for adding a file to server
string addFile(){
  string fileSelection;
  cout << "Please enter the file name you would like to add" << endl;
  cin >> fileSelection;
  fileSelection += "!!!FILLERCONTENT";
  fileSelection += "###";
  return fileSelection;
}

//allows user to add parameters, if applicable, to the command
string processMenuChoice(char command) {
  string commandAndParam = "";
  commandAndParam += command;
  switch (command) {
  case '1': //list files
    return "1";
    break;
  case '2': //get file
    commandAndParam += getFile();
    return commandAndParam;
    break;
  case '3': //remove file
    commandAndParam += deleteFile();
    return commandAndParam;
    break;
  case '4': //add file
    commandAndParam += addFile();
    return commandAndParam;
    break;
  case '5': //exit
    return "5";
    break;
  default:
    break;
  }
}
//display menu to da user
string menu() {
  string input = "";
  cout << "CLIENT MENU:" << endl;
  cout << "1. Display names of files" << endl;
  cout << "2. Get a file" << endl;
  cout << "3. Remove a file" << endl;
  cout << "4. Add a new file" << endl;
  cout << "0. Exit" << endl;
  cin >> input;
  return processMenuChoice(input[0]);
}



int main(int argc, char* args[]) {
  struct sockaddr_un address; //sockaddr_un means a unix domain socket (instead of internet domain)
  int  socket_fd, nbytes;
  cout << "Project 3 client started." << endl;

  //SOCK_STREAM is socket communication type, SOCK_STREAM two-way, reliable connection
  socket_fd = socket(PF_UNIX, SOCK_STREAM, 0); // arg1 is communication domain, PF_UNIX is domain for local communication

  if (socket_fd < 0) //check for socket opening failure
  {
    printf("socket() no bueno\n");
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
  char buffer[256];
  buffer[0] = '0'; // initialize to 0
  while (buffer[0] != '5') {
    string choice = menu();
    cout << "choice is " << choice << endl;
    nbytes = snprintf(buffer, 256, "%s", choice.c_str()); // write into buffer
    write(socket_fd, buffer, nbytes); //send across socket
    //wait for response
    nbytes = read(socket_fd, buffer, 256); //reads, tells it to wait until 256 bytes have been read
    string tempString = string(buffer);
    int endDelimLoc = tempString.find("###");
    string relevantInput = tempString.substr(0, endDelimLoc + 3);
    interpret(choice[0], relevantInput);

    //printf("MESSAGE FROM SERVER: %s\n", buffer);
  }
  close(socket_fd); //close socket connection
  cout << "socket fd " << socket_fd << endl;

  return 0;
}

