//		PartitionManager.cpp
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Authors:
//				Ruzanna Karakozova <r.karakozova@gmail.com>
//				Vahram Martirosyan <vmartirosyan@gmail.com>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.

#include <PartitionManager.hpp>
#include <algorithm>

using std::merge;

std::pair<std::string, unsigned long> map_data[] = {
    std::make_pair("user", 0),
    std::make_pair("atime", 0),
    std::make_pair("dev", 0),
    std::make_pair("diratime", 0),
    std::make_pair("exec", 0),
    std::make_pair("suid", 0),
    std::make_pair("rw", 0),
    std::make_pair("async", 0),
    std::make_pair("_netdev", 0),
    std::make_pair("auto", 0),
    std::make_pair("noauto", 0),
    std::make_pair("defaults", 0),
    std::make_pair("group", 0),
    std::make_pair("iversion", 0),
    std::make_pair("noiversion", 0),
    std::make_pair("loud", 0),
    std::make_pair("nofail", 0),
    std::make_pair("nomand", 0),
    std::make_pair("nostrictatime", 0),
    std::make_pair("nouser", 0),
    std::make_pair("owner", 0),
    std::make_pair("users", 0),
    
    std::make_pair("mand", MS_MANDLOCK),
    std::make_pair("dirsync", MS_DIRSYNC),
    std::make_pair("noatime", MS_NOATIME),
    std::make_pair("nodev", MS_NODEV),
    std::make_pair("nodiratime", MS_NODIRATIME),
    std::make_pair("noexec", MS_NOEXEC),
    std::make_pair("nosuid", MS_NOSUID),
    std::make_pair("ro", MS_RDONLY),
    std::make_pair("relatime", MS_RELATIME),
    std::make_pair("remount", MS_REMOUNT),
    std::make_pair("silent", MS_SILENT),
    std::make_pair("strictatime", MS_STRICTATIME),
    std::make_pair("sync", MS_SYNCHRONOUS)    
};

std::map<string, unsigned long> PartitionManager::MountFlagMap(map_data,
    map_data + sizeof map_data / sizeof map_data[0]);

std::pair<std::string, unsigned long> map2_data[] = {
    std::make_pair("ext4",1024),
	std::make_pair("btrfs",64000),
	std::make_pair("xfs",0),
	std::make_pair("jfs",32768)
};

std::map<string, unsigned long> PartitionManager::SBOffsets(map2_data,
    map2_data + sizeof map2_data / sizeof map2_data[0]);
    
void PartitionManager::Initialize(string ConfigFile, string DeviceName, string MountPoint, string FileSystem, string MountOpts)
{			
	_ConfigFile = ConfigFile;
	_DeviceName = DeviceName;
	_MountPoint = MountPoint;
	_FileSystem = FileSystem;
	_MountOpts = MountOpts;		
	//_CurrentMountOptions = "";
	_Index = _AdditionalOptions[FS_UNSUPPORTED].end();
	_FSIndex = FS_UNSUPPORTED;
	if ( _MountOpts == "" )
	{
		LoadConfiguration();
	}
	else
	{
		vector<string> vOpts = SplitString(_MountOpts, ';');
		
		for ( size_t i = 0; i < vOpts.size(); ++i )
		{
			CurrentOptions tmp;
			tmp.ParseString(vOpts[i].c_str());
			_AdditionalOptions[GetFSNumber(FileSystem)].insert(tmp);
		}
	}
	_FSIndex = GetFSNumber(_FileSystem);
	_Index = _AdditionalOptions[_FSIndex].begin();
	_CurrentOptions = *_Index;
}
    
void PartitionManager::AdvanceOptionsIndex()
{
	_Index++;
	if ( _Index != _AdditionalOptions[_FSIndex].end() )
	{
		_CurrentOptions = *_Index;
	}
	else
	{
		_CurrentOptions.RawData = "";
		_CurrentOptions.MkfsOptions = "";
		_CurrentOptions.MountData = "";
		_CurrentOptions.MountFlags = 0;
	}
}

