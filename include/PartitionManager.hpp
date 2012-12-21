//		PartitionManager.hpp
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

#ifndef PARTITIONMANAGER_HPP
#define PARTITIONMANAGER_HPP

#include <Logger.hpp>
#include <sstream>
#include <stdint.h>
#include <inttypes.h>
#include <sys/shm.h>
using namespace std;

enum FileSystems
{
	FS_COMMON,
	EXT4,
	XFS,
	BTRFS,
	JFS,
	FS_UNSUPPORTED
};

const string FSNames[] = {
	"common",
	"ext4",
	"xfs",
	"btrfs",
	"jfs",
	"Unsupported FS"
};

enum PartitionStatus
{
	PS_Success,
	PS_Done,
	PS_Skip,
	PS_Fatal
};

class PartitionManager
{
	public:
		PartitionManager():
			_ConfigFile(""),
			_DeviceName(""),
			_MountPoint(""),
			_FileSystem(""),
			_MountOpts(""),
			_CurrentMountOptions(""),
			_Index(0),
			_FSIndex(FS_UNSUPPORTED)
			{
				
			}		
		PartitionManager(string ConfigFile, string DeviceName, string MountPoint, string FileSystem, string MountOpts):
			_ConfigFile(ConfigFile),
			_DeviceName(DeviceName),
			_MountPoint(MountPoint),
			_FileSystem(FileSystem),
			_MountOpts(MountOpts),
			_CurrentMountOptions(""),
			_Index(0),
			_FSIndex(FS_UNSUPPORTED)
			{				
				Initialize(ConfigFile, DeviceName, MountPoint, FileSystem, MountOpts);
			}
		virtual ~PartitionManager() {}
		
		PartitionManager & operator = (const PartitionManager & pm)
		{			
			_ConfigFile = pm._ConfigFile;
			_DeviceName = pm._DeviceName;
			_MountPoint = pm._MountPoint;
			_FileSystem = pm._FileSystem;
			_MountOpts = pm._MountOpts;
			_CurrentMountOptions = pm._CurrentMountOptions;
			_Index = pm._Index;
			_FSIndex = pm._FSIndex;			
			return *this;
		}
		
		void Initialize(string ConfigFile, string DeviceName, string MountPoint, string FileSystem, string MountOpts)
		{			
			_ConfigFile = ConfigFile;
			_DeviceName = DeviceName;
			_MountPoint = MountPoint;
			_FileSystem = FileSystem;
			_MountOpts = MountOpts;
			_CurrentMountOptions = "";
			_Index = 0;
			_FSIndex = FS_UNSUPPORTED;
			if ( _MountOpts != "" && _MountOpts[_MountOpts.size() - 1] != ',' )
				_MountOpts += ',';
			LoadConfiguration();
			_FSIndex = GetFSNumber(_FileSystem);			
		}
		
		static uint64_t GetDeviceSize(string partition)
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

		PartitionStatus PreparePartition()
		{
			Logger::LogInfo((string)"Preparing partition " + _DeviceName);
			if ( !ReleasePartition(_MountPoint) )
			{
				return PS_Fatal;
			}
			
			if ( !CreateFilesystem(_FileSystem, _DeviceName,true) )
			{				
				_Index = 0;
				return PS_Done;
			}
			Logger::LogInfo((string)"File system " + _FileSystem + " is created successfully.");
			
			
			if ( (_MountOpts != "" || _Index > 0) && _Index < _AdditionalMountOptions[_FSIndex].size() )
			{
				_CurrentMountOptions = _MountOpts + _AdditionalMountOptions[_FSIndex][_Index];
				
				Logger::LogInfo((string)"Mounting with additional option: " + _CurrentMountOptions);
			}
			if ( _Index == _AdditionalMountOptions[_FSIndex].size() )
			{
				_Index = 0;
				return PS_Done;
			}
			_Index++;
			cout<<  "Mount options: " << _CurrentMountOptions << ". " << _Index << " of " << _AdditionalMountOptions[_FSIndex].size() << endl;
			if( !Mount(_DeviceName,_MountPoint,_FileSystem,_CurrentMountOptions) )
				return PS_Skip;
			
			// Change the owner to the 'nobody' user. Makes some tests (like Access) to cleanup correctly
			struct passwd * nobody = getpwnam("nobody");
			
			if ( nobody != NULL )
			{				
				if ( chown( _MountPoint.c_str(), nobody->pw_uid, nobody->pw_gid) == 0 )
					cerr << "Success" << endl;
			}
			return PS_Success;
		}
		
		// Try to re-create the file system and mount it with the precious mount options.		
		static PartitionStatus RestorePartition(string DeviceName, string MountPoint, string FileSystem, bool RecreateFilesystem = false, bool resizeFlag = false)
		{
			if( !getenv("MountOpts"))
				return PS_Fatal;
			if( !ReleasePartition(MountPoint))
				return PS_Skip;
			if( RecreateFilesystem && !CreateFilesystem(FileSystem, DeviceName,resizeFlag))
				return PS_Fatal;
			if( !Mount(DeviceName,MountPoint,FileSystem,getenv("MountOpts")) )
				return PS_Skip;
			return PS_Success;
		}
		
		
		static bool ReleasePartition(string MountPoint)
		{
			Logger::LogInfo((string)"Unmounting partition " + MountPoint);
			if ( chdir("/") == -1)				
				return false;
			int RetryCount = 0;
			
retry:			
			int res = umount(MountPoint.c_str());
			// Check if partition was successfully unmounted, or it was not mounted yet!
			if ( res == 0 ) 
			{
				Logger::LogInfo((string)"Device is unmounted successfully.");
				return true;
			}
			if ( res == -1 && errno == EINVAL )
			{
				Logger::LogWarn((string)"Cannot unmount. Device was not mounted!");
				return true;
			}
			if ( errno == EBUSY ) // Well, try again...
			{
				sleep(1);
				if ( RetryCount++ < 10 )
				{
					Logger::LogWarn((string)"Device was busy.. retrying... ");
					goto retry;
				}
			}
			cerr << "Cannot unmount partition " << MountPoint << ". " << strerror(errno) << endl;
			return false;
		}
		
