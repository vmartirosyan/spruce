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

PartitionStatus PartitionManager::PreparePartition()
{
	Logger::LogInfo((string)"Preparing partition " + _DeviceName);
	if ( !ReleasePartition(_MountPoint) )
	{
		return PS_Fatal;
	}
	
	
	/*if ( _Index < _AdditionalOptions[_FSIndex].size() )
	{
		_CurrentMountOptions = _MountOpts + _AdditionalMountOptions[_FSIndex][_Index].Mou;
		
		Logger::LogInfo((string)"Mounting with additional option: " + _CurrentMountOptions);
	}*/
	if ( _Index == _AdditionalOptions[_FSIndex].end() )
	{
		_Index = _AdditionalOptions[_FSIndex].begin();
		return PS_Done;
	}
	
	_CurrentOptions = *_Index;
	_Index++;
	
	Logger::LogInfo("Preparing partition: " + GetCurrentOptions(false));
	
	if ( !CreateFilesystem(_FileSystem, _DeviceName, false, _CurrentOptions.MkfsOptions) )
	{				
		_Index = _AdditionalOptions[_FSIndex].begin();
		return PS_Done;
	}
	Logger::LogInfo((string)"File system " + _FileSystem + " is created successfully.");	
	
	if( !Mount(_DeviceName,_MountPoint,_FileSystem, _CurrentOptions.MountData, _CurrentOptions.MountFlags) )
		return PS_Skip;
	
	// Change the owner to the 'nobody' user. Makes some tests (like Access) to cleanup correctly
	struct passwd * nobody = getpwnam("nobody");
	
	if ( nobody != NULL )
	{
		chown( _MountPoint.c_str(), nobody->pw_uid, nobody->pw_gid);			
	}

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
	
	UnixCommand unmount("umount");
	vector<string> unmount_args;
	unmount_args.push_back(MountPoint);
	
retry:		
	
	//res = unmount.Execute(unmount_args);
	int res1 = umount(MountPoint.c_str());
	errno = 0;
	//if ( res->GetStatus() != Success )
	if ( res1 != 0 && errno == EINVAL )
	{
		delete res;
		sleep(1);
		if ( RetryCount++ < 10 )
		{
			Logger::LogWarn((string)"Device was busy.. retrying... ");
			goto retry;
		}
		else
		{
			if(output != NULL)
				*output = string("Cannot unmount partition ") + MountPoint + string(". ") + strerror(errno);
			cerr << "Cannot unmount partition " << MountPoint << ". " << strerror(errno) << endl;
			return false;
		}				
	}
	
	Logger::LogInfo((string)"Device is unmounted successfully.");
	return true;
}

bool PartitionManager::Mount(string DeviceName,string MountPoint,string FileSystem,string Options, unsigned long Flags)
{			
	/*
	UnixCommand * mnt = new UnixCommand("mount");
	vector<string> mnt_args;
	mnt_args.push_back(DeviceName);
	mnt_args.push_back(MountPoint);
	mnt_args.push_back("-t");
	mnt_args.push_back(FileSystem);

	mnt_args.push_back("-n");
	
	mnt_args.push_back("-o");
	mnt_args.push_back(Options);
	
				
	ProcessResult * res = mnt->Execute(mnt_args);
	delete mnt;
	if ( res == NULL || res->GetStatus() != Success )
	{
		cerr << "Cannot mount " << DeviceName << " at folder " << MountPoint << endl;
		cerr << "Error: " << res->GetOutput() << endl;
		return false;
	}
	*/
	if ( Options[Options.length() - 1] == ',' )
		Options.erase(Options.begin() + Options.length() - 1);
	//cerr << "Mounting " << DeviceName << " to " << MountPoint << " (" << FileSystem << ") \nFlags: "
	//	<< Flags << ", Data: " << Options << endl;
	
	const int size = 10;
	char buf[size];
	if ( snprintf(buf, size, "%lu", Flags) >= size )
	{
		Logger::LogError("Cannot get Flags value.");
		return false;
	}
		
	setenv("MountFlags", buf, 1);
	setenv("MountData", Options.c_str(), 1);
	
	if ( mount(DeviceName.c_str(), MountPoint.c_str(), FileSystem.c_str(), Flags, Options.c_str()) == -1 )
	{
		Logger::LogError("Cannot mount device.");
		return false;
	}
	
	
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

	cerr << "Cannot unmount partition " << MountPoint << ". " << strerror(errno) << endl;
	return false;

}