PartitionStatus PartitionManager::PreparePartition()
{
	Logger::LogInfo((string)"Preparing partition `" + _DeviceName + "`");
	
	if ( _Index == _AdditionalOptions[_FSIndex].end() )
	{
		return PS_Done;
	}
	
	if ( !ReleasePartition(_MountPoint) )
	{
		return PS_Fatal;
	}
	
	Logger::LogInfo("Preparing partition: " + GetCurrentOptions(false));
	if ( !CreateFilesystem(_FileSystem, _DeviceName, false, _CurrentOptions.MkfsOptions) )
	{		
		AdvanceOptionsIndex();	
		return PS_Skip;
	}
	Logger::LogInfo((string)"File system " + _FileSystem + " is created successfully.");	
	
	if( !Mount(_DeviceName,_MountPoint,_FileSystem, _CurrentOptions.MountData, _CurrentOptions.MountFlags) )
	{
		AdvanceOptionsIndex();
		return PS_Skip;
	}
	// Change the owner to the 'nobody' user. Makes some tests (like Access) to cleanup correctly
	struct passwd * nobody = getpwnam("nobody");
	
	if ( nobody != NULL )
	{
		chown( _MountPoint.c_str(), nobody->pw_uid, nobody->pw_gid);			
	}
	AdvanceOptionsIndex();
	
	return PS_Success;
}

// Try to re-create the file system and mount it with the precious mount options.		
PartitionStatus PartitionManager::RestorePartition(string DeviceName, string MountPoint, string FileSystem, bool RecreateFilesystem, bool resizeFlag)
{
	if( !getenv("MountFlags") || !getenv("MountData") || ( RecreateFilesystem && !getenv("MkfsOpts") ) )
		return PS_Fatal;
	if( !ReleasePartition(MountPoint))
		return PS_Skip;
	if( RecreateFilesystem && !CreateFilesystem(FileSystem, DeviceName,resizeFlag, getenv("MkfsOpts")))
		return PS_Fatal;
	
	if( !Mount(DeviceName,MountPoint,FileSystem,getenv("MountData"), atoi(getenv("MountFlags"))) )
		return PS_Skip;
	return PS_Success;
}


bool PartitionManager::ReleasePartition(string MountPoint, string* output)
{
	Logger::LogInfo((string)"Unmounting partition " + MountPoint);
	if ( chdir("/") == -1)
		return false;
	
	int RetryCount = 0;
	
	ProcessResult * res = NULL;
	
	UnixCommand grep("grep");
	vector<string> grep_args;
	grep_args.push_back(MountPoint);
	grep_args.push_back("/proc/mounts");
	
	
	res = grep.Execute(grep_args);
	
	if ( res->GetStatus() != 0 )
	{
		Logger::LogInfo("Device was not mounted.");
		return true;
	}
	delete res;

	// There is a data race problem in VFS which brings to crashes on unmount.
	// Let's sync the data first!
	sync();	
	
	UnixCommand unmount("umount");
	vector<string> unmount_args;
	unmount_args.push_back(MountPoint);  
	
retry:		
	
	//res = unmount.Execute(unmount_args);
	Logger::LogInfo((string)"Unmounting partition " + MountPoint);
	int res1 = umount(MountPoint.c_str());	
	//if ( res->GetStatus() != Success )
	if ( res1 == -1 )
	{
		if ( errno == EBUSY  )
		{
			sleep(1);
			if ( RetryCount++ < 5 )
			{
				Logger::LogWarn((string)"Device was busy.. retrying... ");
				system("lsof | grep spruce");
				goto retry;
			}
		}
		if(output != NULL)
			*output = string("Cannot unmount partition ") + MountPoint + string(". ") + strerror(errno);
		Logger::LogError("Cannot unmount partition " + MountPoint + ". ");
		return false;
	}
	
	Logger::LogInfo((string)"Device is unmounted successfully.");
	
	return true;
}

