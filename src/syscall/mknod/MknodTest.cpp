//     MknodTest.cpp
//
// 		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//
//      Author:  Ani Tumanyan <ani.tumanyan92@gmail.com>
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


#include <MknodTest.hpp>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

int MknodTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
         switch (_operation)
		{
			case MknodPathExists:
				return MknodTestPathExistsFunc();
			case MknodIsSymLink:
				return MknodTestIsSymLinkFunc();
			case MknodInvalidArg1:
				return MknodTestInvalidArg1Func();
			case MknodInvalidArg2:
				return MknodTestInvalidArg2Func();
			case MknodBadAdress:
				return MknodTestBadAdressFunc();
			case MknodTooLongPathName:
				return MknodTestTooLongPathNameFunc();
			case MknodNotDir:
				return MknodTestNotDirFunc();
			case MknodPermDenied:
				return MknodTestPermDeniedFunc();
			case MknodNoFile1:
				return MknodTestNoFile1Func();
			case MknodNoFile2:
				return MknodTestNoFile2Func();
			
 	        default:
				cerr << "Unsupported operation.";
				return Unres;   	
	    }
	}
	
	cerr << "Test was successful";
	return Success;
}

//EEXIST
//case 1
Status MknodTest:: MknodTestPathExistsFunc()
{
	struct test_cases {	
	int mode;
	string msg;
 } Test[] = { 
   {S_IFREG | 0777,		"ordinary file with mode 0777 "},
   {S_IFIFO | 0777,		" fifo special with mode 0777 "},
   {S_IFCHR | 0777,		" character special with mode 0777 "},
   {S_IFBLK | 0777,		" block special with mode 0777 "}
    };
    int n = sizeof( Test )/ ( sizeof(string) + sizeof( int ));
	Status status = Success;
	const char *filename = "filename_mknod";
	int fd;
	
	 for ( int i = 0; i < n; ++i )
	 {
		 if ( (fd = open( filename, O_CREAT | O_RDWR, 0777 )) == -1 )
		 {
			 cerr << "Error in creating and opening file: "<<strerror(errno);
			 return Unres;
		 }
		 
		 if ( mknod( filename, Test[i].mode , 0 ) != -1 )
		 {
			cerr << "For" << Test[i].msg.c_str() << " mknod returns 0 in case of File exists. ";
			status = Fail; 
		 }
		 
		 if ( errno != EEXIST )
		 {
			 cerr << "For" << Test[i].msg.c_str() << " mknod sets incorrect error set in errno"
													"in case of File exists: "<<strerror(errno)<< ". ";
		   status = Fail;
		 }
		 if ( unlink( filename ) == -1 )
		 {
			 cerr << "Error in unlinking file: "<<strerror(errno);
			 return Unres;
		 }
     }
	return status;
}

//EEXIST
//case 2
Status MknodTest :: MknodTestIsSymLinkFunc()
{
	struct test_cases {	
	int mode;
	string msg;
 } Test[] = { 
   {S_IFREG | 0777,		"ordinary file with mode 0777 "},
   {S_IFIFO | 0777,		" fifo special with mode 0777 "},
   {S_IFCHR | 0777,		" character special with mode 0777 "},
   {S_IFBLK | 0777,		" block special with mode 0777 "}
    };
	int n = sizeof( Test )/ ( sizeof(string) + sizeof( int ));	
   	Status status  = Success;
	const char *node_name = "node";
	const char *link = "link";
	
	for ( int i = 0; i < n; ++i )
	{
		if ( symlink( node_name, link ) == -1 )
		{
			cerr << "Error in creating symbolic link: "<<strerror(errno);
			return Unres;
		}
		
		if ( mknod( link, Test[i].mode , 0 ) != -1 )
		{
			   cerr <<"For"<< Test[i].msg.c_str() << " mknod returns 0 in case of File exists. ";
			   status = Fail;
		 }
		 
		 if ( errno != EEXIST )
		 {
			 cerr << "For" << Test[i].msg.c_str() << " mknod sets incorrect error set in errno "
			                                      "in case of File exists" << strerror(errno) << ". ";
			 status = Fail;
		 }
	      
	      if ( unlink( link ) == -1 )
	      {
			  cerr << "Error in ulinking file: "<<strerror(errno);
			  return Unres;
		  }	   
		
	}
	
	return status;
	
}

//EINVAL

//case 1
Status MknodTest :: MknodTestInvalidArg1Func()
{
	const char *filename = "filename_mknod";
	
	//setting invalid argument to mode
	if ( mknod( filename, -1, 0 ) != -1 )
	{
		cerr << "returns 0 in case of invalid argument ";
		return Fail;
	}
	
	if ( errno != EINVAL )
	{
		cerr << "Incorrect error set in errno in case of invalid argument "<<strerror(errno);
		return Fail;
	}
	
	return Success; 
	
}

