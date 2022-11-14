#include "LandingGear.h"

namespace LandingGear {

  // A list of all available HTTP status codes.
  static std::unordered_map<int, std::string> statusCodes = {
    // 200 Codes
    {200, "OK"},
    {201, "Created"},
    {202, "Accepted"},
    {203, "Non-Authoritative Information"},
    {204, "No Content"},
    {205, "Reset Content"},
    {206, "Partial Content"},
    {207, "Multi-Status"},
    {208, "Already Reported"},
    {226, "IM Used"},

    // 300 Codes
    {300, "Multiple Choices"},
    {301, "Moved Permanently"},
    {302, "Found"},
    {303, "See Other"},
    {304, "Not Modified"},
    {305, "Use Proxy"},
    {306, "unused"},
    {307, "Temporary Redirect"},
    {308, "Permanent Redirect"},

    // 400 Codes,
    {400, "Bad Request"},
    {401, "Unauthorized"},
    {402, "Payment Required"},
    {403, "Forbidden"},
    {404, "Not Found"},
    {405, "Method Not Allowed"},
    {406, "Not Acceptable"},
    {407, "Proxy Authentication Required"},
    {408, "Request Timeout"},
    {409, "Conflict"},
    {410, "Gone"},
    {411, "Length Required"},
    {412, "Precondition Failed"},
    {413, "Payload Too Large"},
    {414, "URI Too Long"},
    {415, "Unsupported Media Type"},
    {416, "Range Not Satisfiable"},
    {417, "Expectation Failed"},
    {418, "I'm a teapot"},
    {421, "Misdirected Request"},
    {422, "Unprocessable Entity"},
    {423, "Locked"},
    {424, "Failed Dependency"},
    {425, "Too Early"},
    {426, "Upgrade Required"},
    {428, "Precondition Required"},
    {429, "Too Many Requests"},
    {431, "Request Header Fields Too Large"},
    {451, "Unavailable For Legal Reasons"},

    // 500 Codes,
    {500, "Internal Server Error"},
    {501, "Not Implemented"},
    {502, "Bad Gateway"},
    {503, "Service Unavailable"},
    {504, "Gateway Timeout"},
    {505, "HTTP Version Not Supported"},
    {506, "Variant Also Negotiates"},
    {507, "Insufficient Storage"},
    {508, "Loop Detected"},
    {510, "Not Extended"},
    {511, "Network Authentication Required"},
  };

