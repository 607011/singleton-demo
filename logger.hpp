#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <mutex>

class Logger {
private:
  std::mutex mtx;
  std::ofstream f;

  Logger() = default;
  ~Logger() = default;
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

public:
  static Logger &instance() {
    static Logger logger;
    return logger;
  }

  static void init(const std::string &filename) {
    std::lock_guard<std::mutex> lock(instance().mtx);
    if (instance().f.is_open()) {
      throw std::runtime_error{ "Logger already initialized" };
    }
    instance().f.open(filename,
      std::ios::binary | std::ios::app);
  }

  void out(const std::string &msg) {
    std::lock_guard<std::mutex> lock(mtx);
    time_t t = time(nullptr);
    std::stringstream ss;
    ss << std::put_time(localtime(&t),
            "%Y-%m-%dT%H:%M:%S ")
      << msg << std::endl;
    f << ss.str();
    f.flush();
  }

  void flush() {
    std::lock_guard<std::mutex> lock(mtx);
    f.flush();
  }
};

#endif
