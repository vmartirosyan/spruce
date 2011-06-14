#include <stdlib.h>
#include "Deleter.hpp"

int DeleterTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case MultipleFilesDeletion:
				return MultipleFilesDeletionFunc();
			default:
				cerr << "Unsupported operation.";	
				return Unres;		
		}
	}
	
	cerr << "Test was successful";	
	return Success;		
}

Status DeleterTest::MultipleFilesDeletionFunc()
{
	string command = CreateCommand();
    system(command.c_str());   
    
    return Success;
}

string DeleterTest::CreateCommand()
{
	string command = (string)"./MultipleFilesDeletion.sh" + (string)" " + dirName;
	return command;
}
