#include "LandingGear.h"

namespace LG = LandingGear;

int main() {
  LG::LandingGear app = LG::LandingGear();
  const int port = 64432;

  app.get("/", [](LG::LGRequest& req, LG::LGResponse& res) {
    std::cout << "[/]: HIT" << std::endl;
    res.send("Eureka!");
  });

  app.get("/home/:epic", [](LG::LGRequest& req, LG::LGResponse& res) {
    std::cout << "[/home/:epic]: " << "param: " << req.params["epic"] << std::endl;
    res.send(req.params["epic"] + "!");
  });

  app.listen(port, []() {
    std::cout << "Server listening on port " << port << "!" << std::endl;
  });
}