string PartitionManager::GetCurrentOptions(bool Stripped) const
{	
	if ( !Stripped )
		return _CurrentOptions.RawData;	
	string tmp = _CurrentOptions.RawData;
	for ( string::iterator i = tmp.begin(); i != tmp.end(); ++i )
		if (*i == ' ')
			*i = '_';
	return tmp;
}
void PartitionManager::ClearCurrentOptions()
{
	_Index = _AdditionalOptions[_FSIndex].begin();
	//_CurrentMountOptions = "";
}

bool PartitionManager::IsOptionEnabled(string optionName)
{
	if ( IsFlag(optionName) )
	{
		return IsFlagEnabled(optionName);
	}
	if ( IsSpecialOption(optionName) )
		return IsSpecialOptionEnabled(optionName);
	return IsOptionEnabledInternal(optionName);
}

bool PartitionManager::IsFlag(string optionName)
{
	return ( MountFlagMap.find(optionName) != MountFlagMap.end() );
}

bool PartitionManager::IsFlagEnabled(string optionName)
{
	if ( getenv("MountFlags") == NULL )
		return false;

	char * buf = getenv("MountFlags");
	unsigned long flags = atoi(buf);
	
	unsigned long flag = MountFlagMap[optionName];
	
	return ( ( flags & flag ) != 0 );
}

