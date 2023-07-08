#include "format.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace std::chrono;
using std::string;
using std::to_string;

#define CHRONO_DURACTION_CAST(type) \
  std::chrono::duration_cast<std::chrono::type>
// Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  std::chrono::seconds secs{seconds};
  std::chrono::hours HH = CHRONO_DURACTION_CAST(hours)(secs);
  secs -= CHRONO_DURACTION_CAST(seconds)(HH);
  std::chrono::minutes MM = CHRONO_DURACTION_CAST(minutes)(secs);
  secs -= CHRONO_DURACTION_CAST(seconds)(MM);
  std::stringstream ss{};
  ss << std::setw(2) << std::setfill('0') << HH.count() << std::setw(1) << ":"
     << std::setw(2) << std::setfill('0') << MM.count() << std::setw(1) << ":"
     << std::setw(2) << std::setfill('0') << secs.count();
  return ss.str()
}