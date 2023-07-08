#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::stoi;
using std::stol;
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
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string category;
  string MemTotal;
  string MemFree;
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream1(line);
    linestream1 >> category >> MemTotal;
    std::getline(stream, line);
    std::istringstream linestream2(line);
    linestream2 >> category >> MemFree;
    stream.close();
  }
  return 1 - stof(MemFree) / stof(MemTotal);
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  string up_time;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> up_time;
    stream.close();
    return stol(up_time);
  }
  return 0;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long jiffies{0};
  vector<string> utilization = LinuxParser::CpuUtilization();
  for (int i = kUser_; i <= kSteal_; i++) {
    jiffies += stol(utilization[i]);
  }
  return jiffies;
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  long jiffies{0};
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    string tick_value;
    int i = 0;
    while (i <= CSTIME) {
      linestream >> tick_value;
      if (i >= UTIME) {
        jiffies += stol(tick_value);
      }
      i++;
    }
  }
  return jiffies;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies();
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  long jiffies{0};
  vector<string> utilization = LinuxParser::CpuUtilization();
  for (int i = kIdle_; i <= kIOwait_; i++) {
    jiffies += stol(utilization[i]);
  }
  return jiffies;
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  vector<string> utilizations;
  if (stream.is_open()) {
    string value;
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;  // exclude the word 'cpu'
    for (int i = kUser_; i <= kSteal_; i++) {
      linestream >> value;
      utilizations.emplace_back(value);
    }
    stream.close();
  }
  return utilizations;
}

string LinuxParser::GetProcStatValue(string target) {
  string entry;
  string value;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> entry >> value;
      if (entry == target) {
        stream.close();
        return value;
      }
    }
    stream.close();
  }
  return string();
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  return stoi(LinuxParser::GetProcStatValue("processes"));
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  return stoi(LinuxParser::GetProcStatValue("procs_running"));
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string cmd;
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cmd;
    stream.close();
  }
  return cmd;
}

string LinuxParser::GetProcStatusValue(int pid, string target) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  string line;
  string entry;
  string value;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream streamline(line);
      streamline >> entry >> value;
      if (entry == target) {
        stream.close();
        return value;
      }
    }
    stream.close();
  }
  return string();
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  // I choose to use VmRSS instead of VmSize to assuming user don't understand
  // what is vitual memory
  string mem_in_kb = GetProcStatusValue(pid, "VmRSS");
  long mem_in_mb = stol(mem_in_kb) / 1000;
  return to_string(mem_in_mb);
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  return LinuxParser::GetProcStatusValue(pid, "Uid");
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  std::ifstream stream(kPasswordPath);
  string user;
  string pwd;
  string uid;
  string line;
  string targetUid = LinuxParser::Uid(pid);
  if (stream.is_open() && !targetUid.empty()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream streamline(line);
      streamline >> user >> pwd >> uid;
      if (uid == targetUid) {
        stream.close();
        return user;
      }
    }
    stream.close();
  }
  return string();
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  long sys_uptime = LinuxParser::UpTime();
  string starttime;
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream streamline(line);
    int i = 0;
    while (i < STARTTIME) {
      streamline >> starttime;
      i++;
    }
    streamline >> starttime;
    stream.close();
  }
  return sys_uptime - stol(starttime) / sysconf(_SC_CLK_TCK);
}
