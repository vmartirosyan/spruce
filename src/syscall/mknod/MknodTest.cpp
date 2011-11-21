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
			
 	        default:
				cerr << "Unsupported operation.";
				return Unres;   	
	    }
	}
	
	cerr << "Test was successful";
	return Success;
}

//EEXIST
Status MknodTest:: MknodTestPathExistsFunc()
{
	const char *filename = "filename_mknod";
	int fd;
	if ( (fd = open( filename, O_CREAT | O_RDWR, 0777)) == -1 )
	{
		cerr << "Error in opening and creating file: "<<strerror(errno);
		return Unres;
	}
	
	if ( mknod( filename, S_IFREG | 0777, 0 ) != -1 )
	{
		cerr << "returns 0 in case of file exists ";
		return Fail;
	}
	
	if ( errno != EEXIST )
	{
		cerr << "Incorrect error set in errno in case of File exists "<<strerror(errno);
		return Fail;
	}
	
	if ( unlink( filename ) == -1 )
	{
		cerr << "Error in unlinking file:  "<<strerror(errno);
		return Fail;
	}
	
	return Success;
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
	char longfilename[PATH_MAX+1];
	
	memset( longfilename, 'a', PATH_MAX + 1);
	
	if ( mknod( longfilename, S_IFREG, 0 ) != -1 )
	{
		cerr << "returns 0 in case of too long pathname ";
		return Fail;
	} 
	
	if ( errno != ENAMETOOLONG )
	{
		cerr << "Incorrect error set in errno in case of too long pathname "<<strerror(errno);
		return Fail;
	}
	
	
	return Success;
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
