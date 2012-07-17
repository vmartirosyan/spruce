#include <SyscallTest.hpp>
#include "dup/DupFileDescriptor.hpp"
#include "read_write/ReadWriteFile.hpp"
#include "close/Close.hpp"
#include "link/Link.hpp"
#include "chmod/Chmod.hpp"
#include "chown/Chown.hpp"
#include "fcntl/fcntlFD.hpp"
#include "getdents/getdents.hpp"
#include "ioctl/IoctlTest.hpp"
#include "rename/rename.hpp"
#include "readv_writev/ReadvWritev.hpp"
#include "unlink/Unlink.hpp"
#include "unlinkat/UnlinkAt.hpp"
#include "stat/StatTest.hpp"
#include "fsync/FSyncTest.hpp"
#include "fdatasync/FDataSyncTest.hpp"
#include "pread_pwrite/PreadPwrite.hpp"
#include "creat/Creat.hpp"
#include "umask/Umask.hpp"
#include "select/Select.hpp"
#include "lseek/LseekTest.hpp"
#include "mknod/MknodTest.hpp"
#include "mmap/MmapMemory.hpp"
#include "mlock/MlockMemory.hpp"
#include "msync/MsyncMemory.hpp"
#include "mprotect/MprotectMemory.hpp"
#include "readdir/ReadDir.hpp"
#include "open/Open.hpp"
#include "openat/Openat.hpp"
#include "linkat/Linkat.hpp"
#include "readlink/Readlink.hpp"

#include "xattr/Xattr.hpp"
#include <config.hpp>
#include <time.h>
#include <platform_config.hpp>

// Karen
//#include "Mount_Umount.hpp"
// Gio
#include "chdir/Chdir.hpp"
#include "mkdir_rmdir/Mkdir_rmdir.hpp"
#include "getcwd/Getcwd.hpp"
// Vahram
#include "utime/Utime.hpp"
#include "access/Access.hpp"
#include "fstat/Fstat.hpp"
#include "statfs/Statfs.hpp"
#include "fstatfs/Fstatfs.hpp"
#include "truncate/Truncate.hpp"
#include "ftruncate/Ftruncate.hpp"
// Suren
#include "bdflush/Bdflush.hpp"
// Gurgen
#include "fchmod/Fchmod.hpp"
#include "sync/Sync.hpp"
#include "fchown/Fchown.hpp"
// Ruzanna

// Ani

