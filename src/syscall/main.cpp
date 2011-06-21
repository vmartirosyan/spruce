#include "SyscallTest.hpp"
#include "DupFileDescriptor.hpp"
#include "ReadWriteFile.hpp"
#include "Close.hpp"
#include "Link.hpp"
int main(int argc, char ** argv)
{
	TestCollection tests;
	
	tests.AddTest(new DupFileDescriptorTest(Normal, Dup));
	tests.AddTest(new ReadWriteFileTest(Normal, ReadBadFileDescriptor1, ""));
	tests.AddTest(new ReadWriteFileTest(Normal, ReadBadFileDescriptor2, ""));
	tests.AddTest(new ReadWriteFileTest(Normal, ReadEinvalError, ""));
	tests.AddTest(new ReadWriteFileTest(Normal, ReadIsdirError, ""));
	tests.AddTest(new ReadWriteFileTest(Normal, ReadEfaultError, ""));
	tests.AddTest(new ReadWriteFileTest(Normal, proba, ""));

	tests.AddTest(new Close(Normal, BadFileDescriptor, ""));
 	tests.AddTest(new Close(Normal, CorrectDescriptor, ""));
	tests.AddTest(new Close(Normal, CorrectDescriptorUnlink, ""));

	tests.AddTest(new LinkTest(Normal, TooLongOldPath, ""));
	tests.AddTest(new LinkTest(Normal, TooLongNewPath, ""));
	tests.AddTest(new LinkTest(Normal, NewPathAleadyExist, ""));
	tests.AddTest(new LinkTest(Normal, OldPathIsDirectory, ""));
	
	
	TestResultCollection res = tests.Run();
	
	cout << res.ToXML() << endl;	
}
