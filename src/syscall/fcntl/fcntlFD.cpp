//      fnctl.cpp
//
// 		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//
//      Author: Shahzadyan Khachik <qwerity@gmail.com>
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

#include <fcntlFD.hpp>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

int fcntlFD::Main(vector<string> args)
{
	if ( _mode == Normal )
	{
		switch (_operation)
		{
			case fcntlFDGetSetFileDescriptorFlags:
				return get_setFileDescriptorFlags();

			case fcntlFDGetSetFileStatusFlagsIgnore:
				return get_setFileStatusFlagsIgnore();

			case fcntlFDGetSetFileStatusFlagsIgnoreRDONLY:
				return get_setFileStatusFlagsIgnoreRDONLY();

			case fcntlFDGetSetFileStatusFlags:
				return get_setFileStatusFlags();

			case fcntlFDDupFileDescriptor:
				return dupFileDescriptor();
				
		   case fcntlFDGetLock:
		        return fcntlFDGetLockFunction();
		        
		   case  fcntlFDSetLockWithWait:
			 return	fcntlFDSetLockWithWaitFunction();
			 
		   case  fcntlFDSetLock:
				return fcntlFDSetLockFunction();
				
	       case fcntlFDNoteDir:
				return fcntlFDNoteDirFunction();
				
		   case fcntlFDNoteFile:
				return fcntlFDNoteFileFunction();

				
			default:
				cerr << "Unsupported operation.";
				return Unres;
		}
	}

	cerr << "Test was successful";

	return Success;
}

Status fcntlFD::dupFileDescriptor()
{
	int fd;
	int new_fd;
	const char buff[6] = "apero";
	char* nbuff = new char[6];
	long _arg = rand()%99;

	if((fd = open("test", O_CREAT | O_RDWR)) < 0)
		cerr << "Can't open file\n errno: " << strerror(errno) << endl;

	new_fd = fcntl(fd, F_DUPFD, _arg);

	write(fd, &buff, 5);
	lseek(fd, 0, SEEK_SET);
	read(new_fd, nbuff, 5);

	close(fd);
	close(new_fd);
	unlink("test");

	if(new_fd == EINVAL)
	{
		cerr << "arg is negative or is greater than the maximum allowable value." << endl;
		return Fail;
	}

	if(new_fd == EMFILE)
	{
		cerr << "the process already has the maximum number of file descriptors open" << endl;
		return Fail;
	}

	if((strcmp(buff, nbuff) != 0) || (new_fd < _arg))
	{
		delete [] nbuff;
		cerr << "Return wrong file descriptor";
		return Fail;
	}

	return Success;
}

Status fcntlFD::get_setFileStatusFlagsIgnore()
{
	int fd;
	long set_flags;
	long get_flags;

	if((fd = open("test", O_CREAT)) < 0)
		cerr << "Can't open file\n errno: " << strerror(errno) << endl;

	// set File Descriptor Flags
	set_flags = O_WRONLY | O_RDWR | O_CREAT | O_EXCL | O_NOCTTY | O_TRUNC;
	fcntl(fd, F_SETFL, set_flags);

	// get File Descriptor Flags
	get_flags = fcntl(fd, F_GETFL);

	close(fd);
	unlink("test");

	if(set_flags & get_flags != 0)
	{
		cerr << "Must Ignore setted flags, but does not!: " << strerror(errno);
		return Fail;
	}

	return Success;
}

Status fcntlFD::get_setFileStatusFlagsIgnoreRDONLY()
{
	int fd;
	long set_flags;
	long get_flags;

	if((fd = open("test", O_CREAT)) < 0)
		cerr << "Can't open file\n errno: " << strerror(errno) << endl;

	// set File Descriptor Flags
	set_flags = O_RDONLY;
	fcntl(fd, F_SETFL, set_flags);

	// get File Descriptor Flags
	get_flags = fcntl(fd, F_GETFL);

	close(fd);
	unlink("test");

	if(set_flags & get_flags != 0)
	{
		cerr << "Must Ignore setted flags, but does not!: " << strerror(errno);
		return Fail;
	}

	return Success;
}

