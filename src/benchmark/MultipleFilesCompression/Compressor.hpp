#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include "BenchmarkTest.hpp"

enum CompressOperations
{
	MultipleFilesCompression
};

class CompressTest : public BenchmarkTest
{
public:
		
	CompressTest(Mode m, int op, string a) : BenchmarkTest (m, op, a)
	{
		directory = "results";
	}
		
	~CompressTest()
	{
	}
	
	Status Main();
private:
	string directory;
	Status MultipleFilesCompressionFunc();
	string CreateCommand();
};

#endif
