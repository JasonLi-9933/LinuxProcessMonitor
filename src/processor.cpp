#include "processor.h"
#include "linux_parser.h"
#include <unistd.h>
// Return the aggregate CPU utilization
float Processor::Utilization() { 
    long prev_idle_jiffies = LinuxParser::IdleJiffies();
    long prev_total_jiffies = LinuxParser::Jiffies();
    usleep(100000);
    long cur_idle_jiffies = LinuxParser::IdleJiffies();
    long cur_total_jiffies = LinuxParser::Jiffies();

    long total_jiffies_diff = cur_total_jiffies - prev_total_jiffies;
    long idle_jiffies_diff = cur_idle_jiffies - prev_idle_jiffies;
    return float(total_jiffies_diff - idle_jiffies_diff) / float(total_jiffies_diff);
}