// Some options need to be processed differentelly. (e.g. noexec, nodev)		
bool PartitionManager::IsSpecialOption(const string & opt)
{
	if ( opt == "noexec" || opt == "nodev" || opt == "nosuid" )
		return true;
	return false;
}
bool PartitionManager::IsSpecialOptionEnabled(const string & opt)
{
	// According to man 8 mount:
	// user, users: These  options  imply  the  options  noexec,
	//  nosuid,  and  nodev (unless overridden by subsequent options, as
	//  in the option line user,exec,dev,suid).
	if ( opt == "noexec" )
	{
		int userPos;
		int execPos;
		int noexecPos;
		// If the noexec option is there and (exec option is missing or comes before noexec)
		if ( IsOptionEnabledInternal("noexec", &noexecPos) && ( !IsOptionEnabledInternal("exec", &execPos) || execPos < noexecPos ) )
			return true;
		// If the user option is there and (exec option is missing or comes before user)
		if ( IsOptionEnabledInternal("user", &userPos) && ( !IsOptionEnabledInternal("exec", &execPos) || execPos < userPos) )
			return true;
		// If the users option is there and (exec option is missing or comes before users)
		if ( IsOptionEnabledInternal("users", &userPos) && ( !IsOptionEnabledInternal("exec", &execPos) || execPos < userPos) )
			return true;
			
		return false;
	}
	
	// According to man 8 mount:
	// user, users, group, owner: These  options  imply  the  options  
	//  nosuid,  and  nodev (unless overridden by subsequent options, as
	//  in the option line user,dev,suid).
	if ( opt == "nodev" )
	{
		int userPos;
		int groupPos;
		int devPos;
		int nodevPos;
		int ownerPos;
		// If the nodev option is there and (dev option is missing or comes before nodev)
		if ( IsOptionEnabledInternal("nodev", &nodevPos) && ( !IsOptionEnabledInternal("dev", &devPos) || devPos < nodevPos ) )
			return true;
		// If the user option is there and (dev option is missing or comes before user)
		if ( IsOptionEnabledInternal("user", &userPos) && ( !IsOptionEnabledInternal("dev", &devPos) || devPos < userPos) )
			return true;
		// If the users option is there and (dev option is missing or comes before users)
		if ( IsOptionEnabledInternal("users", &userPos) && ( !IsOptionEnabledInternal("dev", &devPos) || devPos < userPos) )
			return true;
		// If the group option is there and (dev option is missing or comes before group)
		if ( IsOptionEnabledInternal("group", &groupPos) && ( !IsOptionEnabledInternal("dev", &devPos) || devPos < groupPos) )
			return true;
		// If the owner option is there and (dev option is missing or comes before group)
		if ( IsOptionEnabledInternal("owner", &ownerPos) && ( !IsOptionEnabledInternal("dev", &devPos) || devPos < ownerPos) )
			return true;
			
		return false;
	}
	
	// According to man 8 mount:
	// user, users, group, owner: These  options  imply  the  options  
	//  nosuid,  and  nodev (unless overridden by subsequent options, as
	//  in the option line user,dev,suid).
	if ( opt == "nosuid" )
	{
		int userPos;
		int groupPos;
		int suidPos;
		int nosuidPos;
		int ownerPos;
		// If the nosuid option is there and (suid option is missing or comes before nosuid)
		if ( IsOptionEnabledInternal("nosuid", &nosuidPos) && ( !IsOptionEnabledInternal("suid", &suidPos) || suidPos < nosuidPos ) )
			return true;
		// If the user option is there and (suid option is missing or comes before user)
		if ( IsOptionEnabledInternal("user", &userPos) && ( !IsOptionEnabledInternal("suid", &suidPos) || suidPos < userPos) )
			return true;
		// If the users option is there and (suid option is missing or comes before users)
		if ( IsOptionEnabledInternal("users", &userPos) && ( !IsOptionEnabledInternal("suid", &suidPos) || suidPos < userPos) )
			return true;
		// If the group option is there and (suid option is missing or comes before group)
		if ( IsOptionEnabledInternal("group", &groupPos) && ( !IsOptionEnabledInternal("suid", &suidPos) || suidPos < groupPos) )
			return true;
		// If the owner option is there and (dev option is missing or comes before group)
		if ( IsOptionEnabledInternal("owner", &ownerPos) && ( !IsOptionEnabledInternal("suid", &suidPos) || suidPos < ownerPos) )
			return true;
			
		return false;
	}
	// In case of unknown option
	return false;
}
bool PartitionManager::IsOptionEnabledInternal(const string & opt, int * position)
{			
	char * opts = NULL;
	if ( (opts = getenv("MountOpts")) != NULL )
	{
		char * buf = new char[strlen(opts) + 1];
		strncpy( buf, opts, strlen(opts) );
		buf[strlen(opts)] = 0;				
		char * pch;
		pch = strtok (buf," ,");
		int pos = -1;
		while (pch != NULL)
		{
			pos++;
			if(strcmp(pch,opt.c_str()) == 0)
			{
				if ( position )
					*position = pos;						
				return true;
			}
			pch = strtok (NULL, " ,");					
		}
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
				cur_opts.RawData = buf;
				
				// Split the line and analyze it
				char * pos = NULL;
				if ( (pos = strstr(buf, ":") ) != 0 )
				{
					// There are some mkfs options... Collect them
					cur_opts.MkfsOptions = strndup(buf, (pos - buf));
					// Skip the colon.
					pos++;
				}
				else
				{
					pos = buf;
					cur_opts.MkfsOptions = "";
				}
				string mount_opts = strdup(pos);
				vector<string> opts = SplitString(mount_opts, ',', vector<string>());
				for ( unsigned int i = 0; i < opts.size(); ++i )
				{
					if ( MountFlagMap.find(opts[i]) != MountFlagMap.end() )
						cur_opts.MountFlags |= MountFlagMap[opts[i]];
					else
						cur_opts.MountData.append(opts[i]).append(",");
				}
			
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
		cerr << "PartitionManager: exception was thrown." << endl;
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
	
	UnixCommand * mkfs = new UnixCommand("mkfs." + fs);
	vector<string> args;		
	args.push_back(partition);		
	if ( fs == "xfs" || fs == "jfs" ) //Force if necessary
		args.push_back("-f");
	if ( fs == "ext4" )
		args.push_back("-F");
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
		if(fs != "ext4")
			s2 << ((DeviceSize / BlockSize) );
		else
			s2 << ((DeviceSize / BlockSize) - 100000);
	}
	else
	{
		s2 << ((DeviceSize / BlockSize) - 10000);
	}
	PartitionSize = s2.str();
	
	if ( fs == "ext4" )
	{	
		args.push_back("-b");
		args.push_back(strBlockSize);
		args.push_back(PartitionSize);
	}
	if ( fs == "xfs" )
	{	
		
	}
	if ( fs == "jfs" )
	{	
		args.push_back(PartitionSize);
	}
	if ( fs == "btrfs" )
	{
		stringstream s3;
		s3 << (DeviceSize - 512*1000);
		string SizeInBlocks = s3.str();
		
		args.push_back("-b");
		args.push_back(SizeInBlocks);
	}
	
	// Add the additional options
	vector<string> tmp = SplitString(mkfs_opts, ' ', vector<string>());
	for ( unsigned int i = 0; i < tmp.size(); ++i )
		args.push_back(tmp[i]);
	
	ProcessResult * res;
	res = mkfs->Execute(args);
	delete mkfs;
	if ( res->GetStatus() != Success )
	{
		cerr << "Cannot create " << fs << " filesystem on device " << partition << endl;
		cerr << "Error: " << res->GetOutput() << endl;
		return false;
	}
	setenv("MkfsOpts", mkfs_opts.c_str(), 1);
	Logger::LogInfo("Mkfs complete.");
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
		cerr << "Cannot get file stats. " << strerror(errno) << endl;
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
			cerr << "Cannot open partition " << strerror(errno) << endl;
			return 0;
		}
		
		if ( ioctl( fd, BLKGETSIZE64, &DeviceSize ) == -1)			
		{
			close(fd);
			cerr << "Cannot get partition size." << strerror(errno) << endl;
			return 0;
		}
		close(fd);
	}
	return DeviceSize;
}
