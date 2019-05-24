#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <mutex>

using namespace std;

class Logger {
private:
  mutex mtx;
  ofstream f;
  Logger() = default;
  Logger(const Logger&)
    = delete;
  Logger& operator=(const Logger&)
    = delete;
  ~Logger() {
    f.close();
  }

public:
  static Logger &instance() {
    static Logger logger;
    return logger;
  }

  static void init(const string &filename) {
    if (instance().f.is_open()) {
      throw runtime_error{ "Logger already initialized" };
    }
    instance().f.open(filename,
      ios::binary | ios::app);
  }

  void out(const string &msg) {
    unique_lock<mutex> lock(mtx);
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
};

#endif
