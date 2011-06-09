#ifndef DELETER_H
#define DELETER_H

#include "BenchmarkTest.hpp"

enum Deleter_Operations
{
	MultipleFilesDeletion
};

class DeleterTest : public BenchmarkTest
{
public:
		
	DeleterTest(Mode m, int op, string a) : BenchmarkTest (m, op, a)
	{
		dirName = "results";
	}
		
	~DeleterTest() {}
	
	Status Main();
private:
	string dirName;
	Status MultipleFilesDeletionFunc();
	string CreateCommand();
};

#endif /* DELETER_H */