Status fcntlFD::get_setFileStatusFlags()
{
	int fd;
	long set_flags;
	long get_flags;

	if((fd = open("test", O_CREAT)) < 0)
	{
		cerr << "Can't open file\n errno: " << strerror(errno) << endl;
	}

	// set File Descriptor Flags
	set_flags = O_APPEND | O_DIRECT | O_NOATIME | O_NONBLOCK;
	fcntl(fd, F_SETFL, set_flags);

	// get File Descriptor Flags
	get_flags = fcntl(fd, F_GETFL);

	close(fd);
	unlink("test");

	if(set_flags != get_flags)
	{
		cerr << "Can't get right file descriptor flags: " << strerror(errno);
		return Fail;
	}

	return Success;
}

Status fcntlFD::get_setFileDescriptorFlags()
{
	int fd;
	long set_flags = FD_CLOEXEC;
	long get_flags;

	if(fd = open("test", O_CREAT | O_WRONLY) < 0)
		cerr << "Can't open file\n errno: " << strerror(errno) << endl;

	// set File Descriptor Flags
	fcntl(fd, F_SETFD, set_flags);

	// get File Descriptor Flags
	get_flags = fcntl(fd, F_GETFD);

	close(fd);
	unlink("test");

	if(FD_CLOEXEC == get_flags)
	{
		return Success;
	}
	else
	{
		cerr << "Cannot get right file descriptor flags: " << strerror(errno);
		return Fail;
	}
}

     /* Advisory locking */

// cmd = F_GETLK 
Status fcntlFD:: fcntlFDGetLockFunction()
{
	
	const char *filename  = "fcntl_file.txt";
	int fd, ret_fcntl;
	struct flock flocks;
	if ((fd= open( filename, O_RDWR | O_CREAT, 0700 )) == -1 )
	{
		cerr << "Error: "<<strerror(errno);
		return Unres;
	}

	//setting flock structure
    
	flocks.l_whence = SEEK_SET;
	flocks.l_start = 0;
	flocks.l_len = 0;
	flocks.l_pid = getpid();
	flocks.l_type = F_RDLCK ;
   
    ret_fcntl = fcntl( fd, F_GETLK, &flocks );
   
   if ( ret_fcntl == -1 )
   {
	 cerr << "Error: "<<strerror(errno);

	 if ( unlink( filename ) == -1 )
	   cerr << "Error in unlinking "<<strerror(errno);
	   return Fail;
   }
  
    if( flocks.l_type != F_UNLCK && flocks.l_whence!=SEEK_SET&&
        flocks.l_start != 0 && flocks.l_len != 0 )
  {
	 cerr <<"incompatible locks prevent this lock\n"
	        "PID of the process holding this lock "<< flocks.l_pid ; 
	 if ( unlink( filename ) == -1 )
      cerr << "Error in unlinking file "<<strerror(errno);
	 return Fail;
  }
	return Success;
}

//cmd = F_SETLK
Status fcntlFD::fcntlFDSetLockFunction ()
{
	
	const char *filename  = "fcntl_file.txt";
	struct flock flocks;
	int fd, type, ret_fcntl;
	if ((fd= open( filename, O_RDWR | O_CREAT, 0700 )) == -1 )
	{
		cerr << "Error: "<<strerror(errno);
		return Fail;
	}

	//setting flock structure
    
	flocks.l_whence = SEEK_SET;
	flocks.l_start = 0;
	flocks.l_len = 0;
	flocks.l_pid = getpid();
	
   
   for ( type = 0; type < 2; ++type )
   {
    flocks.l_type = type ? F_RDLCK : F_WRLCK;
    ret_fcntl = fcntl( fd, F_SETLK, &flocks );
   
      if ( ret_fcntl == -1 )
     {
	   if ( errno == EACCES || EAGAIN )
	    cerr << "conflicting lock is held by another process" << strerror(errno);
	   if ( unlink ( filename ) == -1 )
	         cerr << "Error in unlinking file "<<strerror(errno);
	         
	   return Fail;
    }
   }
   
	flocks.l_type = F_UNLCK;
	if ( ret_fcntl == -1 )
	{
	  if (errno == EACCES || EAGAIN )
	    cerr << "conflicting lock is held by another process "<<strerror(errno);
	    if ( unlink ( filename ) == -1 )
	      cerr << "Error in unlinking file "<<strerror(errno);
	      return Fail;
	  }
	  
   if ( unlink( filename ) == -1 )
   {
	   cerr << "Error in unlinking file "<<strerror(errno);
	   return Fail;
   }
    
   return Success;
}

