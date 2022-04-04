#include <sys/socket.h>  // Unix header for sockets, using socket
#include <sys/un.h>  // defns for Unix domain sockets, using struct sockaddr_un
#include <unistd.h>  // Unix standard header, using close

#include <cassert>  // using assert
#include <cerrno>  // using errno
#include <cstddef>  // using size_t
#include <cstdlib>  // exit
#include <cstring>  // using strncpy, strerror

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <stdlib.h>
#include <stdio.h>
#include <sys/sysinfo.h>

#include "shared.h"

char *shmpath = "myshm";

std::vector<std::string> tp;

int main(int argc, char *argv[]) {
  if (argc != 2){
    return 1;
  }
  std::cout << "SERVER START" << std::endl; //Server is starting
  
  //Receiving filename and path from client through socket

  std::string filename;
  std::clog << "CLIENT REQUEST RECIEVED" << std::endl;

  //Open Shared Memory
  std::clog << "\tMEMORY OPEN" << std::endl;



  

  //Using path
  std::clog << "\tOPENING:" << filename << std::endl;


  //Opens and reads file


  return 0;
}