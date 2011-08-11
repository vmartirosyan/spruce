#include "SyscallTest.hpp"
#include "DupFileDescriptor.hpp"
#include "ReadWriteFile.hpp"
#include "Close.hpp"
#include "Link.hpp"
#include "Chmod.hpp"
#include "fcntlFD.hpp"
#include "getdents.hpp"
#include "IoctlTest.hpp"
#include "rename.hpp"
#include "Creat.hpp"
#include <config.hpp>
#include <Unlink.hpp>

int main(int argc, char ** argv)
{
	TestCollection tests;
	Configuration<DupFileDescriptorTest> conf1("${CMAKE_INSTALL_PREFIX}/config/dup.conf");
	Configuration<ReadWriteFileTest> conf2("${CMAKE_INSTALL_PREFIX}/config/read_write.conf");
	Configuration<Close> conf3("${CMAKE_INSTALL_PREFIX}/config/close.conf");
	Configuration<Chmod> conf4("${CMAKE_INSTALL_PREFIX}/config/chmod.conf");
	Configuration<LinkTest> conf5("${CMAKE_INSTALL_PREFIX}/config/link.conf");
	Configuration<fcntlFD> conf6("${CMAKE_INSTALL_PREFIX}/config/fcntl.conf");
	Configuration<IoctlTest> conf7("${CMAKE_INSTALL_PREFIX}/config/ioctl.conf");
	Configuration<GetDentsTest> conf8("${CMAKE_INSTALL_PREFIX}/config/getdents.conf");
	Configuration<RenameTest> conf9("${CMAKE_INSTALL_PREFIX}/config/rename.conf");
	Configuration<CreatTest> conf10("${CMAKE_INSTALL_PREFIX}/config/creat.conf");
	Configuration<UnlinkTest> conf11("${CMAKE_INSTALL_PREFIX}/config/unlink.conf");
	

	TestCollection tests1 = conf1.Read();
	TestCollection tests2 = conf2.Read();
	TestCollection tests3 = conf3.Read();
	TestCollection tests4 = conf4.Read();
	TestCollection tests5 = conf5.Read();
	TestCollection tests6 = conf6.Read();
	TestCollection tests7 = conf7.Read();
	TestCollection tests8 = conf8.Read();
	TestCollection tests9 = conf9.Read();
	TestCollection tests10 = conf10.Read();
	TestCollection tests11 = conf11.Read();
	
	

	tests.Merge(tests1);
	tests.Merge(tests2);
	tests.Merge(tests3);
	tests.Merge(tests4);
	tests.Merge(tests5);
	tests.Merge(tests6);
	tests.Merge(tests7);
	tests.Merge(tests8);
	tests.Merge(tests9);
	tests.Merge(tests10);
	tests.Merge(tests11);

	TestResultCollection res = tests.Run();
	cout << res.ToXML() << endl;

	/*tests.AddTest(new DupFileDescriptorTest(Normal, Dup));
	tests.AddTest(new DupFileDescriptorTest(Normal, Dup));

	tests.AddTest(new Close(Normal, BadFileDescriptor, ""));
 	tests.AddTest(new Close(Normal, CorrectDescriptor, ""));
	tests.AddTest(new Close(Normal, CorrectDescriptorUnlink, ""));

	tests.AddTest(new Chmod(Normal, CHMOD_S_IRWXU, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_IRUSR, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_IWUSR, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_IXUSR, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_IRWXG, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_IRGRP, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_IWGRP, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_IXGRP, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_IRWXO, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_IROTH, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_IWOTH, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_IXOTH, ""));
	tests.AddTest(new Chmod(Normal, CHMOD_S_ISUID, ""));
    tests.AddTest(new Chmod(Normal,  CHMOD_ERR_ENAMETOOLONG, ""));
    tests.AddTest(new Chmod(Normal,  CHMOD_ERR_ENOENT, ""));
     tests.AddTest(new Chmod(Normal,  CHMOD_ERR_ENOTDIR, ""));


	tests.AddTest(new LinkTest(Normal, LinkTestTooLongOldPath, ""));
	tests.AddTest(new LinkTest(Normal, LinkTestTooLongNewPath, ""));
	tests.AddTest(new LinkTest(Normal, LinkTestNewPathAleadyExist, ""));
	tests.AddTest(new LinkTest(Normal, LinkTestOldPathIsDirectory, ""));
	tests.AddTest(new LinkTest(Normal, LinkTestNormalFile, ""));
	tests.AddTest(new LinkTest(Normal, LinkTestIsNotDirectory, ""));

	TestResultCollection res = tests.Run();

	cout << res.ToXML() << endl;*/
}
