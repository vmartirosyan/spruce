#ifndef CREATOR_H
#define CREATOR_H

#include "BenchmarkTest.hpp"

enum CreatorOperations
{
	MultipleFilesCreation
};

class CreatorTest : public BenchmarkTest
{
public:
		
	CreatorTest(Mode m, int op, string a) : BenchmarkTest (m, op, a)
	{
		filesNumber = "10";
		fileSize = "1M";
	}
		
	~CreatorTest()
	{
	}
	
	Status Main(vector<string>);
private:
	string filesNumber;
	string fileSize;
	Status MultipleFilesCreationFunc();
	string CreateCommand();
};

#endif /* CREATOR_H */
