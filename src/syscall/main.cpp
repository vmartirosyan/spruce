#include "SyscallTest.hpp"
#include "DupFileDescriptor.hpp"
#include "ReadWriteFile.hpp"

int main(int argc, char ** argv)
{
	TestCollection tests;		
	
	tests.AddTest(new DupFileDescriptorTest(Normal, Dup));	
	tests.AddTest(new ReadWriteFileTest(Normal, ReadBadFileDescriptor, ""));
	
	TestResultCollection res = tests.Run();
	
	cout << res.ToXML() << endl;	
}
