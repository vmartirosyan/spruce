#ifndef CREATOR_H
#define CREATOR_H

#include "BenchmarkTest.hpp"

enum Creator_Operations
{
	LargeFileCreation
};

class CreatorTest : public BenchmarkTest
{
public:
		
	CreatorTest(Mode m, int op, string a) : BenchmarkTest (m, op, a)
	{
		filesNumber = "1";
		fileSize = "150M";
	}
		
	~CreatorTest()
	{
	}
	
	Status Main();
private:
	string filesNumber;
	string fileSize;
	Status MultipleFilesCreationFunc();
	string CreateCommand();
};

#endif /* CREATOR_H */
