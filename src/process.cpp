#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;


// DONE: Return this process's ID
int Process::Pid() { return pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() {
  string line;
  float pcpu_util, seconds;
  int utime, stime, cutime, cstime, starttime, uptime, total_time;
  std::ifstream filestream(LinuxParser::kProcDirectory + to_string(Process::Pid()) + LinuxParser::kStatFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){ 
      std::istringstream linestream(line);
      std::istream_iterator<string> begin(linestream), end;
      vector<string> values(begin, end);  

      utime = this->UpTime();
      stime = std::atoi(values[14].c_str());
      cutime = std::atoi(values[15].c_str());
      cstime = std::atoi(values[16].c_str());
      starttime = std::atoi(values[21].c_str());
      uptime = LinuxParser::UpTime(); 

      total_time = utime + stime + cutime + cstime;
      seconds = uptime - (starttime / sysconf(_SC_CLK_TCK));
      pcpu_util = 100.0 * ((total_time / sysconf(_SC_CLK_TCK)) / seconds);
      return pcpu_util;
    }
  }
  return pcpu_util;
}

// DONE: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Process::Pid()); }

// DONE: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Process::Pid()); }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Process::Pid()); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Process::Pid()); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { return this->cpu_util_<a.cpu_util_; }