/**
 * @file posixlib.h
 * @author Mason Marquez (theboys619@gmail.com)
 * @brief The LandingGear socket library for Linux/Posix systems. Implements the LGSocket classes using posix api/sys calls.
 * @version 0.1
 * @date 2022-11-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef POSIXLIB_H
#define POSIXLIB_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

namespace LandingGear {
  
  /**
   * Wrapper class for client sockets. Allows for easy and similar use of sockets between platforms.
  */
  class LGClientSocket {
    private:
    int socket;
    struct sockaddr_in sockAddr;

    public:

    LGClientSocket() {};
    LGClientSocket(int socket): socket(socket) {};

    bool isInit() {
      return socket > 0;
    }

    void setSockAddr(struct sockaddr_in s) {
      sockAddr = s;
    }

    int receive(char* recvbuf, size_t recvbuflen, int flags = 0) {
      return recv(socket, recvbuf, recvbuflen, flags);
    }

    int send(char* recvbuf, size_t recvbuflen, int flags = 0) {
      return ::send(socket, recvbuf, recvbuflen, flags);
    }

    /**
     * Closes the socket connection.
    */
    void close() {
      shutdown(socket, 2);
      ::close(socket);
    }
  };

  class LGServerSocket {
    private:
    int socket;
    struct sockaddr_in addr;

    public:
    int port = 8080;

    LGServerSocket() {};
    LGServerSocket(int port): port(port) {};

    /**
     * Accepts the next available connection.
     * 
     * @returns LGClientSocket - The client socket.
    */
    LGClientSocket accept() {
      struct sockaddr_in sockAddr;
      socklen_t size = sizeof(sockAddr);

      int clientSock = ::accept(socket, (struct sockaddr*)&sockAddr, &size);
      LGClientSocket csocket = LGClientSocket(clientSock);
      csocket.setSockAddr(sockAddr);

      if (clientSock < 0) {
        std::cerr << "Could not accept socket!" << std::endl;
        csocket.close();
        return LGClientSocket(-1);
      }

      return csocket;
    }

    /**
     * Close server socket connection.
    */
    void close() {
      ::close(socket);
    }

    /**
     * Start listening for new connections.
     * 
     * @returns 1 - Error, 0 - Success
    */
    int listen() {
      int results = 0;

      results = ::listen(socket, 5);
      if (results < 0) {
          std::cerr << "Failed to listen!" << std::endl;
          return 1;
      }

      return 0;
    }

    /**
     * Initializes the socket.
     * 
     * @returns 1 - Error, 0 - Success
    */
    int initSocket() {
      socket = -1;

      addr.sin_addr.s_addr = INADDR_ANY;
      addr.sin_port = htons(port);
      addr.sin_family = AF_INET;

      int results = 0;

      socket = ::socket(AF_INET, SOCK_STREAM, 0);

      if (socket < 0) {
        std::cerr << "Failed to initialize socket! Could not create socket!\n";
        return 1;
      }

      const int opt = 1;

      setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

      results = ::bind(socket, (struct sockaddr*)&addr, sizeof(addr));

      if (results < 0) {
        std::cerr << "Failed to initialize socket! Could not bind socket!\n";
        return 1;
      }

      return 0;
    }
  };

}; // namespace LandingGear


#endif