#include "SyscallTest.hpp"
#include "DupFileDescriptor.hpp"
#include "ReadWriteFile.hpp"
#include "Close.hpp"

int main(int argc, char ** argv)
{
	TestCollection tests;
	
	tests.AddTest(new DupFileDescriptorTest(Normal, Dup));
	tests.AddTest(new ReadWriteFileTest(Normal, ReadBadFileDescriptor1, ""));
	tests.AddTest(new ReadWriteFileTest(Normal, ReadBadFileDescriptor2, ""));
	tests.AddTest(new ReadWriteFileTest(Normal, ReadEinvalError, ""));
	tests.AddTest(new ReadWriteFileTest(Normal, ReadIsdirError, ""));
	tests.AddTest(new ReadWriteFileTest(Normal, proba, ""));
	tests.AddTest(new Close(Normal, BadFileDescriptor, ""));
 	tests.AddTest(new Close(Normal, CorrectDescriptor, ""));
	tests.AddTest(new Close(Normal, CorrectDescriptorUnlink, ""));
	
	TestResultCollection res = tests.Run();
	
	cout << res.ToXML() << endl;	
}
