#include <stdlib.h>
#include "LargeFile.hpp"

Status LargeFileTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case LargeFileCreation:
				return LargeFileCreationFunc();
			default:
				cerr << "Unsupported operation.";	
				return Unres;		
		}
	}
	cerr << "Test was successful";	
	return Success;		
}

Status LargeFileTest::LargeFileCreationFunc()
{
	string command = CreateCommand();
    system(command.c_str());   
    
    return Success;
}

string LargeFileTest::CreateCommand()
{
	string command = (string)"./LargeFileCreation.sh" + (string)" " + filesNumber + (string)" " + fileSize;
	return command;
}
