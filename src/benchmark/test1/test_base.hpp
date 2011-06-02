#ifndef TEST_BASE_H
#define TEST_BASE_H

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

enum Status
{
	Success,
	Fail,
	Unres,
	Timeout,
	Signalled,
	Unknown
};

class TestResult
{
public:
	TestResult():
		_status(Unknown),
		_error("Unknown result"),
		_operation("Empty operation"),
		_arguments("No arguments provided") {}
	TestResult(Status s, string err, string op, string args):
		_status(s),
		_error(err),
		_operation(op),
		_arguments(args) {}
	string ToXML();
private:
	Status _status;
	string _error;
	string _operation;
	string _arguments;
};

class TestResultCollection 
{
public:
	void AddResult(Status s, string err, string op, string args)
	{
		_results.push_back(TestResult(s,err,op,args));
	}
	void AddResult(const TestResult & result)
	{
		_results.push_back(result);
	}
	string ToXML()
	{
		string result = "";
		for ( vector<TestResult>::iterator i = _results.begin(); i != _results.end(); ++i )
			result += i->ToXML() + "\n";
			
		return result;
	}
private:
	vector<TestResult> _results;
};

class TestBase
{
public:
	virtual TestResultCollection Run();
	virtual ~TestBase() {};
protected:
	virtual Status RealRun(int operation, string args) = 0;
	vector<Operation> _operations;
};

#endif /* TEST_BASE_H */
