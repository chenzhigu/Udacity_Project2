#include "processor.h"
#include "linux_parser.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
  long jiffies_active = LinuxParser::ActiveJiffies();
  long jiffies_total = LinuxParser::Jiffies();
  return 1.0 * jiffies_active / jiffies_total;
}