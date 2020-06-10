#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "format.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::stoi;

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

// Done: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string key, value;
  float mem_total, mem_free, mem_util;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()){
    while (filestream>>key>>value){
      if (key=="MemTotal:"){
        mem_total = std::atol(value.c_str());
      }
      if (key=="MemFree:"){
        mem_free = std::atol(value.c_str());
      }
    mem_util = (mem_total - mem_free) / mem_total;
    }
    
  }
  return mem_util;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string uptime, downtime;
  long ut;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()){
    while (filestream>>uptime>>downtime){
      ut = std::atol(uptime.c_str()); 
    }
  }
  return ut;
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();}

// DONE: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  long jiffies, seconds;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      std::istream_iterator<string> begin(linestream), end;
      vector<string> values(begin, end);
      jiffies = std::atol(values[21].c_str());
      seconds = jiffies / sysconf(_SC_CLK_TCK);
      return seconds;
    }
  }
  return seconds;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> cpu_util = LinuxParser::CpuUtilization();
  return (
    std::atol(cpu_util[0].c_str()) +
    std::atol(cpu_util[1].c_str()) +
    std::atol(cpu_util[2].c_str()) +
    std::atol(cpu_util[3].c_str()) +
    std::atol(cpu_util[4].c_str()) +
    std::atol(cpu_util[5].c_str()) +
    std::atol(cpu_util[6].c_str()) +
    std::atol(cpu_util[7].c_str())
  );
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpu_util = LinuxParser::CpuUtilization();
  return std::atol(cpu_util[3].c_str());
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string cpu="cpu";
  string line, values;
  string user, nice, system, idle, iowait, irq, softirq, steal;
  vector<string> cpu_util{};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    if (line.compare(0, cpu.size(), cpu)){
      std::getline(filestream, line);
      std::istringstream linestream(line);
      std::istream_iterator<string> begin(linestream), end;
      vector<string> values(begin, end); 

      cpu_util.push_back(values[0]);
      cpu_util.push_back(values[1]);
      cpu_util.push_back(values[2]);
      cpu_util.push_back(values[3]);
      cpu_util.push_back(values[4]);
      cpu_util.push_back(values[5]);
      cpu_util.push_back(values[6]);
      cpu_util.push_back(values[7]);
      return cpu_util;
    }
  }
  return cpu_util;
}

/*
float LinuxParser::CpuUtilization(int pid) {
  string line;
  string value;
  float pcpu_util, seconds;
  int utime, stime, cutime, cstime, starttime, uptime, total_time;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()){
    getline(filestream, line);
    std::istringstream linestream(line);
    std::istream_iterator<string> begin(linestream), end;
    vector<string> values(begin, end);  

    utime = UpTime(pid);
    stime = std::atoi(values[14].c_str());
    cutime = std::atoi(values[15].c_str());
    cstime = std::atoi(values[16].c_str());
    starttime = std::atoi(values[21].c_str());
    uptime = UpTime(); 
    
    total_time = utime + stime + cutime + cstime;
    seconds = uptime - (starttime / sysconf(_SC_CLK_TCK));
    pcpu_util = 100.0 * ((total_time / sysconf(_SC_CLK_TCK)) / seconds);
  }
  return pcpu_util;
}
*/

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  int proc_total;
  string proc_name, proc_num;
  string nproc = "processes";
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      if (line.compare(0, nproc.size(), nproc)==0){
        std::istringstream linestream(line);
        linestream >> proc_name >> proc_num;
        proc_total = std::atoi(proc_num.c_str());
//        return proc_total;
      }
    }
  }
  return proc_total;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  int proc_running;
  string rproc_name, rproc_num;
  string nrproc = "procs_running";
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      if (line.compare(0, nrproc.size(), nrproc)==0){
        std::istringstream linestream(line);
        linestream >> rproc_name >> rproc_num;
        proc_running = std::atoi(rproc_num.c_str());
//        return proc_running;
      }
    }
  }
  return proc_running;
}

// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()){
      std::getline(filestream, line);
  }
  return line;
}


// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line;
  string key, value;
  int ram_mb;
  string ram_str = "VmSize:";
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key.compare(0, ram_str.size(), ram_str)==0){
        ram_mb = (std::atoi(value.c_str()) / 1000);
      }
    }
  }
  return to_string(ram_mb);
}


// DONE: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line;
  string key, value;
  string uid_str = "Uid:";
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      if(line.compare(0, uid_str.size(), uid_str)==0){
        std::istringstream linestream(line);
        linestream >> key >> value;
//        return value;
      }
    }
  }
  return value;
}


// DONE: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string value = "x:" + Uid(pid);
  string line, user;
  
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()){
    while (std::getline(filestream, line)) {
      if (line.find(value) != std::string::npos) {
        user = line.substr(0, line.find(":"));
        return user;
      }
    }
  }
  return user;
}


// DONE: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line;
  string value;
  long uptime;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      std::istream_iterator<string> begin(linestream), end;
      vector<string> values(begin, end);
      uptime = std::atol(values[21].c_str()) / sysconf(_SC_CLK_TCK);
      return uptime;
    }
  }
  return uptime;
}

