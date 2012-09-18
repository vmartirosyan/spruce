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

enum FileSystems
{
	FS_COMMON,
	EXT4,
	XFS,
	BTRFS,
	JFS,
	FS_UNSUPPORTED
};

const char * FSNames[] = {
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
	PS_Skip
};

class PartitionManager
{
	public:
		PartitionManager(string ConfigFile, string DeviceName, string MountPoint, string FileSystem, string MountOpts):
			_ConfigFile(ConfigFile),
			_DeviceName(DeviceName),
			_MountPoint(MountPoint),
			_FileSystem(FileSystem),
			_MountOpts(MountOpts),
			_Index(0),
			_FSIndex(FS_UNSUPPORTED)
			{
				if ( _MountOpts != "" && _MountOpts[_MountOpts.size() - 1] != ',' )
					_MountOpts += ',';
				LoadConfiguration();
				_FSIndex = GetFSNumber(_FileSystem);
			}

		PartitionStatus PreparePartition()
		{
			if ( !ReleasePartition() )
			{				
				return PS_Skip;
			}
			
			cout << "Preparing partition " + _DeviceName << endl;
			
			if ( _Index == _AdditionalMountOptions[_FSIndex].size() )
			{
				_Index = 0;
				return PS_Done;
			}
				
			if ( !CreateFilesystem(_FileSystem, _DeviceName) )
				return PS_Skip;
			cout << "File system is created successfully." << endl;
			
			UnixCommand * mnt = new UnixCommand("mount");
			vector<string> mnt_args;
			mnt_args.push_back(_DeviceName);
			mnt_args.push_back(_MountPoint);
			
			if ( _MountOpts != "" || _Index > 1)
			{
				mnt_args.push_back("-o");				
				mnt_args.push_back(_MountOpts + _AdditionalMountOptions[_FSIndex][_Index]);
				cout << "Mounting with additional option: " << _MountOpts << _AdditionalMountOptions[_FSIndex][_Index-1] << endl;
			}
			_Index++;
									
			ProcessResult * res = mnt->Execute(mnt_args);
			delete mnt;
			if ( res->GetStatus() != Success )
			{
				cerr << "Cannot mount " << _DeviceName << " at folder " << _MountPoint << endl;
				cerr << "Error: " << res->GetOutput() << endl;
				return PS_Skip;
			}
			
			cout << "Device " << _DeviceName << " was mounted on folder " << _MountPoint << endl;
			
			// Now change current dir to the newly mounted partition folder
			if ( chdir(_MountPoint.c_str()) != 0 )
			{
				cerr << "Cannot change current dir to " << _MountPoint << endl;
				cerr << "Error: " << strerror(errno) << endl;
				return PS_Skip;
			}
			cout << "Changed dir" << endl;
			return PS_Success;
		}
		bool ReleasePartition()
		{
			cout << "Unmounting partition " << _DeviceName << endl;
			if ( chdir("/") == -1)				
				return false;
			int res = umount(_MountPoint.c_str());
			// Check if partition was successfully unmounted, or it was not mounted yet!
			if ( res == 0 ) 
			{
				cout << "Device is unmounted successfully." << endl;
				return true;
			}
			if ( res == -1 && errno == EINVAL )
			{
				cout << "Cannot unmount. Device was not mounted!" << endl;
				return true;
			}
			cerr << "Cannot umnount partition " << _DeviceName << ". " << strerror(errno) << endl;
			return false;
		}
	private:
		string _ConfigFile;
		string _DeviceName;
		string _MountPoint;
		string _FileSystem;
		string _MountOpts;		
		unsigned int _Index;
		FileSystems _FSIndex;		
		vector<string> _AdditionalMountOptions[FS_UNSUPPORTED];
		bool LoadConfiguration()
		{
			bool result = false;
			ifstream input;
			try
			{
				string line;
				input.open(_ConfigFile.c_str());
				cout << "PartitionManager: Processing file " << _ConfigFile << endl;
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
					cout << "PartitionManager: FS = " << FSName << endl;
					// Fill in _AdditionalMountOptions[FSNum]
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
		bool CreateFilesystem(string fs, string partition)
		{
			// First get the block device size (in bytes)
			UnixCommand * blockdev = new UnixCommand("blockdev");
			vector<string> blkdev_args;
			blkdev_args.push_back("--getsize64");
			blkdev_args.push_back(partition);
			ProcessResult * blkdev_res = blockdev->Execute(blkdev_args);
			delete blockdev;
			if ( blkdev_res == NULL )
			{
				cerr << "Cannot get device size." << endl;
				return false;
			}
			if ( blkdev_res->GetStatus() != Success )
			{
				cerr << "Error getting device size: " << blkdev_res->GetOutput() << endl;
				return false;
			}
			
			int DeviceSize = atoi(blkdev_res->GetOutput().c_str());
			
			cout << "Device size: " << DeviceSize << endl;
			
			UnixCommand * mkfs = new UnixCommand("mkfs." + fs);
			vector<string> args;		
			args.push_back(partition);		
			if ( fs == "xfs" || fs == "jfs" ) //Force if necessary
				args.push_back("-f");
			if ( fs == "ext4" )
				args.push_back("-F");
			// Block and partition size
			// Reserve empty space on device for later resize tests.
			int BlockSize = 4096;
			char buf[10];
			sprintf(buf, "%d", DeviceSize / BlockSize - 10);
			char buf2[10];
			sprintf(buf2, "%d", BlockSize);
			if ( fs == "ext4" )
			{	
				args.push_back("-b");
				args.push_back(buf2);
				args.push_back(buf);
			}
			if ( fs == "xfs" )
			{	
				
			}
			if ( fs == "jfs" )
			{	
				args.push_back(buf);
			}
			if ( fs == "btrfs" )
			{
				char buf3[10];
				sprintf(buf3, "%d", DeviceSize - 10*4096);
				args.push_back("-b");
				args.push_back(buf3);
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
			cout << "Mkfs complete." << endl;
			return true;
		}


		FileSystems GetFSNumber(string FSName)
		{
			for ( int i = 0; i < FS_UNSUPPORTED; ++i )
				if ( FSName == FSNames[i] )
					return (FileSystems)i;
			return FS_UNSUPPORTED;
		}
};

