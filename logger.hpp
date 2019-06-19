#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <string>
#include <fstream>
#include <iostream> // not in general header, only defines cin and cout that are not used here
#include <sstream>
#include <iomanip>
#include <mutex>

using namespace std;

class Logger {
private:
  mutex mtx;
  ofstream f;
  Logger() = default;
  Logger(const Logger&) // Rule of DesDeMovA would need less code
    = delete;
  Logger& operator=(const Logger&)
    = delete;
  ~Logger() { // = default suffices, better not define at all.
    f.close();
  }

public:
  static Logger &instance() {
    static Logger logger;
    return logger;
  }

  static void init(const string &filename) { // can have data races, because not protected by a mutex
    if (instance().f.is_open()) {
      throw runtime_error{ "Logger already initialized" }; // why not allowing switching output?
    }
    instance().f.open(filename,
      ios::binary | ios::app); // can race, would require double-checked locking
  }

  void out(const string &msg) {
    unique_lock<mutex> lock(mtx); // lock_guard<mutex> would be sufficient
    time_t t = time(nullptr);
    stringstream ss; // not required, because fstream already buffers
    ss << put_time(localtime(&t),
            "%Y-%m-%dT%H:%M:%S ")
      << msg << endl;
    f << ss.str(); // flush missing will cause incomplete logs
  }

  void flush() {
    f.flush();// data race possible
  }
};

#endif
