/**
 * @file LandingGear.h
 * @author Mason Marquez (theboys619@gmail.com)
 * @brief The main header file for the LandingGear library. Contains the classes and method definitions for the library.
 * @version 0.1
 * @date 2022-11-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef LANDINGGEAR_H
#define LANDINGGEAR_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
#include "winlib.h"
#else
#include "posixlib.h"
#endif

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cctype>
#include <thread>
#include <fstream>

#include "EventListener.h"

namespace LandingGear {

  class LandingGear;

  /**
   * @brief Has easy implementation and use for accessing and setting HTTP headers.
   * 
   */
  class LGHeaders {
    private:
    std::string dataStr;

    public:
    std::unordered_map<std::string, std::string> headers;
    std::string method;
    std::string path;
    std::string protocol;

    LGHeaders();
    LGHeaders(std::string dataStr);
    LGHeaders(std::unordered_map<std::string, std::string> headers);
    LGHeaders(const LGHeaders& oHeaders); // copy constructor

    std::string getDataStr() const;

    bool hasHeader(std::string key);

    std::string getHeader(std::string key);
    std::string setHeader(std::string key, std::string value);

    std::string operator[] (std::string key);

    static LGHeaders constructHeaders(std::string unformatted);
    static LGHeaders constructHeader(std::string unformatted);
    static LGHeaders constructRequestInfo(std::string unformatted);
  };

  /**
   * @brief Includes headers, app, and many properties of the current processed request.
   * 
   */
  class LGRequest : public EventListener {
    private:
    LGClientSocket socket;

    public:
    std::string url;
    std::string path;
    std::string method;
    std::string protocol;

    LGHeaders headers;
    std::unordered_map<std::string, std::string> params;

    LandingGear* app;

    LGRequest();
    LGRequest(LGClientSocket socket);

    std::string getRequest();
  };

  /**
   * @brief Allows to send data from the client.
   * 
   */
  class LGResponse : public EventListener {
    private:
    LGClientSocket socket;

    public:
    int statusCode;
    bool headersSent;

    LGHeaders headers;
    LandingGear* app;

    LGResponse();
    LGResponse(LGClientSocket socket);

    void header(std::string header, std::string value);
    void header(std::string header, int value);
    void set(std::unordered_map<std::string, std::string> headers);

    LGResponse& status(int code);
    LGResponse& send(int code, std::string data);
    LGResponse& send(std::string data);
    LGResponse& end(std::string data);

    int sendString(std::string data);
  };

  // Middleware Function Types
  typedef std::function<void(void)> NextFunction;
  typedef std::function<void(LGRequest&, LGResponse&, NextFunction)> LGMiddlewareCB; // eg: (req, res, next) -> {}
  typedef std::function<void(LGRequest&, LGResponse&)> ReqCallback; // eg: (req, res) -> {}

  /**
   * @brief Main middleware classes.
   * contains objects and information about the middleware.
   */
  class LGMiddleware {
    public:
    LGMiddlewareCB cb;
    std::string method; // if eg: app.get("/home", middlewarefunc) -> "GET"
    bool isWildcard;

    std::string path;

    LGMiddleware();
    LGMiddleware(std::string path);
    LGMiddleware(std::string path, const char* method);
    LGMiddleware(std::string path, const char* method, LGMiddlewareCB cb);
    LGMiddleware(std::string path, const char* method, ReqCallback cb);
    LGMiddleware(std::string path, bool isWildcard);
    LGMiddleware(LGMiddlewareCB cb);
    LGMiddleware(LGMiddlewareCB cb, std::string method);

    // Calls the callback (cb)
    void call(LGRequest& req, LGResponse& res, NextFunction next);
  };

  typedef void(*ListenCB)(void);

  /**
   * @brief To initialize the library and create a ServerSocket.
   * Allows for setting up endpoints and request paths.
   */
  class LandingGear {
    private:
    LGServerSocket socket;
    std::thread mainThread;

    public:
    std::vector<LGMiddleware> middleware; // middleware stack

    LandingGear();

    void get(std::string path, ReqCallback cb);
    void get(std::string path, LGMiddlewareCB cb);
    void get(std::string path, LGMiddlewareCB middle, LGMiddlewareCB cb);
    void get(std::string path, LGMiddlewareCB middle, ReqCallback cb);

    void use(std::string path, LGMiddlewareCB cb);

    int listen(int port);
    int listen(int port, ListenCB cb);
  };

  LGMiddlewareCB getStatic(std::string folderpath);
}; // namespace LandingGear

#endif