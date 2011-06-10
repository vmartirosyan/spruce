#include <stdlib.h>
#include "FindFile.hpp"

int FindFileTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case FindFile:
				return FindFileCreationFunc();
			default:
				cerr << "Unsupported operation.";	
				return Unres;		
		}
	}
	cerr << "Test was successful";	
	return Success;		
}

Status FindFileTest::FindFileCreationFunc()
{
	string command = CreateCommand();
    system(command.c_str());   
    
    return Success;
}

string FindFileTest::CreateCommand()
{
	string command = (string)"./FindFile.sh" + (string)" " + directoryName + (string)" " + fileName;
	return command;
}
