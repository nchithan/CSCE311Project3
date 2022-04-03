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

std::vector<std::string> tp;

class UnixDomainSocket {
 public:
  explicit UnixDomainSocket(const char *socket_path) {
    socket_path_ = std::string(socket_path);  // std::string manages char *

    sock_addr_ = {};  // init struct (replaces memset)
    sock_addr_.sun_family = AF_UNIX;  // set to Unix domain socket (e.g. instead
                                      //   of internet domain socket)
    // leaving leading null char sets abstract socket
    strncpy(sock_addr_.sun_path + 1,  // use strncpy to limit copy for
            socket_path,              //   portability
            sizeof(sock_addr_.sun_path) - 2);  // -2 for leading/trailing \0s
  }

 protected:
  ::sockaddr_un sock_addr_;  // socket address from sys/un.h

  std::string socket_path_;  // let std::string manage char *
};


// Domain Socket Server C++ Interface Class
//
class DomainSocketServer : public UnixDomainSocket {
 public:
  using ::UnixDomainSocket::UnixDomainSocket;

  void RunServer() const {
    int sock_fd;  // unnamed socket file descriptor
    int client_req_sock_fd;  // client connect request socket file descriptor

    // (1) create a socket
    //       AF_UNIX -> file system pathnames
    //       SOCK_STREAM -> sequenced bytestream
    //       0 -> default protocol (let OS decide correct protocol)
    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if ( sock_fd < 0 ) {
      std::cerr << strerror(errno) << std::endl;
      exit(-1);
    }

    // (2) bind socket to address for the server
    unlink(socket_path_.c_str());  // sys call to delete file if it exists
                                   // already using Unix system calls for
                                   //   sockets, no reason to be non-Unix
                                   //   portable now.  :-/
    int success = bind(sock_fd,
                       // sockaddr_un is a Unix sockaddr and so may be cast "up"
                       //   to that pointer type (think of it as C polymorphism)
                       reinterpret_cast<const sockaddr*>(&sock_addr_),
                       // size needs be known due to underlying data layout,
                       //   i.e., there may be a size difference between parent
                       //   and child
                       sizeof(sock_addr_));
    if (success < 0) {
      std::cerr << strerror(errno) << std::endl;
      exit(-1);
    }

    // (3) Listen for connections from clients
    size_t kMax_client_conns = get_nprocs_conf() - 1;

    std::clog << "SERVER STARTED" << std::endl;
    std::clog << "\t MAX CLIENTS: " << kMax_client_conns << std::endl;
    success = listen(sock_fd, kMax_client_conns);
    if (success < 0) {
      std::cerr << strerror(errno) << std::endl;
      exit(-1);
    }

    const size_t kWrite_buffer_size = 64;
    char write_buffer[kWrite_buffer_size];
    int bytes_write;
    while (true) {
      // (4) Accept connection from a client
      client_req_sock_fd = accept(sock_fd, nullptr, nullptr);
      if (client_req_sock_fd  < 0) {
        std::cerr << strerror(errno) << std::endl;
        continue;
      }

      std::clog << "CLIENT CONNECTED" << std::endl;

      // (5) Receive data from client(s)
      bytes_write = read(client_req_sock_fd, write_buffer, kWrite_buffer_size);
      // bool found_E = false;
      // bool found_N = false;
      // bool found_D = false;
      int bytes_writeClone = bytes_write;
      for(int i =0; i < kWrite_buffer_size-3; i++){
        if(write_buffer[i] == 'E' && write_buffer[i+1] == 'N' && write_buffer[i+2] == 'D'){
          bytes_write = bytes_write - (i+2);
        }
      }
      std::cout << "Byte_write: " << bytes_write << std::endl;

      const char kKill_msg[] = "quit";  // TODO(lewisjs): trim whitespace
                                        //   from read_buffer for comparison

      std::string filename(write_buffer,bytes_write);
      std::clog << "PATH: " << filename << std::endl;

      bytes_write = bytes_writeClone - bytes_write;
      std::string NeedFind(write_buffer,bytes_write);
      std::clog << "SEEKING: " << NeedFind << std::endl;
      
      while (bytes_write > 0) {
        if (strcmp(write_buffer, kKill_msg) == 0) {
          std::cout << "Server shutting down..." << std::endl;

          bytes_write = 0;  // message handled, disconnect client
          break;
        }

        //std::cout << "read " << bytes_write << " bytes: ";
        //std::clog.write(write_buffer, bytes_write) << std::endl;

        std::fstream file;

         
        ssize_t bytes_wroteTotal = 0;
        file.open(filename, std::ios::in);
        if (file.is_open()){   //checking whether the file is open
          if(!(filename.find("dat/anna_karenina.txt") && filename.find("dat/dante.txt") && filename.find("dat/lorem_ipsum.txt") == std::string::npos)){
            std::string line;
            while(std::getline(file, line)){ //read data from file object and put it into string.
            //tp.push_back(line); //print the data of the string
            std::cout << line << std::endl;
              if(line.find(NeedFind) > 0){
                //Check
        //         write(client_req_sock_fd, write_buffer, line.size());
        //         std::cin.getline(write_buffer, kWrite_buffer_size);
        //         while (std::cin.gcount() > 0) {
        //           if (std::cin.gcount() == kWrite_buffer_size - 1 && std::cin.fail())
        //             std::cin.clear();
        // // write() is equivalent to send() with no flags in send's 3rd param
        //             ssize_t bytes_wrote = write(client_req_sock_fd, write_buffer, std::cin.gcount());
        //             std::clog << "sent " << std::cin.gcount() << " bytes" << std::endl;
                //}
              }
            }
      
      //tp.find(Needfind);
      
      
       //close the file object.
          file.close();
          }
          
        } 
   

        //bytes_write = read(client_req_sock_fd, write_buffer, kWrite_buffer_size);
        //std::clog << "PATH: "  << by<< std::endl;
        //std::clog << "BYTES SENT: " << bytes_write << std::endl;
      }

      if (bytes_write == 0) {
        std::cout << "Client disconnected" << std::endl;

        close(client_req_sock_fd);
      } else if (bytes_write < 0) {
        std::cerr << strerror(errno) << std::endl;

        exit(-1);
      }
    }
  }
};

const char kSocket_path[] = "";
int main(int argc, char *argv[]) {
  if (argc != 2)
    return 1;
  DomainSocketServer dss(kSocket_path);
  dss.RunServer();

  return 0;
}

void getInput(std::string filename, std::string Needfind){
  
}