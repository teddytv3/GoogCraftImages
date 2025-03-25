#pragma once
#include <string>
#include <chrono>
#include <fstream>

namespace Shared {
	extern void log(std::string filename, int8_t successFlag, std::string msg);
}