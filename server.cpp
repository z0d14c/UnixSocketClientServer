#include <iostream> //input/output stream
#include <iomanip>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <dirent.h>
#include <sys/un.h>
#include <stdio.h>
#include <fstream>  //write to file
#include <sstream> //write to memory (string)
#include <string.h>
#include <unistd.h>
#include <vector>
#define GetCurrentDir getcwd
#define UNIX_PATH_MAX    108
using namespace std;
char currentPath[FILENAME_MAX];
char filesPath[FILENAME_MAX];
vector<string> files;

void readDirectory() {
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir (filesPath)) != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
      if (ent->d_name[0] != '.') {
        files.push_back(ent->d_name);
      }
    }
    closedir (dir);
  }
}

//return file list for display command
//!!! is delimeter between files
//### is end of message delimiter
string listFiles() {
  string filesList = "";
  for(int i=0; i<files.size(); i++){
    filesList = filesList + files.at(i) + "!!!";
  }
  filesList = filesList + "###";
  return filesList;
}

//reads files in initially
void initFiles() {
  cout << "reading files in files/ dir" << endl;
  GetCurrentDir(currentPath, sizeof(currentPath));
  snprintf(filesPath, sizeof(filesPath), "%s%s", currentPath, "/files");
  readDirectory();
}

//translate client command
//1 display
//2 get file
//3 remove file
//4 add new file
//pattern is [0] = command [1 to indexOf("###")] = parameters
// returns '1' for success, '0' for failure, or a string of delimited files for display
string execute(char* command) {
  switch(command[0]){
    case '1':
      return listFiles();
      break;
    case '2':
    case '3':
    case '4':
    case '5':
      return "1";
      break;
    default:
      return "0";
      break;
  }
}

//handles the connection, does the high-level stuff
//buffer[0] is command, rest is parameters/delimiters/empty
int connection_handler(int connection_fd)
{
  int nbytes;
  char buffer[256];
  buffer[0] = '0'; //0 means user hasn't chosen yet! aka connection just started
  string response;
  while (buffer[0] != '5') {
    // while(nbytes = read(connection_fd, buffer, 256) != 0){
    nbytes = read(connection_fd, buffer, 256); //reads, tells it to wait until 256 bytes have been read
    response = execute(buffer);
    buffer[nbytes] = 0; // (start after read bytes) to be sent to client

    //printf("MESSAGE FROM CLIENT: %s\n", buffer); //read from client
    nbytes = snprintf(buffer, 256, "%s", response.c_str()); //print to buffer (send to client)
    write(connection_fd, buffer, nbytes);
    // }
  }
  cout << "exiting connection handler" << endl;
  close(connection_fd);
  return 0;
}

int main(void)
{
//read files
  initFiles();
//set up socket stuff
  struct sockaddr_un address;
  int socket_fd, connection_fd;
  socklen_t address_length;
  pid_t child;
  cout << "Project 3 server starting." << endl;

  socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
  if (socket_fd < 0)
  {
    printf("socket() failed\n");
    return 1;
  }

  unlink("./demo_socket"); //delete demo socket file

  /* start with a clean address structure */
  memset(&address, 0, sizeof(struct sockaddr_un));

  address.sun_family = AF_UNIX;
  snprintf(address.sun_path, UNIX_PATH_MAX, "./demo_socket");

  if (bind(socket_fd,
           (struct sockaddr *) &address,
           sizeof(struct sockaddr_un)) != 0)
  {
    printf("bind() failed\n");
    return 1;
  }

  if (listen(socket_fd, 5) != 0)
  {
    printf("listen() failed\n");
    return 1;
  }

  while ((connection_fd = accept(socket_fd,
                                 (struct sockaddr *) &address,
                                 &address_length)) > -1)
  {
    child = fork();
    if (child == 0)
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