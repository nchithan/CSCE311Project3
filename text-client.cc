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
#include <vector>
#include<thread>
#include "shared.h"

std::vector<std::string> storage;

const char kSocket_path[] = "";
int main(int argc, char *argv[]) {
  if (argc != 4)
    return 1;
  int filePathSize = std::string(argv[2]).size();
  int keywordSize = std::string(argv[3]).size();

  char *shmpath = "/myshm";
  // Creating Shared mem and setting its size
  int fd = shm_open(shmpath, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
  
  if(fd == -1){
    errExit("shm_open");
  }
  if(ftruncate(fd, sizeof(struct shmbuf)) == -1){
    errExit("ftruncate");
  }

  /* Map the object into the caller's address space. */
  struct shmbuf *shmp = (shmbuf*) mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  if (shmp == MAP_FAILED){
    errExit("mmap");
  }
  //Initialize semaphores

  if(sem_init(&shmp->sem1, 1,0 ) == -1){
    errExit("sem_init-sem1");
  }
  if(sem_init(&shmp->sem2, 1, 0) == -1){
    errExit("sem_init-sem2");
  }


  //Recieving the data
  



  //Waiting for sem1
  if (sem_wait(&shmp->sem1) == -1){
    errExit("sem_wait");
  }
  //Send the filePath and keyword to server

  //Do Stuff


  /* Post 'sem2' to tell the peer that it can now access the modified data in shared memory. */

  if (sem_post(&shmp->sem2) == -1){
    errExit("sem_post");
  }

  //Printing the results of the threads
  for(int i = 0; i < storage.size(); i++){
    if(storage.at(i) == "INVALID FILE"){
      std::cerr << "INVALID FILE" << std::endl;
    }
    std::cout << (i+1) << "\t " << storage.at(i) << std::endl; 
  }

  /* Unlink the shared memory object. Even if the peer process
  is still using the object, this is okay. The object will
  be removed only after all open references are closed. */

  shm_unlink(shmpath);

  return 0;  //terminates code and returns 0
}
