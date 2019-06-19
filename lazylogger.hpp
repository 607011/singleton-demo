#ifndef __LAZYLOGGER_HPP__
#define __LAZYLOGGER_HPP__

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

class Logger {
private:
  std::mutex mtx;
  std::ofstream f;
  static Logger *logger;

  Logger() = default;
  ~Logger() = default;

public:
  static Logger *instance() {
    static Cleaner cleaner;
    if (logger == nullptr) {
      logger = new Logger;
    }
    return logger;
  }

  static void init(const std::string &filename) {
    std::lock_guard<std::mutex> lock(instance()->mtx);
    if (instance()->f.is_open()) {
      throw std::runtime_error{ "Logger already initialized" };
    }
    instance()->f.open(filename,
      std::ios::binary | std::ios::app);
  }

  Logger(const Logger &) = delete;
  Logger& operator=(const Logger &) = delete;

  void out(const std::string &msg) {
    std::lock_guard<std::mutex> lock(mtx);
    if (!instance()->f.is_open()) {
      throw std::runtime_error{ "Logger not initialized" };
    }
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

  class Cleaner {
  public:
    ~Cleaner() {
      if (Logger::logger != nullptr) {
        delete Logger::logger;
        Logger::logger = nullptr;
      }
    }
  };
};

#endif // __LAZYLOGGER_HPP__