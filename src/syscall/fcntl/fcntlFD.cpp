//      fnctlFD.cpp
//
// 		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//
//      Authors: Shahzadyan Khachik <qwerity@gmail.com>
//               Ani Tumanyan <ani.tumanyan92@gmail.com>  
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
#include <sys/wait.h>
#include <stdio.h>
#include <sys/utsname.h>
#include <linux/version.h>

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

		  case fcntlFDBadFileDescriptor1:
				return fcntlFDBadFileDescriptor1Func();
		
		  case fcntlFDBadFileDescriptor2:
				return fcntlFDBadFileDescriptor2Func();
				
			case fcntlFDTooManyOpenedFiles:
					return fcntlFDTooManyOpenedFilesFunc();
					
			case fcntlFDGetSetLease:
					return fcntlFDGetSetLeaseFunc();
					
			case fcntlFDInvalidArg1: 
					return fcntlFDInvalidArg1Func();
					
			case fcntlFDInvalidArg2:
				return fcntlFDInvalidArg2Func();
				
			case fcntlFDInvalidArg3: 
					return fcntlFDInvalidArg3Func();
					
	        case 	fcntlFDGetSetOwn: 
					return 	fcntlFDGetSetOwnFunc();
					
			case  fcntlFDBadAdress: 
					return fcntlFDBadAdressFunc(); 
					
			case fcntlFDResTempUnavailable:
					return fcntlFDResTempUnavailableFunc();
					
			case fcntlFDGetSetSig: 
					return fcntlFDGetSetSigFunc();
						
			case fcntlFDNoLock:
					return fcntlFDNoLockFunction();
							
			case fcntlFDGetSetOwn_Ex:
						return fcntlFDGetSetOwn_ExFunc();
			case fcntlFDGetSetPipeSize: 
					   return fcntlFDGetSetPipeSizeFunc();
					
			case fcntlFDCapAboveLimit:
						return fcntlFDCapAboveLimitFunc();
						
			case fcntlFDDupWithClExFlag:
						return fcntlFDDupWithClExFlagFunc();
						
			case fcntlFDIntrSysCall: 
	                   return fcntlFDIntrSysCallFunc();    
	
      			
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
	{	
		cerr << "Can't open file\n errno: " << strerror(errno) << endl;
	    return Unres;
    }

	if( (new_fd = fcntl(fd, F_DUPFD, _arg)) == -1 )
	{
		cerr << "Error in fcntl: "<<strerror(errno);
		return Fail;
	}

	if ( write(fd, &buff, 5) == -1 )
	{
		cerr << "Error in writing: "<<strerror(errno);
		return Unres;
	}
	
	if ( lseek(fd, 0, SEEK_SET) == -1 )
	{
		cerr << "Error: "<<strerror(errno);
		return Unres;
	}
	if( read(new_fd, nbuff, 5) == -1 )
	{
		cerr << "Error in writing: "<<strerror(errno);
		return Unres;
	}

	if ( close(fd) == -1 )
	{
		cerr << "Error in closing: "<<strerror(errno);
		return Unres;
	}
	if ( close(new_fd) == -1 )
	{
		cerr << "Error in closing file: "<<strerror(errno);
		return Unres;
	}
	
	if ( unlink("test") == -1 )
	{
		cerr << "Error in unlinking: "<<strerror(errno);
		return Unres;
	}

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
	{
			cerr << "Can't open file\n errno: " << strerror(errno) << endl;
			return Unres;
	}

	// set File Descriptor Flags
	set_flags = O_WRONLY | O_RDWR | O_CREAT | O_EXCL | O_NOCTTY | O_TRUNC;
	if ( fcntl(fd, F_SETFL, set_flags) == -1 )
	{
		cerr << "Error in fcntl: "<<strerror(errno);
		return Fail;
	}

	// get File Descriptor Flags
	if ( (get_flags = fcntl(fd, F_GETFL)) == -1 )
	{
		cerr << "Error in fcntl: "<<strerror(errno);
		return Fail;
	}

 	if ( close(fd) == -1 )
 	{
		cerr << "Error in closing file: "<<strerror(errno);
		return Unres;
	}
	if ( unlink("test") == -1 )
	{
		cerr << "Error in unlinking: "<<strerror(errno);
		return Unres;
	}

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
		{
			cerr << "Can't open file\n errno: " << strerror(errno) << endl;
			return Unres;
		}

	// set File Descriptor Flags
	set_flags = O_RDONLY;
	if( fcntl(fd, F_SETFL, set_flags) == -1 )
	{
		cerr << "Error in fcntl: "<<strerror(errno);
		return Fail;
	}

	// get File Descriptor Flags
	if( (get_flags = fcntl(fd, F_GETFL)) == -1 )
	{
		cerr << "Error in fcntl: "<<strerror(errno);
		return Fail;
	}

	if ( close(fd) == -1 )
	{
		cerr << "Error in closing: "<<strerror(errno);
		return Unres;
	}
	if( unlink("test") == -1 )
	{
		cerr << "Error in unlinking: "<<strerror(errno);
		return Unres;
	}

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
		return Unres;
	}

	// set File Descriptor Flags
	set_flags = O_APPEND | O_DIRECT | O_NOATIME | O_NONBLOCK;
	if ( fcntl(fd, F_SETFL, set_flags) == -1 )
	{
		cerr << "Error in fcntl: "<<strerror(errno);
		return Fail;
	}

	// get File Descriptor Flags
	if( (get_flags = fcntl(fd, F_GETFL)) == -1 )
	{
		cerr << "Error in fcntl: "<<strerror(errno);
		return Fail;
	}

	if ( close(fd) == -1 )
	{
		cerr << "Error in closing: "<<strerror(errno);
		return Unres;
	}
	if ( unlink("test") == -1 )
    {
		cerr << "Error in unlinking: "<<strerror(errno);
		return Unres;
    }
    
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

	if((fd = open("test", O_CREAT | O_WRONLY)) < 0)
	{
		cerr << "Can't open file\n errno: " << strerror(errno) << endl;
		return Unres;
	}

	// set File Descriptor Flags
	if ( fcntl(fd, F_SETFD, set_flags) == -1 )
	{
		cerr << "Error in fcntl: "<<strerror(errno);
		return Fail;
	}

	// get File Descriptor Flags
	if ( (get_flags = fcntl(fd, F_GETFD)) == -1 )
	{
		cerr <<"Error in fcntl: "<<strerror(errno);
		return Fail;
	}

	if ( close(fd) == -1 )
	{
		cerr << "Error in closing: "<<strerror(errno);
		return Unres;
		
	}
	if ( unlink("test") == -1 )
	{
		cerr << "Error in unlinking: "<<strerror(errno);
		return Unres; 
	}

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
	   cerr << "Error in unlinking: "<<strerror(errno);
	   return Fail;
   }
  
    if( flocks.l_type != F_UNLCK && flocks.l_whence!=SEEK_SET&&
        flocks.l_start != 0 && flocks.l_len != 0 )
  {
	 cerr <<"incompatible locks prevent this lock\n"
	        "PID of the process holding this lock "<< flocks.l_pid ; 
	 if ( unlink( filename ) == -1 )
      cerr << "Error in unlinking file: "<<strerror(errno);
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
		cerr << "Error in opening: "<<strerror(errno);
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
	         cerr << "Error in unlinking file: "<<strerror(errno);
	         
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
	   cerr << "Error in unlinking file: "<<strerror(errno);
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
	if ( mkdir ( dir, 0700 ) == -1 )
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

//EBADF
//case 1:file descriptor open mode doesn't match with the type of lock requested
Status fcntlFD::fcntlFDBadFileDescriptor1Func()
{
	
	const char *file = "some_file_1";
	struct flock flocks;
	int fd;
	
    if ( creat ( file, 0777 ) == -1 )
	{
		cerr << "Error in creating file: "<<strerror(errno);
		return Unres;
	}
	if ( (fd =open( file, O_RDONLY )) == -1 ) // file opened only for reading
	{
		cerr << "Error in opening file: "<<strerror(errno);
		return Unres;
	}
	//setting flock structure
	flocks.l_whence = SEEK_SET;
	flocks.l_start = 0;
	flocks.l_len = 0;
	flocks.l_pid = getpid();
	flocks.l_type = F_WRLCK; // write lock 
	
	if ( fcntl ( fd, F_SETLK, &flocks ) != -1 )
	{
		cerr << "returns 0 in case of bad file descriptor ";
		return Fail;
	}
	if ( errno != EBADF )
	{
		cerr << "Incorrect error set in errno in case of bad file descriptor "<<strerror(errno);
		return Fail;
	}
	if ( unlink ( file ) == -1 )
	{
		cerr << "Error in unlinking file: "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}

//case 2: fd is not opened file descriptor
Status fcntlFD::fcntlFDBadFileDescriptor2Func()
{	
	const char *file = "some_file_1";
	int fd;
	
    if ( creat ( file, 0777 ) == -1 )
	{
		cerr << "Error in creating file: "<<strerror(errno);
		return Unres;
	}
	//open and close file to make file descriptor invalid
	if ( (fd =open( file, O_RDONLY )) == -1 )
	{
		cerr << "Error in opening file: "<<strerror(errno);
		return Unres;
	}
	if ( close ( fd ) == -1 )
	{
		cerr << "Error in closing file: "<< strerror(errno);
		return Unres;
	}
	
	if ( fcntl ( fd, F_SETFD, FD_CLOEXEC ) != -1 )
	{
		cerr << "returns 0 in case of bad file descriptor ";
		return Fail;
	}
	if ( errno != EBADF )
	{
		cerr << "Incorrect error set in errno in case of bad file descriptor "<<strerror(errno);
		return Fail;
	}
	if ( unlink ( file ) == -1 )
	{
		cerr << "Error in unlinking file: "<<strerror(errno);
		return Fail;
	}
	return Success;
}

Status fcntlFD:: fcntlFDTooManyOpenedFilesFunc()
{
	int ind, fd;
	pid_t pid;
	const char* filename = "smthfile";
	
	pid = fork();
	
	if ( pid < 0 )
	{
		cerr << "Error in fork: "<<strerror(errno);
		return Unres;
	}
	
	if ( !pid  )
	{
	  //child process
	   int max_files = getdtablesize();

	   for ( ind = 0; ind < max_files; ++ind )
	  {
		 if ( creat( filename , 0777 ) == -1 )
		 {
			 cerr << "Error in creating file: "<<strerror(errno);
			 return Unres;
		 }
		 if ( (fd = open (filename,  O_RDONLY)) == -1 )
		 {
		  cerr << "Error in opening: "<<strerror(errno);
		   return Unres;
	     }
	  }
	  if ( fcntl( 1, F_DUPFD, 1 ) != -1 )
	  {
		 cerr << "returns 0 in case of too many opened files "<<strerror(errno);
		 return Fail;
	  }
     if ( errno != EMFILE )
     {
		cerr << "Incorrect error set in errno in case of too many opened files"
		     << strerror(errno);
		return Fail;
	  }
    }
    if ( unlink ( filename ) == -1 )
    {
		cerr << "Error in unlinking: "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}
Status fcntlFD::fcntlFDGetSetLeaseFunc()
{
	 int fd1, fd2;
	 const char *filename1 = "file1.txt";
     const char *filename2 = "file2.txt";
  
     	
     //read lease ( F_SETLEASE & F_RDLCK )
	  if ( (fd1 = open(filename1, O_RDONLY | O_CREAT, 0777)) == -1 )
  {
	  cerr << "Error in opening file: "<<strerror(errno);
	  return Unres;
  }   
	if ( fcntl( fd1, F_SETLEASE, F_RDLCK ) == -1 )
	{
		cerr << "Error in fcntl: "<<strerror(errno);
	   if ( unlink ( filename1 ) == -1 )
			cerr << "Error in unlinking: "<<strerror(errno);
		return Fail;
	}
	
	if ( fcntl( fd1, F_GETLEASE ) != F_RDLCK )
	{
		cerr << "get-set lease failed for read lease" ;
		if ( unlink ( filename1 ) == -1 )
		cerr << "Error in  unlinking: "<<strerror(errno);
		return  Fail;
	}
	
	if ( unlink ( filename1 ) == -1 )
	{
		cerr << "Error in unlinking: "<<strerror(errno);
		return Fail;
	}
	
	// write lease ( F_SETLEASE & F_WRLCK )
	 if ( (fd2 = open(filename2, O_RDWR | O_CREAT, 0777)) == -1 )
	{
		cerr << "Error in opening: "<< strerror(errno);
		return Unres;
		
	}
	if ( fcntl ( fd2, F_SETLEASE, F_WRLCK ) == -1 )
	{
		cerr << "Error in fcntl: "<<strerror(errno);
		if ( unlink (filename2) == -1 )
			cerr << "Error in unlinking: "<<strerror(errno);
		 return Fail;
	}
	if ( fcntl( fd2, F_GETLEASE ) != F_WRLCK )
	{
		cerr << "get-set lease failed for write lease ";
		if ( unlink(filename2) == -1 )
			cerr << "Error in unlinking: "<<strerror(errno);
		return Fail;
	}
	
	// removing lease ( F_SETLEASE & F_UNLCK )
	if ( fcntl ( fd2, F_SETLEASE , F_UNLCK ) == -1 )
	{
		cerr << "Error in fcntl: "<<strerror(errno);
		if ( unlink (filename2) == -1 )
		  cerr << "Error in unlinking: "<<strerror(errno);
		return Fail;
	}
	if ( fcntl ( fd2, F_GETLEASE ) != F_UNLCK )
	{
		cerr << "get-set lease failed for remove lease ";
		if ( unlink (filename2) == -1 )
			cerr << "Error in unlinking: "<<strerror(errno);
        return Fail;		
	}
	if ( unlink (filename2) == -1 )
	{
		cerr << "Error in unlinking: "<<strerror(errno);
		return Fail;
	} 
	return  Success;
}

//EINVAL
//case 1: setting negative argument for F_DUPFD operation
Status fcntlFD:: fcntlFDInvalidArg1Func()
{
	const char *file = "somefile";
	int fd; 
	if ( (fd= open ( file, O_RDWR | O_CREAT, 0777)) == -1 )
	{
		cerr << "Error in opening and creating: "<<strerror(errno);
		return Unres;
	}
	//setting negative value to arg_
	if ( fcntl ( fd, F_DUPFD, -1 ) != -1 )
	{
		cerr << "returns 0 in case of invalid argument ";
		return Fail;
	}
	if ( errno != EINVAL )
	{
		cerr << "Incorrect error set in errno in case of invalid argument "<<strerror(errno);
		return Fail;
	}
	 if ( unlink ( file ) == -1 )
	 {
		 cerr << "Error in unlinking: "<<strerror(errno);
		 return Fail;
	 }
	return Success;
}

//case 2: setting unallowable  argument for F_SETSIG operation
Status fcntlFD :: fcntlFDInvalidArg2Func()
{
	int fd;
	const char * file = "somefile_fcntl";
	if ( (fd= open ( file, O_RDWR | O_CREAT , 0777 )) == -1 )
	{
		cerr << "Error in opening and creating file: "<<strerror(errno);
		return Unres;
	} 
	//setting negative signal number to arg_
	if ( fcntl ( fd, F_SETSIG, -1 ) != -1 )
	{
		cerr << "returns 0 in case of invalid argument as signal ";
		return Fail;
	}
	if ( errno != EINVAL )
	{
		cerr << "Incorrect error set in errno in case of"
		        "invalid argument "<<strerror(errno);
        		        
		return Fail;       
	}
	if ( unlink ( file ) == -1 )
	{
		cerr << "Error in unlinking file: "<<strerror(errno);
		return Fail;
	}
	return Success; 
}

// case 3: setting negative argument to l_whence field of flock structure
// in  locking operations
Status fcntlFD :: fcntlFDInvalidArg3Func()
{
   struct flock flocks;
   int fd;
   const char *file = "somefile_name";
   if ( (fd= open( file, O_RDWR | O_CREAT, 0777)) == -1 )
   {
	   cerr << "Error in opening file and creating: "<<strerror(errno);
	   return Fail;
   }
   //setting flock structure
   	flocks.l_whence = -1;
	flocks.l_start = 0;
	flocks.l_len = 0;
	flocks.l_pid = getpid();
	flocks.l_type = F_WRLCK ;

   if ( fcntl ( fd, F_SETLK, &flocks) != -1 )
   {
	   cerr << "returns 0  in case of invalid argument";
	   return Fail;
   }
   if ( errno != EINVAL )
   {
	   cerr << "Incorrect error set in errno in case of invalid argument "<<strerror(errno);
	   return Fail;
   }
     
	return  Success;
} 

Status fcntlFD:: fcntlFDGetSetOwnFunc()
{
	int fd;
	const char *file = "smthfile_fcntl";
	if ( (fd= open( file,O_RDWR | O_CREAT, 0700 )) == -1 )
	{
		cerr << "Error in opening and creating: "<<strerror(errno);
		return Unres;
	}
	if ( fcntl ( fd, F_SETOWN, getpid() ) == -1 )
	{
		cerr << "Error in fcntl: "<<strerror(errno);
		return Fail;
	}
	if ( fcntl( fd, F_GETOWN ) != getpid() )
	{
		cerr << "get-set owner failed ";
		return Fail;
	}
	if ( unlink ( file ) == -1 )
	{
		cerr << "Error in unlinking: "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}

//EFAULT
Status fcntlFD:: fcntlFDBadAdressFunc()
{
	int fd;
	const char *filename ="filename";
	
	if ( (fd = open( filename,O_RDWR | O_CREAT, 0700 )) == -1 )
	{
		cerr << "Error in opening: "<<strerror(errno);
		return Unres;
	}
	if ( fcntl ( fd, F_GETLK, (struct flock *)-1 ) != -1 )
	{
		cerr << "return 0 in case of bad adress ";
		return Fail;
	}
	if ( errno != EFAULT )
	{
		cerr << "Incorrect error set in errno in case of bad adress "<<strerror(errno);
		return Fail;
	}
	if ( unlink ( filename ) == -1 )
	{
		cerr << "Error in unlinking: "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}

//EAGAIN
Status fcntlFD :: fcntlFDResTempUnavailableFunc()
{
	pid_t pid;
	struct flock flocks;
	const char *filename = "filename_fcntl";
	int fd;
	
	if ( (fd=open( filename, O_RDWR | O_CREAT, 0700 ))  == -1 )
	{
		cerr << "Error in opening and creating file: "<<strerror(errno);
		return Unres;
	}
	
	//setting flock structure
	flocks.l_whence = SEEK_SET;
	flocks.l_start = 0;
	flocks.l_len = 0;
	flocks.l_pid = getpid();
	flocks.l_type = F_WRLCK ;
	
	//setting write lock
	if ( fcntl ( fd, F_SETLK, &flocks) == -1 )
	{
		cerr << "Error in fcntl: "<<strerror(errno);
		return Fail;
	}
	
	pid = fork();
	
	if ( pid < 0 )
	{
		cerr << "Error in fork: "<<strerror(errno);
		return Unres;
	}
	else
	{
	
	 if ( pid == 0 )  //child process
	 {
		if ( fcntl ( fd, F_SETLK , &flocks) != -1 )
		{
			cerr << "returns 0 in case of resource temporalily unavailable";
			return Fail;
		}
		if ( errno != EAGAIN )
		{
			cerr << "Incorrect error set in errno in case of"
			        " resource temporalily  unavailable "<<strerror(errno);
			 return Fail; 
		}
		
	 }
	 else
	 {
		 //parent process waits for child process termination
		 wait(0);
		 //cleaning up
		 if ( unlink ( filename ) == -1 )
		 {
			 cerr << "Error in unlinking: "<<strerror(errno);
			 return Fail;
	     }
	 }
    }
    return Success;
}

//ENOLCK

Status fcntlFD :: fcntlFDNoLockFunction()
{
	const char *filename = "somefile";
	int fd;
	int ind;
	struct flock flocks[1000000];
	if ( (fd=open(filename, O_CREAT | O_RDWR , 0700)) == -1 )
	{
		cerr << "Error in opening and creating file: "<<strerror(errno);
		return Unres;
	}
	//setting flock structures
	 for ( ind = 0; ind < 1000000; ++ind )
	 {
     flocks[ind].l_whence = SEEK_SET;
	 flocks[ind].l_start = 0;
	 flocks[ind].l_len = ind;
	 flocks[ind].l_pid = getpid();
	 flocks[ind].l_type = F_WRLCK ;
     }
     
     for ( ind = 0 ; ind < 1000000; ++ind )
      if ( fcntl( fd, F_SETLK, &flocks[ind] ) == -1 )
	    {
		  if ( errno != ENOLCK )
	     {
			cerr << "Incorrect error set in errno in case of "
			        "no locks available "<<strerror(errno);
			return Fail;
		 }
		break;
	    }
	
	   if ( ind == 1000000 )
	   { 	
	    cerr << "returns 0 in case of no locks availble ";
	    return Fail;
       }
	 
	 if ( unlink ( filename ) == -1 )
	 {
		 cerr << "Error in unlinking file: "<<strerror(errno);
		 return Fail;
	 }
     return Success;
}

Status fcntlFD :: fcntlFDGetSetSigFunc()
{
	const char *filename = "somefile1";
	int fd; 
	
	if ( (fd = open( filename, O_CREAT | O_RDWR , 0777 )) == -1 )
	{
		cerr << "Error in opening and creating: "<<strerror(errno);
		return Unres;
	}
	if ( fcntl( fd, F_SETSIG, SIGIO ) == -1 )
	{
		cerr << "Error in fcntl: "<<strerror(errno);
		return Fail;
	}
	if ( fcntl( fd, F_GETSIG ) != SIGIO )
	{
		cerr<< "get-set signal failed ";
		return Fail;
	}
	if ( unlink ( filename ) == -1 )
	{
		cerr << "Error in unlinking: "<<strerror(errno);
		return Fail;
	}
	return Success;
}

Status fcntlFD :: fcntlFDGetSetOwn_ExFunc()
{
	
	const char *filename = "somefilename";
	int fd; 
	struct f_owner_ex owner;
	struct f_owner_ex owner1; 
	if ( (fd = open( filename, O_CREAT | O_RDWR , 0777 )) == -1 )
	{
		cerr << "Error in creating and opening file: "<<strerror(errno);
		return Unres;
	}
	//setting f_owner_ex structure
	owner.type =  F_OWNER_PID;
	owner.pid = getpid();
	if ( fcntl( fd, F_SETOWN_EX, &owner ) == -1 )
	{
		cerr << "Error in fcntl: "<<strerror(errno);
		return Fail;
	}

	if ( fcntl( fd, F_GETOWN_EX, &owner1 ) == -1 )
	{
		cerr << "Error in fcntl: "<<strerror(errno);
		return Fail;
	}
	if ( owner1.type != F_OWNER_PID || owner1.pid != getpid() )
	{
		cerr << "get-set owner_ex failed";  
		return Fail;
	}
 
    if ( unlink ( filename ) == -1 )
    {
		cerr << "Error in unlinking: "<<strerror(errno);
		return Fail;
	}	
	     return Success;  
}

Status fcntlFD::fcntlFDGetSetPipeSizeFunc()
{
	

	#if  LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35)
		cerr <<"Kernel version doesn't support this operation ";
		return Unres;
	
	#else
	
	int pipefd[2];

	if ( pipe ( pipefd ) == -1 )
	{
		cerr << "Error in creating pipe: "<<strerror(errno);
		return Unres;
	}
	//setting pipe's capability to be at least arg_ bytes 
	if ( fcntl( pipefd[1], F_SETPIPE_SZ, 100) == -1 )
	{
		cerr << "Error in fcntl: "<<strerror(errno);
		return Fail;
	}
	// checking whether capability of the pipe is at least equal to arg_
	if ( fcntl ( pipefd[1], F_GETPIPE_SZ ) < 100 )
	{
		cerr << "get-set pipe size failed";
		return Fail;
	}
  return Success;
  #endif
}

//EPERM
//attemp to set capability to the pipe above limit
Status fcntlFD :: fcntlFDCapAboveLimitFunc ()
{

	
	
	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35)
	
		cerr <<"Kernel version doesn't support this operation ";
		return Unres; 
	
	#else
	int pipefd[2];
	if (pipe( pipefd ) == -1 )
	{
		cerr << "Error in creatong file: "<<strerror(errno);
		return Unres;
	}
	if ( fcntl ( pipefd[1], F_SETPIPE_SZ, 1048580 ) != -1 )
	{
		cerr << "returns 0 in case of permission denied because of attemp to set "
		         "capability above limit";
		return Fail;
	}
	if ( errno != EPERM )
	{
		cerr << "Incorrect error set in errno in case of permission denied "<<strerror(errno);
		return Fail;
	}
   
   return Success;
   #endif 
}

//test for F_DUPFD_CLOEXEC operation
Status fcntlFD :: fcntlFDDupWithClExFlagFunc()
{
	int flags_, fd1, fd2;
	const char *filename = "somefilename_";
	if ( (fd1 = open( filename, O_CREAT | O_RDWR , 0777 )) == -1 )
	{
		cerr << "Error in creating and opening: "<<strerror(errno);
		return Unres;
	}
	if ( (fd2 = fcntl( fd1,F_DUPFD_CLOEXEC,1)) == -1 )
	{
		cerr << "Error in fcntl: "<<strerror(errno);
		return Fail;
	} 
	if ( (flags_ = fcntl( fd2, F_GETFD)) == -1 )
	{
		cerr << "Error in fcntl: "<<strerror(errno);
		return Fail;
	}
	if ( flags_ != FD_CLOEXEC )
	{
		cerr << "setting the close-on-exec flag  for"
                " the  duplicate  descriptor failed " ;
         return Fail;       
	}
	if ( unlink( filename ) == -1 )
	{
		cerr << "Error in unlinking file: "<<strerror(errno);
		return Fail;
	}
	return Success;
}

Status fcntlFD:: fcntlFDIntrSysCallFunc()
{
	struct flock fl;
	pid_t pid;
	const char *filename = "filename";
	int fd;
	if ( (fd = open( filename, O_CREAT | O_RDWR, 0777 )) == -1 )
	{
		cerr << "Error in opening and creating file: "<<strerror(errno);
		return Unres;
	}
	//setting flock structure
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;
	fl.l_pid = getpid();
	fl.l_type = F_WRLCK ;
     
	if ( fcntl ( fd, F_SETLK, &fl ) == -1 )
	{
		cerr << "Error in fcntl: "<<strerror(errno);
		return Unres;
	}
	pid = fork();
	
	if ( pid < 0 )
	{
		cerr << "Error in fork: "<<strerror(errno);
		return Unres;
	}
	else    
	{
		if ( pid == 0 )    /* child proccess*/
	   {
	    	if ( fcntl( fd,F_SETLKW, &fl)  == -1 )
		    {
			cerr << "Error in fcntl: "<<strerror(errno);
			return Unres;
		    }
	    	if ( kill ( getpid(), SIGINT ) == -1 )
		   {
			cerr << "Error in kill system call: "<<strerror(errno);
		    return Unres;	
          }
          if ( fcntl( fd, F_SETLKW, &fl ) != -1 )
          {
		   cerr << "returns 0 in case of system call was interrupted ";
		   return Fail;
	      }
	     if ( errno != EINTR )
	      {
		  cerr << "Incorrect error set in errno in case of "
	              "interrupted system call: "<<strerror(errno);
	      return Fail;  
	      }
      } 
      else
      {
	  }
	  
	}
	
    return Success;	
	
}
