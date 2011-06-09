#include <stdlib.h>
#include "Compressor.hpp"

Status CompressTest::Main()
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case MultipleFilesCompression:
				return MultipleFilesCompressionFunc();
			default:
				cerr << "Unsupported operation.";	
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status CompressTest::MultipleFilesCompressionFunc()
{
	string command = CreateCommand();
    system(command.c_str());
    
    return Success;
}

string CompressTest::CreateCommand()
{
	string command = (string)"./MultipleFileCompression.sh" + (string)" " + directory;
	return command;
}
