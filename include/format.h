#ifndef FORMAT_H
#define FORMAT_H

#include <string>

#define SECS_IN_MINUTE 60
#define SECS_IN_HOUR 3600

namespace Format {
std::string ElapsedTime(long times);  // TODO: See src/format.cpp
};                                    // namespace Format

#endif