bool PartitionManager::Mount(string DeviceName,string MountPoint,string FileSystem,string Options, unsigned long Flags)
{			
	
	if ( Options[Options.length() - 1] == ',' )
		Options.erase(Options.begin() + Options.length() - 1);
	
	const int size = 10;
	char buf[size];
	if ( snprintf(buf, size, "%lu", Flags) >= size )
	{
		Logger::LogError("Cannot get Flags value.");
		return false;
	}
		
	// There was a problem in BtrFS (old kernels) with mount.
	// That's why it makes sense to retry mounting several times with a delay.
	int ret_cnt;
	for(ret_cnt = 0; ret_cnt < 5; ++ret_cnt)
	{
		if ( mount(DeviceName.c_str(), MountPoint.c_str(), FileSystem.c_str(), Flags, Options.c_str()) == 0 )
		{
			break;
		}
		Logger::LogError("Cannot mount device, retrying.");		
		sleep(1);
	}
	if(ret_cnt == 5)
	{
		Logger::LogError("Cannot mount device.");		
		return false;
	}
	
	setenv("MountFlags", buf, 1);
	setenv("MountData", Options.c_str(), 1);
	
	Logger::LogInfo( "Device " + DeviceName + " was mounted on folder " 
			+ MountPoint + "(opts=" + Options + ") ");
	
	
	// Now change current dir to the newly mounted partition folder
	/*
	if ( chdir(MountPoint.c_str()) != 0 )
	{
		cerr << "Cannot change current dir to " << MountPoint << endl;
		cerr << "Error: " << strerror(errno) << endl;
		return false;
	}	
	*/
	//cout << "Changed dir" << endl;
	return true;

	Logger::LogError("Cannot unmount partition " + MountPoint + ".");
	return false;

}

string PartitionManager::GetCurrentOptions(bool Stripped) const
{	
	if ( !Stripped )
		return _CurrentOptions.RawData;
		
	string tmp = _CurrentOptions.RawData;
	for ( string::iterator i = tmp.begin(); i != tmp.end(); ++i )
	{
		if (*i == ' ')
			*i = '_';
		if (*i == '/')
			*i = '_';
	}
	return tmp;
}

void PartitionManager::ClearCurrentOptions()
{
	_Index = _AdditionalOptions[_FSIndex].begin();
	//_CurrentMountOptions = "";
}

bool PartitionManager::IsOptionEnabled(string optionName, bool IsMkfsOption)
{
	if(IsMkfsOption)
	{
		return IsMkfsOptionEnabled(optionName);
	}
	return IsMountOptionEnabled(optionName);
}


bool PartitionManager::NoOptionsEnabled()
{
	return NoMountOptionsEnabled() && NoMkfsOptionsEnabled();
}


bool PartitionManager::NoMountOptionsEnabled()
{
	char * sFlags = getenv("MountFlags");
	char * sData = getenv("MountData");
	if ( !sFlags && !sData )
		return true;
		
	if ( sFlags )
	{
		if ( strcmp("0", sFlags) )
			return false;
	}
	
	if ( sData )
	{
		if ( strcmp("", sData) )
			return false;
	}
	
	return true;
}


bool PartitionManager::NoMkfsOptionsEnabled()
{
	char * sOpts = getenv("MkfsOpts");
	if ( !sOpts || !strcmp("", sOpts))
	{
		return true;
	}
	
	return false;
}


bool PartitionManager::IsUserQuotaEnabled()
{
	return IsMountOptionEnabled("quota") || IsMountOptionEnabled("usrquota") || IsMountOptionEnabled("uquota") || IsMountOptionEnabled("uqnoenforce") || IsMountOptionEnabled("qnoenforce") || IsMountOptionEnabled("usrjquota=aquota.user");
}

bool PartitionManager::IsGroupQuotaEnabled()
{
	return IsMountOptionEnabled("grpquota") || IsMountOptionEnabled("gquota") || IsMountOptionEnabled("gqnoenforce") || IsMountOptionEnabled("usrjquota=aquota.group");
}

bool PartitionManager::IsProjectQuotaEnabled()
{
	return IsMountOptionEnabled("prjquota") || IsMountOptionEnabled("pquota") || IsMountOptionEnabled("pqnoenforce");
}

