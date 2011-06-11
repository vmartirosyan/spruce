#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>
#include <iostream>
#include <vector>
using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::pair;

typedef pair<int, string> Operation;

enum Mode
{
	Normal,
	FaultSimulation
};

enum Status
{
	Success,
	Fail,
	Unres,
	Timeout,
	Signaled,
	Unknown
};

#endif /* COMMON_HPP */