		string GetCurrentMountOptions() const
		{
			return _CurrentMountOptions;
		}
		void ClearCurrentMountOptions()
		{
			_Index = 0;
			_CurrentMountOptions = "";
		}
		
		static bool IsOptionEnabled(string optionName)
		{
			if ( IsSpecialOption(optionName) )
				return IsSpecialOptionEnabled(optionName);
			return IsOptionEnabledInternal(optionName);
		}
	private:
		string _ConfigFile;
		string _DeviceName;
		string _MountPoint;
		string _FileSystem;
		string _MountOpts;
		string _CurrentMountOptions;
		unsigned int _Index;
		FileSystems _FSIndex;		
		vector<string> _AdditionalMountOptions[FS_UNSUPPORTED];
		// Some options need to be processed differentelly. (e.g. noexec, nodev)		
		static bool IsSpecialOption(const string & opt)
		{
			if ( opt == "noexec" || opt == "nodev" || opt == "nosuid" )
				return true;
			return false;
		}
		static bool IsSpecialOptionEnabled(const string & opt)
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
		static bool IsOptionEnabledInternal(const string & opt, int * position = 0)
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
		bool LoadConfiguration()
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
					//cout << "PartitionManager: FS = " << FSName << endl;
					// Fill in _AdditionalMountOptions[FSNum]
					// Set the quota mount option for normal case to be able to run all the quota-related tests
					//_AdditionalMountOptions[_FSIndex].push_back("quota");
					_AdditionalMountOptions[_FSIndex].push_back("");
					char buf[255];
					// Skip the current line
					input.getline(buf, 255);
					
					while ( true )
					{						
						input.getline(buf, 255);
						
						// Check if the section is over				
						if ( !strcmp(buf, "") )
							break;							
						
						_AdditionalMountOptions[_FSIndex].push_back(buf);
					}
					
					//Copy the "common" options to all the FS options vectors.
					if ( _FSIndex == FS_COMMON )
					{
						for ( int i = FS_COMMON + 1; i < FS_UNSUPPORTED; ++i )
							_AdditionalMountOptions[i].insert(_AdditionalMountOptions[i].begin(), _AdditionalMountOptions[FS_COMMON].begin(), 
								_AdditionalMountOptions[FS_COMMON].end());
						_FSIndex = FS_UNSUPPORTED;
						_AdditionalMountOptions[FS_COMMON].erase(_AdditionalMountOptions[FS_COMMON].begin(), 
								_AdditionalMountOptions[FS_COMMON].end());
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
		
		static bool CreateFilesystem(string fs, string partition, bool resizeFlag = false)
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
				s2 << ((DeviceSize / BlockSize) );
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
				s3 << (DeviceSize);
				string SizeInBlocks = s3.str();
				
				args.push_back("-b");
				args.push_back(SizeInBlocks);
			}
			
			ProcessResult * res;
			res = mkfs->Execute(args);
			delete mkfs;
			if ( res->GetStatus() != Success )
			{
				cerr << "Cannot create " << fs << " filesystem on device " << partition << endl;
				cerr << "Error: " << res->GetOutput() << endl;
				return false;
			}
			Logger::LogInfo("Mkfs complete.");
			return true;
		}

		static bool Mount(string DeviceName,string MountPoint,string FileSystem,string Options)
		{
			Logger::LogWarn("Mount: Options: " + Options);
			UnixCommand * mnt = new UnixCommand("mount");
			vector<string> mnt_args;
			mnt_args.push_back(DeviceName);
			mnt_args.push_back(MountPoint);
			mnt_args.push_back("-t");
			mnt_args.push_back(FileSystem);
			
			mnt_args.push_back("-o");
			mnt_args.push_back(Options);
			setenv("MountOpts", Options.c_str(), 1);
						
			ProcessResult * res = mnt->Execute(mnt_args);
			delete mnt;
			if ( res == NULL || res->GetStatus() != Success )
			{
				cerr << "Cannot mount " << DeviceName << " at folder " << MountPoint << endl;
				cerr << "Error: " << res->GetOutput() << endl;
				return false;
			}
			
			cout << "Device " << DeviceName << " was mounted on folder ";
			cout << MountPoint << "(opts=" << Options << ") "<< endl;
			
			// Now change current dir to the newly mounted partition folder
			if ( chdir(MountPoint.c_str()) != 0 )
			{
				cerr << "Cannot change current dir to " << MountPoint << endl;
				cerr << "Error: " << strerror(errno) << endl;
				return false;
			}
			//cout << "Changed dir" << endl;
			return true;
		}
		FileSystems GetFSNumber(string FSName)
		{
			for ( int i = 0; i < FS_UNSUPPORTED; ++i )
				if ( FSName == FSNames[i] )
					return static_cast<FileSystems>(i);
			return FS_UNSUPPORTED;
		}
};

#endif /* PARTITIONMANAGER_HPP */
