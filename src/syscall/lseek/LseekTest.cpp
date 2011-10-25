//     LseekTest.cpp
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

#include <LseekTest.hpp>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

int LseekTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
         switch (_operation)
		{
 	       case LseekInvalidArg1:
 	           return LseekTestInvalidArg1Func();
 	       case LseekInvalidArg2:
 	           return LseekTestInvalidArg2Func();
 	       case LseekBadFileDesc1:
 	           return LseekTestBadFileDesc1Func();
 	       case LseekBadFileDesc2:
 	           return LseekTestBadFileDesc2Func();
 	       case LseekIllegalSeek:
				return LseekTestIllegalSeekFunc();
 	        default:
				cerr << "Unsupported operation.";
				return Unres;	
	    }
	}
	cerr << "Test was successful";
	return Success;
}
	
	
//EBADF	
//case 1: normal file descriptor is invalid		
Status LseekTest:: LseekTestBadFileDesc1Func()
{
	
	const char  *filename = "filename1";
	int fd;
	int whence = SEEK_CUR;
	
	if ( ( fd = open( filename, O_CREAT | O_RDWR , 0777 ) ) == -1 )
	{
		cerr << "Error in creating and opening file: "<<strerror(errno);
		return Unres;
	}
	
	if ( close( fd )  == -1 )
	{
		cerr << "Error in closing file: "<<strerror(errno);
		return Unres;
	}
	
	if ( lseek( fd, (loff_t)0,whence ) != -1 )
	{
		cerr << "returns 0 in case of bad file descriptor ";
		return Fail;
	}
	
	if ( errno != EBADF )
	{
		cerr << "Incorrect error set in errno in case of bad file descriptor: "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}


//EBADF
//case 2: pipe descriptor is invalid
Status LseekTest:: LseekTestBadFileDesc2Func()
{
	int pfd[2];
	int whence = SEEK_CUR;

	
	if ( pipe( pfd ) == -1 )
	{
		cerr << "Error in creating pipe: "<<strerror(errno);
		return Unres;
	}
	
	//making descriptor invalid
	if ( close ( pfd[0] ) == -1 )
	{
		cerr << "Error in closing pipe: "<<strerror(errno);
		return Unres;
	}
	
     if ( lseek( pfd[0], (loff_t)0, whence ) != -1 )
     {
		 cerr << "returns 0 in case of bad file descriptor ";
		 return Fail;
	 }	
	 
	  if ( errno != EBADF  )
	 {
		 cerr << "Incorrect error set in errno in case of bad file descriptor  "<<strerror(errno);
		 return Fail;
	 }
	
	return Success;
}

//EINVAL	
//case 1: invalid value of whence
Status LseekTest:: LseekTestInvalidArg1Func()
{
	const char  *filename = "filename1";
	int fd;
	int whence = -1;
	
	if ( ( fd = open( filename, O_CREAT | O_RDWR , 0777 ) ) == -1 )
	{
		cerr << "Error in creating and opening file: "<<strerror(errno);
		return Unres;
	}

	
	if ( lseek( fd, (loff_t)0, whence ) != -1 )
	{
		cerr << "returns 0 in case of invalid argument";
		return Fail;
	}
	
	if ( errno != EINVAL )
	{
		cerr << "Incorrect error set in errno in case of invalid argument: "<<strerror(errno);
		return Fail;
	}
	if ( unlink( filename ) == -1 )
	{
		cerr << "Error in unlinking file: "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}

//EINVAL
//case 2: invalid value of offset
Status LseekTest:: LseekTestInvalidArg2Func()
{
	const char  *filename = "filename1";
	int fd;
	int whence = SEEK_CUR;
	
	if ( ( fd = open( filename, O_CREAT | O_RDWR , 0777 ) ) == -1 )
	{
		cerr << "Error in creating and opening file: "<<strerror(errno);
		return Unres;
	}

	//setting invalid value to offset 
	if ( lseek( fd, (loff_t)-1, whence ) != -1 )
	{
		cerr << "returns 0 in case of invalid argument ";
		return Fail;
	}
	
	if ( errno != EINVAL )
	{
		cerr << "Incorrect error set in errno in case of invalid argument: "<<strerror(errno);
		return Fail;
	}
	
	if ( unlink( filename ) == -1 )
	{
		cerr << "Error in unlinking file : "<<strerror(errno);
		return Fail;
	}
	return Success;
}


//ESPIPE
Status LseekTest:: LseekTestIllegalSeekFunc()
{
	int pfd[2];
	int whence = SEEK_CUR;

	
	if ( pipe( pfd ) == -1 )
	{
		cerr << "Error in creating pipe: "<<strerror(errno);
		return Unres;
	}
     if ( lseek( pfd[0], (loff_t)0,whence ) != -1 )
     {
		 cerr << "returns 0 in case of illegal seek ";
		 return Fail;
	 }	
	 
	 if ( errno != ESPIPE )
	 {
		 cerr << "Incorrect error set in errno in case of illegal seek  "<<strerror(errno);
		 return Fail;
	 }
	 
	 if ( close( pfd[0] ) == -1 )
	 {
		 cerr<<"Error in closing pipe: "<<strerror(errno);
		 return Fail;
	 }
	 if ( close( pfd[1] ) == -1 )
	 {
		 cerr << "Error in closing pipe: "<<strerror(errno);
		 return Fail;
	 }
	return Success;
}

