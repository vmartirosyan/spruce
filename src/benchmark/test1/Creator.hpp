#ifndef CREATOR_H
#define CREATOR_H

#include "test_base.hpp"

enum Creator_Operations
{
	MultipleFilesCreation
};

class CreatorTest : public TestBase
{
public:	
	CreatorTest()
	{
		filesNumber = "10";
		fileSize = "1M";
		timestamp = 0.0;
	}
		
	virtual ~CreatorTest()
	{
	}
	virtual TestResultCollection Run();
protected:	
	virtual Status RealRun(int operation, string args);
private:
	string filesNumber;
	string fileSize;
	double timestamp;
	Status MultipleFilesCreationFunc();
	string CreateCommand();
};

#endif /* CREATOR_H */
