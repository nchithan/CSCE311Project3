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

#include "shared.h"

const char kSocket_path[] = "";
int main(int argc, char *argv[]) {
  if (argc != 4)
    return 1;
  int filePathSize = std::string(argv[2]).size();
  int keywordSize = std::string(argv[3]).size();
  
  


  return 0;
}


// int i = 1;
// std::string lines;
// while(getline(filename, lines)){
//   std::cout << i << "\t" << lines << std::endl;
//   i++;
// }