bool PartitionManager::IsMountOptionEnabled(const string & opt)
{	
	string DeviceName = (getenv("Partition") ? getenv("Partition") : "");
	string MountPoint = (getenv("MountAt") ? getenv("MountAt") : "");
	string FileSystem = (getenv("FileSystem") ? getenv("FileSystem") : "");
	
	string prefix = DeviceName + " " + MountPoint + " " + FileSystem + " ";
	
	vector<string> grep_args;
	grep_args.push_back(prefix);
	grep_args.push_back("/proc/mounts");
	
	UnixCommand grep("grep");	
	ProcessResult * res = grep.Execute(grep_args);	
	
	if ( res->GetStatus() != Success )
	{		
		delete res;
		return false;
	}	
	string output = res->GetOutput();
	delete res;
	
	output = output.substr(prefix.length(), output.length() - prefix.length());
	
	
	size_t end = output.find(' ');
	if ( end == string::npos )
	{
		Logger::LogWarn("Wrong formatted output.");
		return false;
	}
	
	output = output.substr(0, end);
	
	vector<string> opts = SplitString(output, ',', vector<string>());
	
	if ( find(opts.begin(), opts.end(), opt) == opts.end())
		return false;
		
	return true;
}

bool PartitionManager::IsMkfsOptionEnabled(const string & opt)
{
	string mkfs_opts = getenv("MkfsOpts");
	vector<string> mkfs_opts_s = SplitString(mkfs_opts, '@', vector<string>());
	int i = 0;
	int size = mkfs_opts_s.size();
	for( i = 0; i < size; ++i )
	{
		if(mkfs_opts_s[i].find(opt, 0) != string::npos)
			return true;
	}
	return false;
}

bool PartitionManager::LoadConfiguration()
{
	bool result = false;
	ifstream input;
	try
	{
		string line;
		input.open(_ConfigFile.c_str());
		//cout << "PartitionManager: Processing file " << _ConfigFile << endl;
		// Find the section start
		while ( input.good() )
		{
			input >> line;
								
			// There are no file systems with less than 2 chars long
			if ( line.size() < 8 || line.find('[') == string::npos )
				continue;
				
			// Process the section
			string FSName = line.substr(4, line.size() - 5);
			_FSIndex = GetFSNumber(FSName);
			if ( _FSIndex == -1 )
				continue;
			
			_AdditionalOptions[_FSIndex].insert(CurrentOptions());
			char buf[255];
			// Skip the current line
			input.getline(buf, 255);
			
			while ( true )
			{				
				input.getline(buf, 255);

				// Check if the section is over
				if ( !strcmp(buf, "") )
					break;
					
				CurrentOptions cur_opts;
				cur_opts.ParseString(buf);
			
				_AdditionalOptions[_FSIndex].insert(cur_opts);
			}
			
			//Copy the "common" options to all the FS options vectors.
			if ( _FSIndex == FS_COMMON )
			{
				for ( int i = FS_COMMON + 1; i < FS_UNSUPPORTED; ++i )
					for ( set<CurrentOptions>::iterator j = _AdditionalOptions[FS_COMMON].begin(); j != _AdditionalOptions[FS_COMMON].end(); ++j ) 
						_AdditionalOptions[i].insert(*j);
						
				_FSIndex = FS_UNSUPPORTED;
				_AdditionalOptions[FS_COMMON].erase(_AdditionalOptions[FS_COMMON].begin(), 
						_AdditionalOptions[FS_COMMON].end());
			}
		}
		result = true;				
	}
	catch (...)
	{
		Logger::LogError("PartitionManager: exception was thrown.");
		result = false;				
	}

	input.close();
	return result;
}

