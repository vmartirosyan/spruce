#include "test.hpp"
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

char * StatusMessages[] = {
	(char * )"Success",
	(char * )"Failed",
	(char * )"Unresolved",
	(char * )"Timeout",
	(char * )"Signaled",
	(char * )"Unknown"
	};
	
ProcessResult * Test::Execute()
{
	ProcessResult * p_res = Process::Execute();
	
	TestResult * t_res = new TestResult(*p_res, _operation, _args);
	
	delete p_res;
	
	return t_res;
}

TestResultCollection TestCollection::Run()
{
	TestResultCollection Results;
		
	for ( unsigned int index = 0 ; index < _tests.size(); ++index)
	{
		Results.AddResult( (TestResult *)_tests[index]->Execute() );
	}	
	
	return Results;
}

string TestResult::StatusToString()
{
	if ( _status >= Success && _status <= Unknown )
		return (string)StatusMessages[_status];
	else
		return (string)StatusMessages[Unknown];
}

string TestResult::ToXML()
{
	return StatusToString() + " : " + _output + " : " /* + _operation + " : " */ + _arguments;
}
