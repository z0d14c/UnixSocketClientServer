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
#include <stdlib.h>
#include <vector>
#include <semaphore.h> //for semaphore control
#define GetCurrentDir getcwd
#define UNIX_PATH_MAX    108
using namespace std;
char currentPath[FILENAME_MAX];
char filesPath[FILENAME_MAX];
vector<string> files;
vector<string> fileContents;
sem_t executeGuard; //used for mutual exclusion

//TODO: add arguments
//TODO: add read-from-files

//adds a file to the virtual file system
void addFile(string fileName, string fileData) {
  files.push_back(fileName);
  fileContents.push_back(fileData);
}

//does add file work
void addFileCmd(string params) {
  int separatorLoc = params.find("!!!");
  string fileName = params.substr(0, separatorLoc);
  string fileContent = params.substr(separatorLoc + 3, params.length() - 1);
  addFile(fileName, fileContent);
}

//return file content
string getFile(string params) {
  int choiceInt = atoi(params.c_str());
  string fileContent = fileContents.at(choiceInt);
  string fileName = files.at(choiceInt);
  fileContent = fileName + "!!!" + fileContent + "###";
  return fileContent;
}

//remove file from virtual system
void deleteFile(string choice) {
  int choiceInt = atoi(choice.c_str());
  choiceInt--;
  files.erase(files.begin() + choiceInt);
  fileContents.erase(fileContents.begin() + choiceInt);
}

//read files from directory
void readDirectory() {
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir (filesPath)) != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
      if (ent->d_name[0] != '.') {
        string fileContent = ""; //gets stored in vector of
        string str; //tempstring
        char localFilePath[sizeof("files/") + sizeof(ent->d_name)];
        strcpy(localFilePath, "files/"); // copy to result array
        strcat(localFilePath, ent->d_name);
        ifstream infile (localFilePath);
        if (infile.is_open())
        {
          while ( getline (infile, str) )
          {
            fileContent += str;
          }
          infile.close();
        }
        addFile(ent->d_name, fileContent);
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
  for (int i = 0; i < files.size(); i++) {
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
  string commandString(command);
  string params = commandString.substr(1, commandString.find("###"));
  cout << params << endl;
  switch (command[0]) {
  case '1': //list files
    return listFiles();
    break;
  case '2': //get file
    return getFile(params);
    break;
  case '3': //delete file
    deleteFile(params);
    return "1###";
    break;
  case '4': //add file
    addFileCmd(params);
    return "1###";
    break;
  case '5': //exit
    return "5";
    break;
  default:
    return "5";
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
  while (buffer[0] != '5') { //5 is exit
    // while(nbytes = read(connection_fd, buffer, 256) != 0){
    nbytes = read(connection_fd, buffer, 256); //reads, tells it to wait until 256 bytes have been read
    sem_wait(&executeGuard);
    response = execute(buffer);
    sem_post(&executeGuard);
    buffer[nbytes] = 0; // (start after read bytes) to be sent to client
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
//prep semaphore
  sem_init(&executeGuard, 1, 1);
//read files
  initFiles();
//set up socket stuff
  struct sockaddr_un address;
  int socket_fd, connection_fd;
  socklen_t address_length;
  pid_t cp;
  cout << "Project 3 server starting." << endl;

  socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
  if (socket_fd < 0)
  {
    printf("socket() failed\n");
    return 1;
  }

  unlink("./socket_file"); //delete demo socket file

  /* start with a clean address structure */
  memset(&address, 0, sizeof(struct sockaddr_un));

  address.sun_family = AF_UNIX;
  snprintf(address.sun_path, UNIX_PATH_MAX, "./socket_file");

  if (bind(socket_fd,
           (struct sockaddr *) &address,
           sizeof(struct sockaddr_un)) != 0)
  {
    printf("the bind function failed");
    return 1;
  }

  if (listen(socket_fd, 5) != 0)
  {
    printf("the listen function failed");
    return 0;
  }

  while ((connection_fd = accept(socket_fd,
                                 (struct sockaddr *) &address,
                                 &address_length)) > -1)
  {
    cp = fork();
    if (cp == 0)
    {
      connection_handler(connection_fd);
    }
  }

  close(socket_fd);
  unlink("./socket_file");
  return 1;
}