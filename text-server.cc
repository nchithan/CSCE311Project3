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

  std::string filepath;
  std::string searchString;
  int len = 0;
  

  std::clog << "CLIENT REQUEST RECIEVED" << std::endl;

  //Open Shared Memory
  int fd = shm_open(shmpath, O_RDWR, 0);
  if (fd == -1){
    errExit("shm_open");
  }
  struct shmbuf *shmp = (shmbuf*) mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  std::clog << "\tMEMORY OPEN" << std::endl;



  /* Copy data into the shared memory object. */
  std::ifstream file;
  //Opening file
  std::clog << "\tOPENING: " << filepath << std::endl;
  file.open(filepath);
  if(file.is_open()){
    std::string line;
    while (std::getline(file,line)){
      if(line.find(searchString) != std::string::npos){
        char *charline = &line[0];
        len = strlen(charline);
        shmp->cnt = len;
        memcpy(&shmp->buf, charline, len);
        /* Tell peer that it can now access shared memory. */
        if (sem_post(&shmp->sem1) == -1)
          errExit("sem_post");
        /* Wait until peer says that it has finished accessing
        the shared memory. */
        if (sem_wait(&shmp->sem2) == -1){
          errExit("sem_wait");
        }
      }
    }


    std::clog << "\tFILE CLOSED" << std::endl;
  } else {
    const char *NOTVALID = "INVALID FILE";
    len = strlen(NOTVALID);
    shmp->cnt = len;
    memcpy(&shmp->buf, NOTVALID, len);
  }
  
  shm_unlink(shmpath);
  std::clog << "\tMEMORY CLOSE" << std::endl;

  // /* Tell peer that it can now access shared memory. */

  // if (sem_post(&shmp->sem1) == -1)
  //   errExit("sem_post");

  // /* Wait until peer says that it has finished accessing
  // the shared memory. */

  // if (sem_wait(&shmp->sem2) == -1){
  //   errExit("sem_wait");
  // }

  return 0;
}