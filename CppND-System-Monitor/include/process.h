#ifndef PROCESS_H
#define PROCESS_H
#include "unistd.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int);
  int Pid();                               // TODO: See src/process.cpp
  std::string User();                      // TODO: See src/process.cpp
  std::string Command();                   // TODO: See src/process.cpp
  float CpuUtilization();                  // TODO: See src/process.cpp
  std::string Ram();                       // TODO: See src/process.cpp
  long int UpTime();                       // TODO: See src/process.cpp
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp

  // TODO: Declare any necessary private members
 private:
  	int pid_ = 0;
    std::string user_ = "";
    std::string command_ = "";
    float cpuusage_ = 0;
};

#endif