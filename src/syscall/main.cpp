#include "SyscallTest.hpp"
#include "DupFileDescriptor.hpp"

int main(int argc, char ** argv)
{
	TestCollection tests;		
	
	tests.AddTest(new DupFileDescriptorTest(Normal, Dup));	
	
	TestResultCollection res = tests.Run();
	
	cout << res.ToXML() << endl;	
}
