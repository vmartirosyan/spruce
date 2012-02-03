#include "SyscallTest.hpp"
#include "DupFileDescriptor.hpp"
#include "ReadWriteFile.hpp"
#include "Close.hpp"
#include "Link.hpp"
#include "Chmod.hpp"
#include "Chown.hpp"
#include "fcntlFD.hpp"
#include "getdents.hpp"
#include "IoctlTest.hpp"
#include "rename.hpp"
#include "ReadvWritev.hpp"
#include "Unlink.hpp"
#include "UnlinkAt.hpp"
#include "StatTest.hpp"
#include "FSyncTest.hpp"
#include "FDataSyncTest.hpp"
#include "PreadPwrite.hpp"
#include "Creat.hpp"
#include "Umask.hpp"
#include "Select.hpp"
#include "LseekTest.hpp"
#include "MknodTest.hpp"
#include "MmapMemory.hpp"
#include "MlockMemory.hpp"
#include "MsyncMemory.hpp"
#include "MprotectMemory.hpp"
#include "ReadDir.hpp"
#include "Open.hpp"
#include "Openat.hpp"
#include "Linkat.hpp"
#include "Readlink.hpp"
#include <config.hpp>
#include <time.h>

int main(int argc, char ** argv)
{
	srand(time(0));
	TestCollection tests;
	try
	{
		Configuration<DupFileDescriptorTest> conf1("${CMAKE_INSTALL_PREFIX}/config/dup.conf");
		Configuration<ReadWriteFileTest> conf2("${CMAKE_INSTALL_PREFIX}/config/read_write.conf");
		Configuration<Close> conf3("${CMAKE_INSTALL_PREFIX}/config/close.conf");
		Configuration<Chmod> conf4("${CMAKE_INSTALL_PREFIX}/config/chmod.conf");
		Configuration<LinkTest> conf5("${CMAKE_INSTALL_PREFIX}/config/link.conf");
		Configuration<fcntlFD> conf6("${CMAKE_INSTALL_PREFIX}/config/fcntl.conf");
		Configuration<IoctlTest> conf7("${CMAKE_INSTALL_PREFIX}/config/ioctl.conf");
		Configuration<GetDentsTest> conf8("${CMAKE_INSTALL_PREFIX}/config/getdents.conf");
		Configuration<RenameTest> conf9("${CMAKE_INSTALL_PREFIX}/config/rename.conf");
		Configuration<ReadvWritev> conf10("${CMAKE_INSTALL_PREFIX}config/readv_writev.conf");
		Configuration<UnlinkTest> conf11("${CMAKE_INSTALL_PREFIX}/config/unlink.conf");
		Configuration<UnlinkAtTest> conf12("${CMAKE_INSTALL_PREFIX}/config/unlinkat.conf");
		Configuration<CreatTest> conf13("${CMAKE_INSTALL_PREFIX}/config/creat.conf");
		Configuration<Chown> chownConf("${CMAKE_INSTALL_PREFIX}/config/chown.conf");
		Configuration<StatTest> statConf("${CMAKE_INSTALL_PREFIX}/config/stat.conf");
		Configuration<FSyncTest> fsyncConf("${CMAKE_INSTALL_PREFIX}/config/fsync.conf");
		Configuration<FDataSyncTest> fdatasyncConf("${CMAKE_INSTALL_PREFIX}/config/fdatasync.conf");
		Configuration<PreadPwrite> preadPwriteConf("${CMAKE_INSTALL_PREFIX}/config/pread_pwrite.conf");
		Configuration<Umask> umaskConf("${CMAKE_INSTALL_PREFIX}/config/umask.conf");
		Configuration<SelectTest> selectConf("${CMAKE_INSTALL_PREFIX}/config/select.conf");
		Configuration<LseekTest> lseekConf("${CMAKE_INSTALL_PREFIX}/config/lseek.conf");
		Configuration<MknodTest> mknodConf("${CMAKE_INSTALL_PREFIX}/config/mknod.conf");
		Configuration<MmapMemoryTest> mmapConf("${CMAKE_INSTALL_PREFIX}/config/mmap.conf");
		Configuration<MsyncMemoryTest> msyncConf("${CMAKE_INSTALL_PREFIX}/config/msync.conf");
		Configuration<MlockMemoryTest> mlockConf("${CMAKE_INSTALL_PREFIX}/config/mlock.conf");
		Configuration<MprotectMemoryTest> mprotectConf("${CMAKE_INSTALL_PREFIX}/config/mprotect.conf");
		Configuration<ReadDirTest> readdirConf("${CMAKE_INSTALL_PREFIX}/config/readdir.conf");
		Configuration<ReadlinkTest> readlinkConf("${CMAKE_INSTALL_PREFIX}/config/readlink.conf");
		Configuration<Openat> openatConf("${CMAKE_INSTALL_PREFIX}/config/openat.conf");
		Configuration<Open> openConf("${CMAKE_INSTALL_PREFIX}/config/open.conf");
		Configuration<LinkatTest> linkatConf("${CMAKE_INSTALL_PREFIX}/config/linkat.conf");
		
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
		TestCollection tests12 = conf12.Read();
		TestCollection tests13 = conf13.Read();
		TestCollection statTests = statConf.Read();
		TestCollection fsyncTests = fsyncConf.Read();
		TestCollection fdatasyncTests = fdatasyncConf.Read();
		TestCollection preadPwriteTests = preadPwriteConf.Read();
		TestCollection chownTests = chownConf.Read();
		TestCollection umaskTests = umaskConf.Read();
		TestCollection selectTests = selectConf.Read();
		TestCollection lseekTests = lseekConf.Read();
		TestCollection mknodTests = mknodConf.Read();
		TestCollection mmapTests = mmapConf.Read();
		TestCollection msyncTests = msyncConf.Read();
		TestCollection mlockTests = mlockConf.Read();
		TestCollection mprotectTests = mprotectConf.Read();
		TestCollection readdirTests = readdirConf.Read();
		TestCollection readlinkTests = readlinkConf.Read();
		TestCollection openatTests = openatConf.Read();
		TestCollection openTests = openConf.Read();
		TestCollection linkatTests = linkatConf.Read();
		
	  
	  
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
		tests.Merge(tests12);
		tests.Merge(tests13);
		tests.Merge(statTests);
		tests.Merge(fsyncTests);
		tests.Merge(fdatasyncTests);
		tests.Merge(preadPwriteTests);
		tests.Merge(chownTests);
		tests.Merge(umaskTests);
		tests.Merge(selectTests);
		tests.Merge(lseekTests);
		tests.Merge(mknodTests);
		tests.Merge(mmapTests);
		tests.Merge(msyncTests);
		tests.Merge(mlockTests);
		tests.Merge(mprotectTests);
		tests.Merge(readdirTests);
		tests.Merge(readlinkTests);
		tests.Merge(openatTests);
		tests.Merge(openTests);
		tests.Merge(linkatTests);
	}
	catch (Exception ex)
	{
		cerr << "Exception: " << ex.GetMessage();
		return EXIT_FAILURE;
	}

    
	TestResultCollection res = tests.Run();
	//cerr << "bla bla" << endl;
	cerr << "<Syscalls>\n" << res.ToXML() << "</Syscalls>" << endl;
	//cerr << res.ToXML() << endl;

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
