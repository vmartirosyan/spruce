//     Select.cpp
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

#include <Select.hpp>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>

int SelectTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case SelectInvalidArg1:
				return SelectTestInvalidArg1Func ();
			case SelectInvalidArg2:
			   return SelectTestInvalidArg2Func();
			case SelectBadFileDesc:
				return SelectTestBadFileDescFunc();
		
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

//EINVAL
//case 1: setting negative argument to nfds 
Status SelectTest:: SelectTestInvalidArg1Func()
{
	fd_set rfds;
	struct timeval tv;
	int fd;
	const char *filename = "somefilename";
	
	if ( (fd= open ( filename, O_CREAT | O_RDWR, 0777 )) == -1 )
	{
		cerr << "Error in creating and opening file: "<<strerror(errno);
		return Unres;
	}
	
	FD_ZERO( &rfds );
	FD_SET( fd, &rfds );
   
    tv.tv_sec = 0;
    tv.tv_usec = 5;
    
    //setting negative argument to nfds    
    if ( select( -1, &rfds, NULL, NULL, &tv ) != -1 )
    {
		cerr << "returns 0 in case of invalid argument ";
		return Fail;
	}
	if ( errno != EINVAL )
	{
		cerr << "Incorrect error set in errno in case of invalid argument "<<strerror(errno);
		return Fail;
	}
	if ( unlink ( filename ) == -1 )
	{
		cerr << "Error in unlinking: "<<strerror(errno);
		return Fail;
	}	
	return Success;
}

//EINVAL
//case 2: setting invalid argument to timeval structure field
Status SelectTest:: SelectTestInvalidArg2Func()
{
	fd_set rfds;
	struct timeval tv;
	
	//setting timeval structure 
	tv.tv_sec = 0;
	tv.tv_usec = -1; 

    FD_ZERO( &rfds );
    FD_SET ( 0, &rfds );
    
    if ( select( 1, &rfds, NULL, NULL, &tv ) != -1 )
    {
		cerr << "returns 0 in case of invalid argument  "<<strerror(errno);
		return Fail;
	}	
	
	if ( errno != EINVAL )
	{
		cerr << "Incorrect error set in errno in case of invalid argument "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}

//EBADF
Status SelectTest:: SelectTestBadFileDescFunc()
{
	const char *filename = "soemfilename";
	int fd;
	fd_set rfds;
	struct timeval tv;
	
	//open and close file to make file descriptor invalid
	if ( ( fd = open ( filename, O_CREAT | O_RDWR , 0777 ) ) == -1 )
	{
		cerr << "Error in opening and creating file: "<<strerror(errno);
		return Unres;
	}
	if ( close ( fd ) == -1 )
	{
		cerr << "Error in closing file: "<<strerror(errno);
		return Unres;
	}
	
	FD_ZERO( &rfds );
	FD_SET( fd, &rfds );
	
	//setting timeval struct
	tv.tv_sec = 0;
	tv.tv_usec = 1;
	
	if ( select ( fd+1, &rfds, NULL, NULL, &tv ) != -1 )
	{
		cerr << "returns 0 in case of bad file descriptor ";
		return Fail;
	} 
	if ( errno != EBADF )
	{
		cerr << "Incorrect error set in errno in case of "
		         "bad file descriptor "<<strerror(errno);
		         return Fail;
	}
	
	return Success;
}


