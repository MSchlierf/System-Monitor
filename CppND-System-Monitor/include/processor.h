#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <vector>
#include <string>

class Processor {
 public:
  	float Utilization();  // TODO: See src/processor.cpp

 private:
    float cpu_util;
  	float activeJiffies;
  	float totalJiffies;
};

#endif