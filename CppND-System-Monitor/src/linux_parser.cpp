#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::stoi;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line, key;
  float mem_total, mem_free, value;
  float mem_util = 0.0;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          mem_total = value; }
        else if (key == "MemFree:") {
          mem_free = value; }          
      }
   	}
  }
  mem_util = (mem_total - mem_free) / mem_total;
  return mem_util;
}    

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line;
  long uptime = 0;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return ActiveJiffies()+IdleJiffies();
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  vector<string> proc_stats = CpuUtilizationProcessData(pid);
  return (std::stol(proc_stats[13]) + std::stol(proc_stats[14]) + 
    std::stol(proc_stats[15]) + std::stol(proc_stats[16]));
}

//helper func to provide process related data
vector<string> LinuxParser::CpuUtilizationProcessData(int pid) {
	std::ifstream proc_stats(kProcDirectory + std::to_string(pid) + kStatFilename);
  	if (proc_stats) {
      string line, pidStat;
      getline(proc_stats, line);
      proc_stats.close();
      
      std::istringstream procStream(line);
      std::vector<string> pidStats;
      
      while(procStream >> pidStat) {
        pidStats.push_back(pidStat); 
      }
      return pidStats;
    }
  	return {};
}


// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  long activeJiffies = 0;
  vector<CPUStates> activeSts {kUser_,kNice_,kSystem_,kIRQ_,kSoftIRQ_,kSteal_};
  vector<string> util_data=CpuUtilization();
  for (auto state: activeSts){ activeJiffies+=stol(util_data[state]);}
  return activeJiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  long idleJiffies = 0;
  vector<CPUStates> idleSts {kIdle_,kIOwait_};
  vector<string> util_data=CpuUtilization();
  for (auto state: idleSts){ idleJiffies+=stol(util_data[state]);}
  return idleJiffies;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  std::string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream lineStream(line);
    std::vector<std::string> cpu_ut_info{std::istream_iterator<string>{lineStream},
                                         std::istream_iterator<string>{}};
    cpu_ut_info.erase(cpu_ut_info.begin());
    return cpu_ut_info;
  }
 return {};
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  string key, value;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
          return stoi(value);
      }
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  string key, value;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
          return stoi(value);  
      }     
    }
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {std::getline(stream, line); }
  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line, ram, key, value, ram_string;
  int ram_int;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.find("VmSize:") != string::npos) {
        std::istringstream linestream(line);
        linestream >> key >> value;
        ram = value;
      }
    }
  }
  ram_int = std::stoi(ram)/ 1024;
  ram_string = std::to_string(ram_int);
  return ram_string;
}
                  

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line, key, uid, value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          uid = value;
      	}
      }
    }
  }
  return uid;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uid = LinuxParser::Uid(pid);
  std::string username, key, value, line, x;
  std::ifstream stream(kPasswordPath);  
  if (stream.is_open()){
    while(std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lineStream(line);
      lineStream >> value >> x >> key; 
      if (key == uid) {
        username = value;
      }
    }
  }
  return username;
}


// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  std::string line;
  long uptime;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream lineStream(line);
    std::vector<string> data{std::istream_iterator<string>{lineStream}, std::istream_iterator<string>{}};
    uptime = LinuxParser::UpTime() - std::stol(data[21])/sysconf(_SC_CLK_TCK);
  }
  return uptime;
}