  // trim from start (in place)
  static inline void ltrim(std::string &s) {
      s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
          return !std::isspace(ch);
      }));
  }

  // trim from end (in place)
  static inline void rtrim(std::string &s) {
      s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
          return !std::isspace(ch);
      }).base(), s.end());
  }

  // trim from both ends (in place)
  static inline void trim(std::string &s) {
      ltrim(s);
      rtrim(s);
  }

  std::string get(std::string thisstr, int start, int amount) {
    std::string str = "";

    for (int i = start; i < start+amount; i++) {
      if (i >= thisstr.size()) break;
      str += thisstr[i];
    }

    return str;
  }

  void toLowerCase(std::string& data) {
    std::transform(data.begin(), data.end(), data.begin(),
    [](unsigned char c){ return std::tolower(c); });
  }

  /**
   * @brief Splits a string by a specified separator
   * 
   * @param thisstr The string to be splitted
   * @param sep The separator
   * @return std::vector<std::string> - Each index is a split point. 
   */
  std::vector<std::string> split(std::string thisstr, std::string sep) {
    std::vector<std::string> splitted = {};
    std::string current = "";

    for (int i = 0; i < thisstr.size(); i++) {
      std::string peeked = get(thisstr, i, sep.size());

      if (peeked == sep) {
        splitted.push_back(current);
        current = "";
        i += peeked.size() - 1;
      } else {
        current += thisstr[i];
      }
      
    }

    if (current.size() > 0) splitted.push_back(current);

    return splitted;
  }

  bool endsWith(const std::string &mainStr, const std::string &toMatch)
  {
    if (mainStr.size() >= toMatch.size() &&
      mainStr.compare(mainStr.size() - toMatch.size(), toMatch.size(), toMatch) == 0)
      return true;
    else
      return false;
  }

  bool startsWith(const std::string &mainStr, const std::string &toMatch)
  {
    return mainStr.rfind(toMatch, 0) == 0;
  }

  bool includes(const std::string &str, const std::string &toMatch) {
    return str.find(toMatch) != std::string::npos;
  }

  LGHeaders::LGHeaders() {};
  LGHeaders::LGHeaders(std::string dataStr): dataStr(dataStr) {};
  LGHeaders::LGHeaders(std::unordered_map<std::string, std::string> headers): headers(headers) {};
  LGHeaders::LGHeaders(const LGHeaders& oHeaders) {
    headers = oHeaders.headers;
    dataStr = oHeaders.getDataStr();
  }

  std::string LGHeaders::getDataStr() const {
    return dataStr;
  }

  /**
   * @brief Checks to see if header exists in the list of known headers.
   * 
   * @param key The header to find
   * @return true - The header exists
   * @return false - The header does not exist
   */
  bool LGHeaders::hasHeader(std::string key) {
    return headers.find(key) != headers.end();
  }

  /**
   * @brief Gets a header from a string.
   * 
   * @param key The header name
   * @return std::string The header data
   */
  std::string LGHeaders::getHeader(std::string key) {
    return headers[key];
  }

  /**
   * @brief Sets a header from a key, value pair.
   * 
   * @param key The header name
   * @param value The value to set the header with
   * @return std::string 
   */
  std::string LGHeaders::setHeader(std::string key, std::string value) {
    return (headers[key] = value);
  }

  /**
   * @brief Gets a header from a string.
   * 
   * @param key The header name
   * @return std::string The header data
   */
  std::string LGHeaders::operator[](std::string key) {
    return headers[key];
  }

  /**
   * @brief Constructs headers from an unformatted string received by a client socket.
   * 
   * @param unformatted The unformatted string with header information.
   * @return LGHeaders - The newly formatted headers.
   */
  LGHeaders LGHeaders::constructHeaders(std::string unformatted) {
    std::vector<std::string> lines = split(unformatted, "\n");

    LGHeaders headers = LGHeaders(unformatted);

    for (int i = 0; i < lines.size(); i++) {
      std::vector<std::string> headerKV = split(lines[i], ":");
      std::string key = headerKV[0];
      trim(key);

      std::string value = "";

      for (int j = 1; j < headerKV.size(); j++) {
        std::string newValue = headerKV[j];
        trim(newValue);

        value += (j > 1 ? ":" : "") + newValue;
      }

      toLowerCase(key);

      headers.setHeader(key, value);
    }

    return headers;
  }

  /**
   * @brief Constructs a single header from an unformatted string received by a client socket.
   * 
   * @param unformatted The unformatted string with header information.
   * @return LGHeaders - The newly formatted header. Only contains 1 header
   */
  LGHeaders LGHeaders::constructHeader(std::string unformatted) {
    if (endsWith(unformatted, "\n")) {
      unformatted = split(unformatted, "\n")[0];
    }
    LGHeaders headers = LGHeaders(unformatted);

    std::vector<std::string> headerKV = split(unformatted, ":");
    std::string key = headerKV[0];
    trim(key);

    std::string value = "";

    for (int j = 1; j < headerKV.size(); j++) {
      std::string newValue = headerKV[j];
      trim(newValue);

      value += (j > 1 ? ":" : "") + newValue;
    }

    toLowerCase(key);

    headers.setHeader(key, value);

    return headers;
  }

  /**
   * @brief Constructs request information from an unformatted string.
   * (eg. "GET /path HTTP/1.1\\r\\n")
   * 
   * @param unformatted The unformatted string with the request
   * @return LGHeaders A header object that only contains request information. (eg. `headers.method`)
   */
  LGHeaders LGHeaders::constructRequestInfo(std::string unformatted) {
    if (endsWith(unformatted, "\n")) {
      unformatted = split(unformatted, "\n")[0];
    }
    std::vector<std::string> reqInfo = split(unformatted, " "); // REQUEST: METHOD, PATH, PROTOCOL
    trim(reqInfo[0]);

    std::string method = reqInfo[0];

    trim(reqInfo[1]);
    std::string path = reqInfo[1];

    trim(reqInfo[2]);
    std::string protocol = reqInfo[2];

    LGHeaders headers = LGHeaders(unformatted);
    headers.method = method;
    headers.path = path;
    headers.protocol = protocol;

    return headers;
  }

  LGResponse::LGResponse() {
    statusCode = 404;
    headers = LGHeaders();
  };
  LGResponse::LGResponse(LGClientSocket socket): socket(socket) {
    statusCode = 404;
    headers = LGHeaders();
  };

  /**
   * @brief Set a header from a string
   * 
   * @param header The header to set
   * @param value The value or data of the header
   */
  void LGResponse::header(std::string header, std::string value) {
    toLowerCase(header);
    headers.setHeader(header, value);
  };

  /**
   * @brief Set a header from an integer
   * 
   * @param header The header to set
   * @param value The value of the header
   */
  void LGResponse::header(std::string header, int value) {
    std::ostringstream o;
    o << value;
    this->header(header, o.str());
  };

  /**
   * @brief Set headers from a Map. Does not replace current headers only adds onto it. Will override existing header if one should exist.
   * 
   * @param headers The map that contains headers to be added.
   */
  void LGResponse::set(std::unordered_map<std::string, std::string> headers) {
    for (auto data : headers) {
      std::string key = data.first;
      std::string value = data.second;

      header(key,  value);
    }
  };

  /**
   * @brief Sets the status code for the response.
   * 
   * @param code The status code
   */
  LGResponse& LGResponse::status(int code) {
    statusCode = code;

    return *this;
  };

  /**
   * @brief Sends data with a specified status code.
   * 
   * @param code The status code to be sent (eg. 200)
   * @param data The data to be sent
   */
  LGResponse& LGResponse::send(int code, std::string data) {
    status(code);

    if (!headers.hasHeader("content-type")) {
      header("Content-Type", "text/plain");
    }

    if (!headers.hasHeader("content-length")) {
      header("Content-Length", data.size());
    }

    std::ostringstream resString;
    resString << "HTTP/1.1 " << statusCode << " " << statusCodes[statusCode] << "\r\n"
      << "Connection: keep-alive\r\n"
      << "Content-Type: " << headers["content-type"] << "\r\n"
      << "Content-Length: " << headers["content-length"] << "\r\n"
      << "\r\n";

    sendString(resString.str());
    int bytes = sendString(data);

    if (bytes >= std::stoi(headers["content-length"])) {
      headersSent = true;
    }

    return *this;
  };

  /**
   * @brief Send a string to the client. Abstracts away socket functions.
   * Automatically sends status code 200.
   * 
   * @param data The data to be sent.
   */
  LGResponse& LGResponse::send(std::string data) {
    send(200, data);

    return *this;
  };

  /**
   * @brief Send a string to the client. Abstracts away socket functions.
   * Sends the set status code.
   * 
   * @param data The data to be sent.
   */
  LGResponse& LGResponse::end(std::string data) {
    send(statusCode, data);

    return *this;
  };

  /**
   * @brief Sends a string to the client. A wrapper for 'send' syscall/function call.
   * 
   * @param data The data to be sent
   * @return int The amount of bytes sent
   */
  int LGResponse::sendString(std::string data) {
    return socket.send((char*)data.c_str(), data.size());
  };

  LGRequest::LGRequest() {};
  LGRequest::LGRequest(LGClientSocket socket): socket(socket) {};

  /**
   * @brief Process current request. Constructs response object and fills information of the request into the Request object.
   * 
   * @return std::string The full body of the request
   */
  std::string LGRequest::getRequest() {    
    int bytes = 0;

    std::string fullData = "";

    bool gotRequest = false;
    bool recievedHeaders = false;

    std::string headersString = "";
    int headerStartIndex = 0;

    bool nextCalled = true;
    int index = 0;

    LGResponse res = LGResponse(socket);
    res.app = app;
    std::vector<LGMiddleware> middleware = app->middleware;

    NextFunction next = [&]() {
      nextCalled = true;
      index++;
    };

    std::function<void(LGMiddleware, LGRequest*)> checkMiddle = [&res, &next](LGMiddleware middle, LGRequest* req) {
      if (middle.method == "USE") {
        if (middle.path.size() > 0) {
          if (startsWith(req->path, middle.path)) {
            middle.call(*req, res, next);
          }
        } else {
          middle.call(*req, res, next);
        }
      } else {
        std::string path = req->path;
        std::string mpath = middle.path;
        trim(path);
        trim(mpath);

        if (startsWith(path, "/")) {
          path = path.substr(1); // /home/nice
        }
        if (startsWith(mpath, "/")) {
          mpath = mpath.substr(1); // /home/:monkey
        }

        std::vector<std::string> reqPaths = split(path, "/"); // [home, nice]
        std::vector<std::string> methodPaths = split(mpath, "/"); // [home, :monkey]

        bool isCorrectPath = path == middle.path || (reqPaths.size() == 0 && methodPaths.size() == 0);

        if (middle.method == req->method) {
          if (!isCorrectPath && reqPaths.size() == methodPaths.size()) {
            for (int i = 0; i < reqPaths.size(); i++) {
              std::string currentPath = reqPaths[i];
              std::string currentMPath = methodPaths[i];

              if (currentPath == currentMPath) {
                isCorrectPath = true;
                continue;
              } else if (!startsWith(currentMPath, ":") && !startsWith(currentMPath, "*")) {
                isCorrectPath = false;
                break;
              }

              if (startsWith(currentMPath, ":")) {
                req->params[currentMPath.substr(1)] = currentPath;
              }
            }
          }

          if (isCorrectPath) {
            middle.call(*req, res, next);
          } else {
            next();
          }
        }
      }
    };

    do {
      char buffer[512] = {'\0'};

      std::string currentData = "";
      bytes = socket.receive(buffer, 511);

      if (bytes <= 0) {
        break;
      }

      buffer[bytes] = '\0';
      currentData = buffer;
      ltrim(currentData);

      if (!gotRequest && (includes(currentData, "\r\n") || includes(currentData, "\n"))) {
        gotRequest = true;

        headerStartIndex = currentData.find("\r\n");
        if (headerStartIndex == std::string::npos) {
          headerStartIndex = currentData.find("\n");
        }

        std::string reqInfoStr = currentData.substr(0, headerStartIndex);

        LGHeaders reqInfo = LGHeaders::constructRequestInfo(reqInfoStr);
        method = reqInfo.method;
        path = reqInfo.path;
        protocol = reqInfo.protocol;
      }

      if (!recievedHeaders && (includes(currentData, "\r\n\r\n") || includes(currentData, "\n\n"))) {
        recievedHeaders = true;

        int endingIndex = currentData.find("\r\n\r\n");
        if (endingIndex == std::string::npos) {
          endingIndex = currentData.find("\n\n");
        }

        headersString = fullData.substr(headerStartIndex, endingIndex - headerStartIndex);
        trim(headersString);
        headers = LGHeaders::constructHeaders(headersString);
        headers.method = method;
        headers.path = path;
        headers.protocol = protocol;

        url = startsWith(protocol, "HTTPS") ? "https" : "http://";
        url += headers["host"];
        url += path;
      }

      if (recievedHeaders && nextCalled) {
        nextCalled = false;

        LGMiddleware middle = middleware[index];
        checkMiddle(middle, this);
      }

      fullData += currentData;

      this->emit("data", EventData(EventType::CHUNK, currentData));

      if (bytes != 511 || res.headersSent) {
        break;
      }
    } while (bytes > 0);

    while (index < middleware.size() && !res.headersSent) {
      if (!nextCalled) {
        break;
      }

      nextCalled = false;
      
      LGMiddleware middle = middleware[index];
      checkMiddle(middle, this);
    }

    if (!res.headersSent) {
      res.status(404).end("Page Not Found!");
    }

    this->emit("end", EventData(EventType::CHUNK, fullData));
    socket.close();

    return fullData;
  }

  LGMiddleware::LGMiddleware() {};
  LGMiddleware::LGMiddleware(std::string path): path(path) {};
  LGMiddleware::LGMiddleware(std::string path, const char* method): path(path), method(method) {};
  LGMiddleware::LGMiddleware(std::string path, bool isWildcard): path(path), isWildcard(isWildcard) {};
  LGMiddleware::LGMiddleware(LGMiddlewareCB cb): cb(cb) {};
  LGMiddleware::LGMiddleware(LGMiddlewareCB cb, std::string method): cb(cb), method(method) {};

  // Calls the callback (cb)
  void LGMiddleware::call(LGRequest& req, LGResponse& res, std::function<void(void)> next) {
    cb(req, res, next);
  }

  LandingGear::LandingGear() {
    socket = LGServerSocket();
  }

  void LandingGear::get(std::string path, ReqCallback cb) {
    LGMiddleware middlew = LGMiddleware(path, "GET");
    middlew.cb = [&](LGRequest& req, LGResponse& res, NextFunction next) {
      cb(req, res);
      if (!res.headersSent) {
        next();
      }
    };

    middleware.push_back(middlew);
  }
  void LandingGear::get(std::string path, LGMiddlewareCB cb) {
    LGMiddleware middlew = LGMiddleware(path, "GET");
    middlew.cb = cb;

    middleware.push_back(middlew);
  }
  void LandingGear::get(std::string path, LGMiddlewareCB middle, LGMiddlewareCB cb) {
    LGMiddleware middlew = LGMiddleware(path, "GET");
    middlew.cb = [&](LGRequest& req, LGResponse& res, NextFunction next) {
      middle(req, res, [&]() {
        cb(req, res, next);
      });
    };

    middleware.push_back(middlew);
  }
  void LandingGear::get(std::string path, LGMiddlewareCB middle, ReqCallback cb) {
    LGMiddleware middlew = LGMiddleware(path, "GET");
    middlew.cb = [&](LGRequest& req, LGResponse& res, NextFunction next) {
      middle(req, res, [&]() {
        cb(req, res);

        if (!res.headersSent) {
          next();
        }
      });
    };

    middleware.push_back(middlew);
  }

  int LandingGear::listen(int port, ListenCB cb) {
    cb();

    return listen(port);
  }

  /**
   * @brief Starts the webserver on the specified port.
   * 
   * @param port The port to bind and listen on
   * @return int Status code: 1 - Failure, 0 - Success
   */
  int LandingGear::listen(int port) {
    socket.port = port;
    socket.initSocket();

    socket.listen();

    bool serverRunning = false;

    mainThread = std::thread([&](){
      serverRunning = true;
      while (true) {
        LGClientSocket clientSocket = socket.accept();

        if (!clientSocket.isInit()) {
          socket.close();
          return 1;
        }

        LGRequest req = LGRequest(clientSocket);
        req.app = this;
        
        req.getRequest();
      }

      socket.close();
      serverRunning = false;
    });

    mainThread.join();

    return 0;
  }
};