bool PartitionManager::CreateFilesystem(string fs, string partition, bool resizeFlag, string mkfs_opts)
{
	uint64_t DeviceSize = GetDeviceSize(partition);
	
	if ( DeviceSize == 0 )
		return false;
	
	MkfsCommand * mkfs = new MkfsCommand("mkfs." + fs);
	vector<string> args;
	
	// Block and partition size
	// Reserve empty space on device for later resize tests.
	stringstream s1;
	int BlockSize = 4096;
	string strBlockSize;			
	s1 << BlockSize;
	strBlockSize = s1.str();
	
	stringstream s2;
	string PartitionSize;
	if ( !resizeFlag )
	{
			s2 << ((DeviceSize / BlockSize) );
	}
	else
	{
		// If the partition is too small (less than 1GB, then take -100000 blocks
		// Else let's make it 1GB
		s2 << ( ( 1000000 < (DeviceSize / BlockSize) ) ? 1000000 : (DeviceSize / BlockSize) - 100000);
	}
	PartitionSize = s2.str();
	
	// If we need >= one mkfs;	
	vector<string> mkfs_opts_s = SplitString(mkfs_opts, '@', vector<string>());
	if(mkfs_opts_s.size() == 0)
	{
		// default mkfs options;
		if ( fs == "xfs" || fs == "jfs" ) //Force if necessary
			args.push_back("-f");
		if ( fs == "ext4" )
		{
			args.push_back("-F");
			args.push_back("-b");
			args.push_back(strBlockSize);
		}

		if ( fs == "btrfs" )
		{
			stringstream s3;
			s3 << (DeviceSize - 512*1000);
			string SizeInBlocks = s3.str();
			args.push_back("-b");
			args.push_back(SizeInBlocks);
		}

		args.push_back(partition);
		if ( fs == "jfs" || fs == "ext4")
		{	
			args.push_back(PartitionSize);
		}
				
		ProcessResult * res;
		
		stringstream str;
		
		str << "mkfs." << fs << " ";
		for(int i = 0; i < args.size(); i++)
		{
			str << args[i] << " ";
		}
		Logger::LogInfo(str.str());
		
		res = mkfs->Execute(args);
		
		if ( res->GetStatus() != Success )
		{
			Logger::LogError("Cannot create " + fs + " filesystem on device " + partition);
			Logger::LogError("Error: " + res->GetOutput());
			delete mkfs;
			return false;
		}		
	}
	else
		for(int i = 0; i < mkfs_opts_s.size(); i++)
		{		
			string Partition = partition;
			args.clear();
			vector<string> tmp = SplitString(mkfs_opts_s[i], ' ', vector<string>());
			// Overwrite default block size, if it is specified in mkfs_opts, for ext4 and xfs fs
			int blkSzInd = -1;
			if (fs == "ext4" || fs == "xfs")
			{
				for ( blkSzInd = 0; blkSzInd < tmp.size(); ++blkSzInd )
					if(tmp[blkSzInd] == "-b")
					{
						BlockSize = atoi(tmp[blkSzInd+1].c_str());
						strBlockSize = tmp[blkSzInd+1];
						s2.str("");
						if ( !resizeFlag )
						{
								s2 << ((DeviceSize / BlockSize) );
						}
						else
						{
							// If the partition is too small (less than 1GB, then take -100000 blocks
							// Else let's make it 1GB
							s2 << ( ( 1000000 < (DeviceSize / BlockSize) ) ? 1000000 : (DeviceSize / BlockSize) - 100000);
						}
						PartitionSize = s2.str();
						break;
					}
			}	
			if(fs == "jfs")
			{
				for(int i = 0; i < tmp.size(); i++)
				{
					if(tmp[i] == "-J" && tmp[i+1] == "journal_dev")
					{
						Partition = "";
						PartitionSize = "";
					}
					if(tmp[i] == "-J" && tmp[i+1].find("device=", 0) != string::npos)
					{
						PartitionSize = "";
					}	 
				}
			}

				
			if ( fs == "xfs" || fs == "jfs" ) //Force if necessary
				args.push_back("-f");
			if ( fs == "ext4" )
				args.push_back("-F");
			if ( fs == "ext4" )
			{	
				args.push_back("-b");
				args.push_back(strBlockSize);
			}
			if ( fs == "xfs" )
			{	
				
			}

			if ( fs == "btrfs" )
			{
				stringstream s3;
				s3 << (DeviceSize - 512*1000);
				string SizeInBlocks = s3.str();
				args.push_back("-b");
				args.push_back(SizeInBlocks);
			}
			
			if (fs == "ext4" || fs == "xfs")
				for ( int i = 0; i < tmp.size() && i != blkSzInd && i != blkSzInd + 1 ; ++i )
					args.push_back(tmp[i]);
			else
			{
				for ( int i = 0; i < tmp.size(); ++i )
					args.push_back(tmp[i]);
			}	
			if(Partition != "")
				args.push_back(Partition);
			if ( fs == "jfs" && PartitionSize != "")
			{	
				args.push_back(PartitionSize);
			}
			if( fs == "ext4" )
			{
				args.push_back(PartitionSize);
			}
			
			ProcessResult * res;
			
			res = mkfs->Execute(args);
			
			if ( res->GetStatus() != Success )
			{
				Logger::LogError("Cannot create " + fs + " filesystem on device " + Partition);
				Logger::LogError("Error: " + res->GetOutput());
				delete mkfs;
				return false;
			}		
		}

	setenv("MkfsOpts", mkfs_opts.c_str(), 1);
	Logger::LogInfo("Mkfs complete.");
	delete mkfs;
	return true;
}

