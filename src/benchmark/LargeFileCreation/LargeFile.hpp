#ifndef LARGE_FILE_CREATOR_H
#define LARGE_FILE_CREATOR_H

#include "BenchmarkTest.hpp"

enum LargeFileOperations
{
	LargeFileCreation
};

class LargeFileTest : public BenchmarkTest
{
public:
		
	LargeFileTest(Mode m, int op, string a) : BenchmarkTest (m, op, a)
	{
		filesNumber = "1";
		fileSize = "150M";
	}
		
	~LargeFileTest()
	{
	}
	
	Status Main();
private:
	string filesNumber;
	string fileSize;
	Status LargeFileCreationFunc();
	string CreateCommand();
};

#endif