// cmd = F_SETLKW
Status fcntlFD::fcntlFDSetLockWithWaitFunction()
{
	const char *filename  = "fcntl_file.txt";
	struct flock flocks;
	int fd, type, ret_fcntl;
	if ((fd= open( filename, O_RDWR | O_CREAT, 0700 )) == -1 )
	{
		cerr << "Error: "<<strerror(errno);
		return Unres;
	}

	//setting flock structure
    
	flocks.l_whence = SEEK_SET;
	flocks.l_start = 0;
	flocks.l_len = 0;
	flocks.l_pid = getpid();
	
   
   for ( type = 0; type < 2; ++type )
   {
    flocks.l_type = type ? F_RDLCK : F_WRLCK;
    ret_fcntl = fcntl( fd, F_SETLK, &flocks );
   
   if ( ret_fcntl == -1 )
   {
	 if ( errno == EINTR )
	   cerr << "signal is caught while waiting, then the call is interrupted " << strerror(errno);
	 if ( unlink ( filename ) == -1 )
	   cerr << "Error in unlinking file "<<strerror(errno);
	   return Fail;
   }
  }
	flocks.l_type = F_UNLCK;
	if ( ret_fcntl == -1 )
	{
	  if (errno == EINTR )
	    cerr << "signal is caught while waiting, then the call is interrupted  "<<strerror(errno);
	    if ( unlink ( filename ) == -1 )
	      cerr << "Error in unlinking file "<<strerror(errno);
	      return Fail;
	  }
	  
   if ( unlink( filename ) == -1 )
   {
	   cerr << "Error in unlinking file "<<strerror(errno);
	   return Fail;
   }
   
   return Success;
}

       /*File and directory change notification*/
//cmd = F_NOTIFY

//directory change notification
Status fcntlFD::fcntlFDNoteDirFunction()
{
	  int  dirfd;
	 const char *  dirname = "fcntl_directory";


    //DN_CREATE
	if ( mkdir( dirname, 0700 ) == -1 )
	{
		cerr << "Error in making directory: "<<strerror(errno)<< endl;
		return Unres;
	}
	if ( (dirfd= open( dirname, O_DIRECTORY )) == -1 )
	{
		cerr << "Error in opening directory: "<<strerror(errno);
		return Unres;
	}

	if ( fcntl( dirfd, F_NOTIFY,  DN_CREATE ) == -1 )
	{
		cerr << "Error: "<<strerror(errno) << " but must return 0 "
		         "because directory has been created and opened "<<endl;
		return Fail;
	}
	
	//DN_ATTRIB
     if ( chmod( dirname , 0000 ) == -1 )
	{
		cerr<< "Error in chmod: "<< strerror(errno);
		return Unres;
	}
	if ( fcntl ( dirfd, F_NOTIFY, DN_ATTRIB ) == -1 )
	{
		cerr << "Error: "<<strerror(errno)<< " but must return 0 "
		        " because permission set has been changed by chmod system call ";
		return Fail;
	}
	
	//DN_DELETE
    if ( rmdir( dirname ) == -1 )
	{
		cerr << "Error in removing directory: "<<strerror(errno);
		return Unres;
	}
	
	if ( fcntl ( dirfd, F_NOTIFY, DN_DELETE ) == -1 )
	{
		cerr << "Error: "<<strerror(errno) << " but must return 0"
		        "because directory has been removed ";
		 return Fail;
	}
	
   	return  Success;

}

