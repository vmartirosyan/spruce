#include <stdlib.h>
#include "Creator.hpp"

Status CreatorTest::Main()
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case MultipleFilesCreation:
				return MultipleFilesCreationFunc();
			default:
				cerr << "Unsupported operation.";	
				return Unres;		
		}
	}
	cerr << "Test was successful";	
	return Success;		
}

Status CreatorTest::MultipleFilesCreationFunc()
{
	string command = CreateCommand();
    system(command.c_str());   
    
    return Success;
}

string CreatorTest::CreateCommand()
{
	string command = (string)"./test1.sh" + (string)" " + filesNumber + (string)" " + fileSize;
	return command;
}
