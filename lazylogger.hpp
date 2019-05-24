#ifndef __LAZYLOGGER_HPP__
#define __LAZYLOGGER_HPP__

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

class Logger {
private:
  ofstream f;
  static Logger *logger;

  Logger() { /* ... */ }

  ~Logger() {
    f.close();
  }

public:
  static Logger *instance() {
    static Cleaner cleaner;
    if (logger == nullptr) {
      logger = new Logger;
    }
    return logger;
  }

  static void init(const string &filename) {
    if (instance()->f.is_open()) {
      throw runtime_error{ "Logger already initialized" };
    }
    instance()->f.open(filename,
      ios::binary | ios::app);
  }

  Logger(const Logger &) = delete;
  Logger& operator=(const Logger &) = delete;

  void out(const string &msg) {
    if (!instance()->f.is_open()) {
      throw runtime_error{ "Logger not initialized" };
    }
    time_t t = time(nullptr);
    stringstream ss;
    ss << put_time(localtime(&t),
            "%Y-%m-%dT%H:%M:%S ")
      << msg << endl;
    f << ss.str();
  }

  void flush() {
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