FileSystems PartitionManager::GetFSNumber(string FSName)
{
	for ( int i = 0; i < FS_UNSUPPORTED; ++i )
		if ( FSName == FSNames[i] )
			return static_cast<FileSystems>(i);
	return FS_UNSUPPORTED;
}

uint64_t PartitionManager::GetDeviceSize(string partition)
{
	uint64_t DeviceSize;
	struct stat st;
	if ( stat(partition.c_str(), &st) == -1)
	{
		Logger::LogError("Cannot get file stats.");
		return 0;
	}
	if (S_ISREG(st.st_mode))
	{
		Logger::LogInfo("Loop device on regular file.");
		DeviceSize = st.st_size;
	}
	else
	{
		int fd = open(partition.c_str(), O_RDONLY);
		if (fd == -1)
		{
			Logger::LogError("Cannot open partition.");
			return 0;
		}
		
		if ( ioctl( fd, BLKGETSIZE64, &DeviceSize ) == -1)			
		{
			close(fd);
			Logger::LogError("Cannot get partition size.");
			return 0;
		}
		close(fd);
	}
	return DeviceSize;
}

bool PartitionManager::GetSuperBlock(void * sb_struct, int size)
{
	string fs;
	string dev;
	int fd;
	unsigned long offset;
	if( getenv("FileSystem") && getenv("Partition") )
	{
		fs = getenv("FileSystem");
		dev = getenv("Partition");
	}
	else
	{
		Logger::LogError("Cannot get filesystem type or device name.");
		return false;
	}
	offset = SBOffsets[fs];
	fd = open(dev.c_str(), O_RDONLY);
	if(fd == -1)
	{
		Logger::LogError("Cannot open device.");
		return false;
	}
	if( lseek(fd, offset, SEEK_SET) == -1 )
	{
		Logger::LogError("Cannot seek in device.");
		close(fd);
		return false;
	}
	if( read(fd, sb_struct, size) == -1 )
	{
		Logger::LogError("Cannot read superblock from the device.");
		close(fd);
		return false;
	}
	close(fd);
	return true;
		
}

bool PartitionManager::SetSuperBlock(void * sb_struct, int size)
{
	string fs;
	string dev;
	string mnt;
	int fd;
	unsigned long offset;
	if( getenv("FileSystem") && getenv("Partition") && getenv("MountAt"))
	{
		fs = getenv("FileSystem");
		dev = getenv("Partition");
		mnt = getenv("MountAt");
	}
	else
	{
		Logger::LogError("Cannot get filesystem type or device name or mountpoint.");
		return false;
	}
	//just in case
	ReleasePartition(mnt);

	
	offset = SBOffsets[fs];
	fd = open(dev.c_str(), O_RDWR);
	if(fd == -1)
	{
		Logger::LogError("Cannot open device.");
		return false;
	}
	if( lseek(fd, offset, SEEK_SET) == -1 )
	{
		Logger::LogError("Cannot seek in device.");
		close(fd);
		return false;
	}
	if( write(fd, sb_struct, size) == -1 )
	{
		Logger::LogError("Cannot write superblock to the device.");
		close(fd);
		return false;
	}
	close(fd);
	if(!Mount(dev, mnt, fs, ""))
		return false;
	return true;
		
}
