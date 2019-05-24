#include "lazylogger.hpp"

#include <string>

int main(int argc, char *argv[]) {
  std::string msg = (argc > 1)
  ? argv[1]
  : "lorem ipsum ...";
  Logger::init("app.log");
  Logger *logger = Logger::instance();
  logger->out(msg);
  return 0;
}