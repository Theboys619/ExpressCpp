/**
 * @file winlib.h
 * @author Mason Marquez (theboys619@gmail.com)
 * @brief The LandingGear socket library for Windows. Implements the LGSocket classes using the WinAPI and Winsock2.
 * @version 0.1
 * @date 2022-11-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef WINLIB_H
#define WINLIB_H

// WINDOWS SOCKET IMPLEMENTATION //

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

namespace LandingGear {

  /**
   * Wrapper class for client sockets. Allows for easy and similar use of sockets between platforms.
  */
  class LGClientSocket {
    private:
    SOCKET socket;

    public:

    LGClientSocket() {};
    LGClientSocket(SOCKET socket): socket(socket) {};

    bool isInit() {
      return socket != INVALID_SOCKET;
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
      shutdown(socket, SD_SEND);
      closesocket(socket);
    }
  };

  class LGServerSocket {
    private:
    SOCKET socket;
    struct addrinfo* addr = NULL; // Address information of where to listen
    struct addrinfo hints;

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
      SOCKET clientSock = ::accept(socket, NULL, NULL);
      LGClientSocket csocket = LGClientSocket(clientSock);

      if (clientSock == INVALID_SOCKET) {
        std::cerr << "Could not accept socket!" << std::endl;
        close();
        return LGClientSocket(INVALID_SOCKET);
      }

      return csocket;
    }

    /**
     * Close server socket connection.
    */
    void close() {
      closesocket(socket);
    }

    /**
     * Start listening for new connections.
     * 
     * @returns 1 - Error, 0 - Success
    */
    int listen() {
      int results = 0;

      results = ::listen(socket, SOMAXCONN);
      if (results == SOCKET_ERROR) {
          std::cerr << "Failed to listen! Error: " << WSAGetLastError() << std::endl;
          closesocket(socket);
          WSACleanup();
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
      WSADATA wsadata;
      int results = 0;

      results = WSAStartup(MAKEWORD(2,2), &wsadata);
      if (results != 0) {
        std::cerr << "WSAStartup failed!\n";
        return 1;
      }

      socket = INVALID_SOCKET;

      ZeroMemory(&hints, sizeof(hints));

      hints.ai_family = AF_INET;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_protocol = IPPROTO_TCP;
      hints.ai_flags = AI_PASSIVE;


      std::ostringstream portStr;
      portStr << port;

      std::string portInfo = portStr.str();

      results = getaddrinfo(NULL, (PCSTR)portInfo.c_str(), &hints, &addr);

      if (results != 0) {
        std::cerr << "Failed to initialize socket! Could not get address information! Error: \n" << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
      }

      socket = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

      if (socket == INVALID_SOCKET) {
        std::cerr << "Failed to initialize socket! Could not create socket!\n";
        freeaddrinfo(addr);
        WSACleanup();
        return 1;
      }

      results = ::bind(socket, addr->ai_addr, (int)addr->ai_addrlen);

      if (results == SOCKET_ERROR) {
        std::cerr << "Failed to initialize socket! Could not bind socket!\n";
        freeaddrinfo(addr);
        closesocket(socket);
        WSACleanup();
        return 1;
      }

      freeaddrinfo(addr);

      return 0;
    }
  };

}; // namespace LandingGear

#endif