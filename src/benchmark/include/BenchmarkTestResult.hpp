#ifndef BENCHMARK_TEST_RESULT_HPP
#define BENCHMARK_TEST_RESULT_HPP

#include <stdio.h>
#include "test.hpp"

class BenchmarkTestResult : public TestResult
{
public:
	
	BenchmarkTestResult(ProcessResult pr, int op, string args, double time): TestResult(pr, op, args)
	{
		timestamp = time;
	}
	BenchmarkTestResult(Status s, string output, int op, string args, double time) : TestResult (s, output, op, args)
	{
		timestamp = time;
	}
	
	virtual ~BenchmarkTestResult()
	{
	}
	
	virtual string ToXML()
	{
		char strToTime[100];
		sprintf(strToTime, "%.2f", timestamp);
		
		return Operation::ToString(static_cast<Operations>(_operation)) + " : " + 
				StatusToString() + " : " + _output + " : " + _arguments + " : " + static_cast<string>(strToTime);
	}
protected:
	double timestamp;
};

#endif
