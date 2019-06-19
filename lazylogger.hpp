#ifndef __LAZYLOGGER_HPP__
#define __LAZYLOGGER_HPP__

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std; // bad practice in header files, Cevelop can refactor away from it

class Logger {
private:
  ofstream f;
  static Logger *logger;// unprotected mutable global state, shoult at least be std::atomic<Logger *>

  Logger() { /* ... */ } // unnecessary, better: Logger() = default;

  ~Logger() { // better: do not declare it.
    f.close(); // unnecessary, f will be closed automatically on destruction.
  }

public:
  static Logger *instance() {
    static Cleaner cleaner;
    if (logger == nullptr) { // potential data race, see POSA 2: double-checked locking
      logger = new Logger; // use atomic exchange
    }
    return logger;
  }

  static void init(const string &filename) {
    if (instance()->f.is_open()) { // data race potential
      throw runtime_error{ "Logger already initialized" };
    }
    instance()->f.open(filename,
      ios::binary | ios::app); // why binary for text output? but not really an issue
  }

  Logger(const Logger &) = delete;
  Logger& operator=(const Logger &) = delete; // simpler would be the Rule of DesDeMovA: just delete move assignment operator

  void out(const string &msg) {
    if (!instance()->f.is_open()) { // check is counterproductive for ostreams, does not allow to turn off ignore logging
      throw runtime_error{ "Logger not initialized" };
    }
    time_t t = time(nullptr);
    stringstream ss; // completely unneccesary, because fstream buffers
    ss << put_time(localtime(&t), // better use boost::date time formatting, that will be included in the std. not thread safe!!!
            "%Y-%m-%dT%H:%M:%S ")
      << msg << endl;
    f << ss.str(); // missing flush will produce incomplete logs
  }

  void flush() {
    f.flush();
  }

  class Cleaner {
  public:
    ~Cleaner() { // thus code might never run, e.g. when terminate() is called, even if called, the sequence is not actually determinable reasonably
      if (Logger::logger != nullptr) {
        delete Logger::logger; // why free it, when the program ends anyway? a flush in out might help solving the problem that this hack tries to solve.
        Logger::logger = nullptr;
      }
    }
  };
};

#endif // __LAZYLOGGER_HPP__
