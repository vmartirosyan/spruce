#include "Creator.hpp"
#include <stdlib.h>
#include <sys/time.h>

TestResultCollection CreatorTest::Run()
{
	_operations.push_back(pair<int, string>(MultipleFilesCreation, "MultipleFilesCreation"));
	
	return TestBase::Run();
}

Status CreatorTest::RealRun(int operation, string args)
{
	switch (operation)
	{
		case MultipleFilesCreation:
			return MultipleFilesCreationFunc();
		default:
			cerr << "Unsupported operation.";	
			return Unres;
	}
	cerr << "Test was successful";	
	return Success;
}

Status CreatorTest::MultipleFilesCreationFunc()
{
	struct timeval start, end;
    double seconds, useconds;
    double mtime;
        
    string command = CreateCommand();
    gettimeofday(&start, NULL);
    
    system("pwd");
    system(command.c_str());   
    
    gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = seconds + useconds/1000000.0;
    
    timestamp = mtime;
    return Success;
}

string CreatorTest::CreateCommand()
{
	string command = (string)"./test1.sh" + (string)" " + filesNumber + (string)" " + fileSize;
	return command;
}