int main(int argc, char ** argv)
{
	srand(time(0));
	TestCollection tests;
	try
	{
		Configuration<DupFileDescriptorTest> conf1(INSTALL_PREFIX"/share/spruce/config/dup.conf");
		Configuration<ReadWriteFileTest> conf2(INSTALL_PREFIX"/share/spruce/config/read_write.conf");
		Configuration<Close> conf3(INSTALL_PREFIX"/share/spruce/config/close.conf");
		Configuration<Chmod> conf4(INSTALL_PREFIX"/share/spruce/config/chmod.conf");
		Configuration<LinkTest> conf5(INSTALL_PREFIX"/share/spruce/config/link.conf");
		Configuration<fcntlFD> conf6(INSTALL_PREFIX"/share/spruce/config/fcntl.conf");
		Configuration<IoctlTest> conf7(INSTALL_PREFIX"/share/spruce/config/ioctl.conf");
		Configuration<GetDentsTest> conf8(INSTALL_PREFIX"/share/spruce/config/getdents.conf");
		Configuration<RenameTest> conf9(INSTALL_PREFIX"/share/spruce/config/rename.conf");
		Configuration<ReadvWritev> conf10(INSTALL_PREFIX"/share/spruce/config/readv_writev.conf");
		Configuration<UnlinkTest> conf11(INSTALL_PREFIX"/share/spruce/config/unlink.conf");
		Configuration<UnlinkAtTest> conf12(INSTALL_PREFIX"/share/spruce/config/unlinkat.conf");
		Configuration<CreatTest> conf13(INSTALL_PREFIX"/share/spruce/config/creat.conf");
		Configuration<Chown> chownConf(INSTALL_PREFIX"/share/spruce/config/chown.conf");
		Configuration<StatTest> statConf(INSTALL_PREFIX"/share/spruce/config/stat.conf");
		Configuration<FSyncTest> fsyncConf(INSTALL_PREFIX"/share/spruce/config/fsync.conf");
		Configuration<FDataSyncTest> fdatasyncConf(INSTALL_PREFIX"/share/spruce/config/fdatasync.conf");
		Configuration<PreadPwrite> preadPwriteConf(INSTALL_PREFIX"/share/spruce/config/pread_pwrite.conf");
		Configuration<Umask> umaskConf(INSTALL_PREFIX"/share/spruce/config/umask.conf");
		Configuration<SelectTest> selectConf(INSTALL_PREFIX"/share/spruce/config/select.conf");
		Configuration<LseekTest> lseekConf(INSTALL_PREFIX"/share/spruce/config/lseek.conf");
		Configuration<MknodTest> mknodConf(INSTALL_PREFIX"/share/spruce/config/mknod.conf");
		Configuration<MmapMemoryTest> mmapConf(INSTALL_PREFIX"/share/spruce/config/mmap.conf");
		Configuration<MsyncMemoryTest> msyncConf(INSTALL_PREFIX"/share/spruce/config/msync.conf");
		Configuration<MlockMemoryTest> mlockConf(INSTALL_PREFIX"/share/spruce/config/mlock.conf");
		Configuration<MprotectMemoryTest> mprotectConf(INSTALL_PREFIX"/share/spruce/config/mprotect.conf");
		Configuration<ReadDirTest> readdirConf(INSTALL_PREFIX"/share/spruce/config/readdir.conf");
		Configuration<ReadlinkTest> readlinkConf(INSTALL_PREFIX"/share/spruce/config/readlink.conf");
		Configuration<Openat> openatConf(INSTALL_PREFIX"/share/spruce/config/openat.conf");
		Configuration<Open> openConf(INSTALL_PREFIX"/share/spruce/config/open.conf");
		
		Configuration<LinkatTest> linkatConf(INSTALL_PREFIX"/share/spruce/config/linkat.conf");
		Configuration<Xattr> xattrConf(INSTALL_PREFIX"/share/spruce/config/xattr.conf");
		
		// Karen
		//Configuration<Mount_Umount> mount_umountConf(INSTALL_PREFIX"/share/spruce/config/mount_umount.conf");
		// Gio
		Configuration<Chdir> chdirConf(INSTALL_PREFIX"/share/spruce/config/chdir.conf");		
		Configuration<Getcwd> getcwdConf(INSTALL_PREFIX"/share/spruce/config/getcwd.conf");	
		Configuration<Mkdir_rmdir> mkdir_rmdirConf(INSTALL_PREFIX"/share/spruce/config/mkdir_rmdir.conf");		
		// Vahram
		Configuration<UtimeTest> utimeConf(INSTALL_PREFIX"/share/spruce/config/utime.conf");
		Configuration<AccessTest> accessConf(INSTALL_PREFIX"/share/spruce/config/access.conf");
		Configuration<FstatTest> fstatConf(INSTALL_PREFIX"/share/spruce/config/fstat.conf");
		Configuration<StatfsTest> statfsConf(INSTALL_PREFIX"/share/spruce/config/statfs.conf");
		Configuration<FstatfsTest> fstatfsConf(INSTALL_PREFIX"/share/spruce/config/fstatfs.conf");
		Configuration<TruncateTest> truncateConf(INSTALL_PREFIX"/share/spruce/config/truncate.conf");
		Configuration<FtruncateTest> ftruncateConf(INSTALL_PREFIX"/share/spruce/config/ftruncate.conf");
		// Suren
		Configuration<BdFlushTest> bdflushConf(INSTALL_PREFIX"/share/spruce/config/bdflush.conf");
		// Gurgen
			Configuration<Fchmod> fchmodConf(INSTALL_PREFIX"/share/spruce/config/fchmod.conf");
			Configuration<Sync> syncConf(INSTALL_PREFIX"/share/spruce/config/sync.conf");		
			Configuration<Fchown> fchownConf(INSTALL_PREFIX"/share/spruce/config/fchown.conf");		
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
		//TestCollection mount_umountTests = mount_umountConf.Read();
		// Gio
		TestCollection chdirTests = chdirConf.Read();
		TestCollection getcwdTests = getcwdConf.Read();
		TestCollection mkdir_rmdirTests = mkdir_rmdirConf.Read();		
		// Vahram
		TestCollection utimeTests = utimeConf.Read();
		TestCollection accessTests = accessConf.Read();
		TestCollection fstatTests = fstatConf.Read();
		TestCollection statfsTests = statfsConf.Read();
		TestCollection fstatfsTests = fstatfsConf.Read();
		TestCollection truncateTests = truncateConf.Read();
		TestCollection ftruncateTests = ftruncateConf.Read();
		// Suren
		TestCollection bdflushTest = bdflushConf.Read();
		// Gurgen
		TestCollection fchmodTests = fchmodConf.Read();
		TestCollection syncTests = syncConf.Read();
		TestCollection fchownTests = fchownConf.Read();
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
		//tests.Merge(mount_umountTests);		
		// Gio
		tests.Merge(chdirTests);	
		tests.Merge(getcwdTests);	
		tests.Merge(mkdir_rmdirTests);		
		// Vahram
		tests.Merge(utimeTests);
		tests.Merge(accessTests);
		tests.Merge(fstatTests);
		tests.Merge(statfsTests);
		tests.Merge(fstatfsTests);
		tests.Merge(truncateTests);
		tests.Merge(ftruncateTests);
		// Suren
		tests.Merge(bdflushTest);
		// Gurgen
		tests.Merge(fchmodTests);
		tests.Merge(syncTests);
		tests.Merge(fchownTests);
		// Ruzanna
		
		// Ani
		
		
		TestResultCollection res = tests.Run();		
		
		string XML = res.ToXML();
		unsigned int XMLLength = XML.size();
		
		XML = "\n<Module Name=\"Syscall\">\n\t" + XML + "\n</Module>\n";
		
		for ( unsigned int i = 0; i < XMLLength / 1000 + 1; ++i )
			cerr << XML.substr(i*1000, 1000);
		
		//cerr << "<Module Name=\"Syscall\">\n" << res.ToXML() << "</Module>" << endl;
			
		
		return res.GetStatus();
	}
	catch (Exception ex)
	{
		cerr << "Exception: " << ex.GetMessage();
		return EXIT_FAILURE;
	}

    
	
	
}

