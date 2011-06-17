#include "SyscallTest.hpp"
#include "DupFileDescriptor.hpp"
#include "ReadWriteFile.hpp"

int main(int argc, char ** argv)
{
	TestCollection tests;
	
	tests.AddTest(new DupFileDescriptorTest(Normal, Dup));	
	tests.AddTest(new ReadWriteFileTest(Normal, ReadBadFileDescriptor1, ""));
	tests.AddTest(new ReadWriteFileTest(Normal, ReadBadFileDescriptor2, ""));
	tests.AddTest(new ReadWriteFileTest(Normal, ReadEinvalError, ""));
	
	TestResultCollection res = tests.Run();
	
	cout << res.ToXML() << endl;	
}
