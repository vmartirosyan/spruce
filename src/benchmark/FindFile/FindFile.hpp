#ifndef FIND_FILE_H
#define FIND_FILE_H

#include "BenchmarkTest.hpp"

enum FindFileOperations
{
	FindFile
};

class FindFileTest : public BenchmarkTest
{
public:
		
	FindFileTest(Mode m, int op, string a) : BenchmarkTest (m, op, a)
	{
		directoryName = "results";
		fileName = "0";
	}
	
	~FindFileTest()
	{
	}
	
	Status Main(vector<string>);
private:
	string fileName;
	string directoryName;
	Status FindFileCreationFunc();
	string CreateCommand();
};

#endif
