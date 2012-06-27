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

#include "Xattr.hpp"
#include <config.hpp>
#include <time.h>

// Karen
		
// Gio
#include "chdir.hpp"
// Vahram
#include "Utime.hpp"
// Suren

// Gurgen
#include "Fchmod.hpp"
#include "Sync.hpp"
// Ruzanna

// Ani

int main(int argc, char ** argv)
{
	srand(time(0));
	TestCollection tests;
	try
	{
		Configuration<DupFileDescriptorTest> conf1("${CMAKE_INSTALL_PREFIX}/share/spruce/config/dup.conf");
		Configuration<ReadWriteFileTest> conf2("${CMAKE_INSTALL_PREFIX}/share/spruce/config/read_write.conf");
		Configuration<Close> conf3("${CMAKE_INSTALL_PREFIX}/share/spruce/config/close.conf");
		Configuration<Chmod> conf4("${CMAKE_INSTALL_PREFIX}/share/spruce/config/chmod.conf");
		Configuration<LinkTest> conf5("${CMAKE_INSTALL_PREFIX}/share/spruce/config/link.conf");
		Configuration<fcntlFD> conf6("${CMAKE_INSTALL_PREFIX}/share/spruce/config/fcntl.conf");
		Configuration<IoctlTest> conf7("${CMAKE_INSTALL_PREFIX}/share/spruce/config/ioctl.conf");
		Configuration<GetDentsTest> conf8("${CMAKE_INSTALL_PREFIX}/share/spruce/config/getdents.conf");
		Configuration<RenameTest> conf9("${CMAKE_INSTALL_PREFIX}/share/spruce/config/rename.conf");
		Configuration<ReadvWritev> conf10("${CMAKE_INSTALL_PREFIX}/share/spruce/config/readv_writev.conf");
		Configuration<UnlinkTest> conf11("${CMAKE_INSTALL_PREFIX}/share/spruce/config/unlink.conf");
		Configuration<UnlinkAtTest> conf12("${CMAKE_INSTALL_PREFIX}/share/spruce/config/unlinkat.conf");
		Configuration<CreatTest> conf13("${CMAKE_INSTALL_PREFIX}/share/spruce/config/creat.conf");
		Configuration<Chown> chownConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/chown.conf");
		Configuration<StatTest> statConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/stat.conf");
		Configuration<FSyncTest> fsyncConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/fsync.conf");
		Configuration<FDataSyncTest> fdatasyncConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/fdatasync.conf");
		Configuration<PreadPwrite> preadPwriteConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/pread_pwrite.conf");
		Configuration<Umask> umaskConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/umask.conf");
		Configuration<SelectTest> selectConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/select.conf");
		Configuration<LseekTest> lseekConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/lseek.conf");
		Configuration<MknodTest> mknodConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/mknod.conf");
		Configuration<MmapMemoryTest> mmapConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/mmap.conf");
		Configuration<MsyncMemoryTest> msyncConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/msync.conf");
		Configuration<MlockMemoryTest> mlockConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/mlock.conf");
		Configuration<MprotectMemoryTest> mprotectConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/mprotect.conf");
		Configuration<ReadDirTest> readdirConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/readdir.conf");
		Configuration<ReadlinkTest> readlinkConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/readlink.conf");
		Configuration<Openat> openatConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/openat.conf");
		Configuration<Open> openConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/open.conf");
		
		Configuration<LinkatTest> linkatConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/linkat.conf");
		Configuration<Xattr> xattrConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/xattr.conf");
		
		// Karen
				
		// Gio
		Configuration<Chdir> chdirConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/chdir.conf");			
		// Vahram
		Configuration<UtimeTest> utimeConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/utime.conf");
		// Suren
		
		// Gurgen
			Configuration<Fchmod> fchmodConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/fchmod.conf");
			Configuration<Sync> syncConf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/sync.conf");		
		// Ruzanna
		
		// Ani
		
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
		TestCollection xattrTests = xattrConf.Read();
		
		// Karen
				
		// Gio
		TestCollection chdirTests = chdirConf.Read();		
		// Vahram
		TestCollection utimeTests = utimeConf.Read();
		// Suren
		
		// Gurgen
		TestCollection fchmodTests = fchmodConf.Read();
		TestCollection syncTests = syncConf.Read();
		// Ruzanna
		
		// Ani
	  
	  
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
		tests.Merge(xattrTests);
		
		// Karen
				
		// Gio
		tests.Merge(chdirTests);		
		// Vahram
		tests.Merge(utimeTests);
		// Suren
		
		// Gurgen
		tests.Merge(fchmodTests);
		tests.Merge(syncTests);
		// Ruzanna
		
		// Ani
	}
	catch (Exception ex)
	{
		cerr << "Exception: " << ex.GetMessage();
		return EXIT_FAILURE;
	}

    
	TestResultCollection res = tests.Run();
	
	cerr << "<Syscalls>\n" << res.ToXML() << "</Syscalls>" << endl;
	
}
