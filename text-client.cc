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

// Domain Socket Client C++ Interface Class
//
class DomainSocketClient : public UnixDomainSocket {
 public:
  using UnixDomainSocket::UnixDomainSocket;

  void RunClient(std::string filePath, int filePathSize, std::string keyword, int keywordSize) {
    // (1) open nameless Unix socket
    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd < 0) {
      std::cerr << strerror(errno) << std::endl;
      exit(-1);
    }

    // (2) connect to an existing socket
    int success = connect(socket_fd,
                          // sockaddr_un is a Unix sockaddr
                          reinterpret_cast<const sockaddr*>(&sock_addr_),
                          sizeof(sock_addr_));
    if (success < 0) {
      std::cerr << strerror(errno) << std::endl;
      exit(-1);
    }

    // (3) write to socket
    const ssize_t kWrite_buffer_size = 64;
    char write_buffer[kWrite_buffer_size];
    while (true) {
      std::cin.getline(write_buffer, kWrite_buffer_size);
      while (std::cin.gcount() > 0) {
        if (std::cin.gcount() == kWrite_buffer_size - 1 && std::cin.fail())
          std::cin.clear();
        // write() is equivalent to send() with no flags in send's 3rd param
        ssize_t bytes_wrote = write(socket_fd, write_buffer, std::cin.gcount());
        std::cout << "sent " << std::cin.gcount() << " bytes" << std::endl;
        if (bytes_wrote < 0) {
          std::cerr << strerror(errno) << std::endl;

          exit(-1);
        }

        if (bytes_wrote == 0) {
          std::clog << "Server dropped connection!" << std::endl;
          exit(-2);
        }

        std::cin.getline(write_buffer, kWrite_buffer_size);
      }
    }
  }
};

const char kSocket_path[] = "";
int main(int argc, char *argv[]) {
  if (argc != 4)
    return 1;
  int filePathSize = std::string(argv[2]).size();
  int keywordSize = std::string(argv[3]).size();
  DomainSocketClient dsc(kSocket_path);
  dsc.RunClient(argv[2], filePathSize, argv[3], keywordSize);

  return 0;
}