//file change notification
Status fcntlFD::fcntlFDNoteFileFunction()
{
	int fd, dirfd ;
	char w_buffer_[5]="1234";
	char r_buffer_[5];
	const char *dir = "fcntl_directory_";
	const char *newfile = "fcntl_directory_/fcntl_file_";
	const char *file = "fcntl_director/fcntl_newfile_";
	

	//DN_CREATE
	if ( mkdir ( dir, 0777 ) == -1 )
	{
		cerr << "Error in making directory: "<<strerror(errno);
		return Unres;
	}
	if ( (dirfd = open ( dir, O_DIRECTORY )) == -1 )
	{
		cerr << "Error in opening directory: "<<strerror(errno);
		return Unres;
	}
	if ( creat ( file, 0777 ) == -1 )
	{
		cerr << "Error in creating file: "<<strerror(errno);
		return Unres;
	}
	if ( (fd = open ( file, O_RDWR )) == -1 )
	{
		cerr << "Error in opening file: "<<strerror(errno);
		return Unres;
	}
	if ( fcntl ( dirfd, F_NOTIFY, DN_CREATE ) == -1 )
	{
		cerr << "Error: "<<strerror(errno)<<" but fcntl must return 0 "
		        "because directory and file in that directory have been created and opened";
		return Fail;
	}
	//DN_ATTRIB
	if ( chmod( file, 0700 ) == -1 )
	{
		cerr << "Error in chmod: "<<strerror(errno);
		return Unres;
	}
	
	if ( fcntl ( dirfd, F_NOTIFY, DN_ATTRIB ) == -1 )
	{
		cerr << "Error: "<<strerror(errno)<< " but fcntl must return 0 "
		        "because permissions of the file in directory "
		        "with descriptor eqaul to dirfd has been changed ";
		return Fail;
	}
	
	//DN_RENAME
	if ( rename ( file, newfile ) == -1 )
	{
		cerr << "Error in renaming: "<<strerror(errno);
		return Unres;
	}
	if ( fcntl ( dirfd, F_NOTIFY, DN_RENAME ) == -1 )
	{
		cerr << "Error: "<<strerror(errno)<<" but fcntl must return 0 "
		        "because the file in derectory with directory descriptor equal to dirfd "
		        " has been renamed ";
		return Fail;
	}
	
	//DN_MODIFY
	if ( write ( fd, w_buffer_, 5 ) == -1 )
	{
		cerr << "Error in writing in the file: "<<strerror(errno);
		return Unres;
	}
	if ( fcntl ( dirfd, F_NOTIFY, DN_MODIFY ) == -1 )
	{
		cerr << "Error: "<<strerror(errno)<< " but fcntl must return 0 "
		        "because the file in directory with directory descriptor "
		        "equal to dirfd has been modified by write system call ";
		return Fail;
	}
	
	//DN_ACCESS
	if ( read ( fd, r_buffer_, 5 ) == -1 )
	{
		cerr << "Error in reading: "<<strerror(errno);
		return Unres;
	}
	if ( fcntl ( dirfd, F_NOTIFY, DN_ACCESS ) == -1 )
	{
		cerr << "Error : "<<strerror(errno)<< " but fcntl must return 0"
		        "because file in directory with directory descriptor equal to dirfd "
		        "has been accessed by read system call ";
		return Fail;
	}

	//DN_DELETE
	if ( unlink( newfile ) == -1 )
	{
		cerr << "Error in nulinking file: "<<strerror(errno);
		return Unres;
	}
	if ( fcntl ( dirfd, F_NOTIFY, DN_DELETE ) == -1 )
	{
		cerr << "Error: "<<strerror(errno)<<" but fcntl must return 0 "
		        "because file in directory with directory descriptor equal to dirfd "
		        " has been unlinked ";
		 return Fail;
	}
	
	if ( rmdir( dir ) == -1 )
	{
		cerr << "Error in removing directory: "<<strerror(errno);
		return Fail;
	}

     return  Success;
}
