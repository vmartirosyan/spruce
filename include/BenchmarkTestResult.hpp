#ifndef BENCHMARK_TEST_RESULT_HPP
#define BENCHMARK_TEST_RESULT_HPP

#include "test_base.hpp"

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
		return _output + " : " + _arguments + " : " + " our time";
	}
protected:
	double timestamp;
};

#endif
