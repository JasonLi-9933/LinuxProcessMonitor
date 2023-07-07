#include <string>

#include "format.h"

using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    long SS = seconds % SECS_IN_MINUTE;
    seconds -= SS;
    long MM = (seconds % SECS_IN_HOUR) / SECS_IN_MINUTE;
    long HH = seconds / SECS_IN_HOUR;
    return to_string(HH) + ":" + to_string(MM) + ":" + to_string(SS); 
}