//case 2
Status MknodTest :: MknodTestInvalidArg2Func()
{
	const char *filename = "filename_mknod";
	
	//setting invalid argument to dev
	if ( mknod( filename, S_IFREG, -1 ) != -1 )
	{
		cerr << "returns 0 in case of invalid argument "<<strerror(errno);
		return Fail;
	}
	
	if ( errno != EINVAL )
	{
		cerr << "Incorrect error set in errno in case of invalid argument "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}

//EFAULT
Status MknodTest :: MknodTestBadAdressFunc()
{
	
	if ( mknod( (const char *)-1, S_IFREG, 0 ) != -1 )
	{
		cerr << "returns 0 in case of bad adress ";
		return Fail;
	}
	
	if ( errno != EFAULT )
	{
		cerr << "Incorrect error set in errno in case of bad adress "<<strerror(errno);
		return Fail;
	}
	
	
	return Success;
}

//ENAMETOOLONG
Status MknodTest :: MknodTestTooLongPathNameFunc()
{
	struct test_cases {	
	int mode;
	string msg;
 } Test[] = { 
   {S_IFREG | 0777,		"ordinary file with mode 0777 "},
   {S_IFIFO | 0777,		" fifo special with mode 0777 "},
   {S_IFCHR | 0777,		" character special with mode 0777 "},
   {S_IFBLK | 0777,		" block special with mode 0777 "}
    };
	int n = sizeof( Test )/ ( sizeof(string) + sizeof( int ));
	char longfilename[PATH_MAX+1];
	Status status = Success;
	
	memset( longfilename, 'a', PATH_MAX + 1);
	
	for ( int i =  0; i < n; ++i )
	{
		if ( mknod ( longfilename, Test[i].mode, 0 ) != -1 )
		{
			cerr << "For " << Test[i].msg.c_str() <<"mknod returns 0 in case of "
													"too long pathname. ";
			status = Fail;									
		}
		
		if ( errno != ENAMETOOLONG )
		{
			cerr << "For "<<Test[i].msg.c_str()<< " mknod sets incorrect error set in errno "
									"in case of too long pathname: "<<strerror(errno)<< ". ";
		  status = Fail;
		}
	}
	
	return status;
}

//ENOTDIR
Status MknodTest :: MknodTestNotDirFunc()
{
	string filename = "file_mknod";
	string otherfilename = "somefile";
	string pathname = otherfilename + "/" + filename;
	
	if ( open( otherfilename.c_str(), O_CREAT | O_RDWR, 0777) == -1 )
	{
		cerr << "Error in opening and creating file: "<<strerror(errno);
		return Unres;
	}
	
	if ( mknod( pathname.c_str(), S_IFREG | 0777, 0 ) != -1 )
	{
		cerr << "returns 0 in case of not directory in the pathname "<<strerror(errno);
		return Fail;
	}
	
	if ( errno != ENOTDIR )
	{
		cerr << "Incorrect error set in errno in case of "
		        "not directory in the pathname "<<strerror(errno);
		        
		 return Fail;
	}
	
	if ( unlink( otherfilename.c_str() ) == -1 )
	{
		cerr << "Error in unlinking file:  "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}

//EACCES
Status MknodTest :: MknodTestPermDeniedFunc()
{
	string filename = "file", dirname = "directory";
	string pathname = dirname + "/" + filename;
	
	if ( mkdir( dirname.c_str(), 0000 ) == -1 )
	{
		cerr << "Error in creating directory: "<<strerror(errno);
		return Unres;
	}
	
	if ( mknod( pathname.c_str(), S_IFREG | 0777 , 0 ) != -1 )
	{
		cerr << "returns 0 in case of permission denied ";
		return Fail;
	}
	
	if ( errno != EACCES )
	{
		cerr << "Incorrect error set in errno in case of permission denied "<<strerror(errno);
		return Fail;
	}
	
	if ( rmdir( dirname.c_str() ) == -1 )
	{
		cerr << "Error in removing directory: "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}

//ENOENT

//case 1
Status MknodTest :: MknodTestNoFile1Func()
{
	const char *filename = "";
	
	if ( mknod( filename, S_IFREG | 0777, 0 ) != -1 )
	{
		cerr << "returns 0 in case of no such file or directory "<<strerror(errno);
		return Fail;
	}
	
	if ( errno != ENOENT )
	{
		cerr << "Incorrect error set in errno in case of no such file or directory "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}

//case 2
Status MknodTest :: MknodTestNoFile2Func()
{
	string dirname = "mknod_dirname";
	string link = dirname + "/link";
	string filename = link + "/filename1";
	
	if( mkdir( dirname.c_str(), 0777 ) == -1 )
	{
		cerr << "Error in creating directory: "<<strerror(errno);
		return Unres;
	}
	
	if ( symlink( dirname.c_str(), link.c_str() ) == -1 )
	{
		cerr << "Error in creating symbolic link: "<<strerror(errno);
		return Unres;
	}
	
	if ( unlink( link.c_str() ) == -1 )
	{
		cerr << "Error in unlinking symbolic link: "<<strerror(errno);
		return Unres;
	}
	
	if ( mknod( filename.c_str(), S_IFREG, 0 ) != -1 )
	{
		cerr << "returns 0 in case of no such file or directory ";
		return Fail;
	} 
	
	if ( errno != ENOENT )
	{
		cerr << "Incorrect error set in errno in case of" 
		        "no such file or directory "<<strerror(errno);
		 return Fail;
	}
	
	//cleaning up
	
	if ( rmdir( dirname.c_str() ) == -1 )
	{
		cerr << "Error in removing directory: "<<strerror(errno);
		return Unres;
	}
	
	return Success;
}
