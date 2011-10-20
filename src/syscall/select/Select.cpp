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
			case SelectBadFileDesc1:
				return SelectTestBadFileDesc1Func();
			case SelectBadFileDesc2:
			    return  SelectTestBadFileDesc2Func();
			case SelectNormalCase1:
			     return SelectTestNormalCase1Func();
		   case SelectNormalCase2:
		         return SelectTestNormalCase2Func();
		    case SelectNormalCase3:
		       return SelectTestNormalCase3Func();
	        case SelectNormalPipeCase:
				return SelectTestNormalPipeCaseFunc();
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
//case 1 
Status SelectTest:: SelectTestBadFileDesc1Func()
{
	const char *filename = "somefilename";
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
	if ( unlink ( filename ) == -1 )
	{
		cerr << "Errror in unlinking file: "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}

//EBADF
//case 2
Status SelectTest:: SelectTestBadFileDesc2Func()
{
		int pfd[2];
	fd_set rfds;
    struct timeval tv;
    
    if( pipe( pfd ) == -1 )
    {
		cerr << "Error in creating pipe: "<<strerror(errno);
		return Unres;
	}    
	
	//setting timeval structure
	tv.tv_sec = 0;
	tv.tv_usec = 1;
	
	FD_ZERO( &rfds );
	FD_SET( pfd[0], &rfds );
	
	if ( close( pfd[0] ) == -1 )
	{
		cerr << "Error in closing pipe: "<<strerror(errno);
		return Unres;
	}
	
	if ( select( pfd[0]+1, &rfds, NULL, NULL, &tv ) != -1 )
	{
		cerr <<"returns 0 in case of bad file descriptor ";
		return Fail;
	}
	if ( errno != EBADF )
	{
		cerr <<"Incorrect error set in errno in case of bad file descriptor: "<<strerror(errno);
		return Fail;
	}
	return Success;
}

Status SelectTest :: SelectTestNormalCase1Func()
{
	int rfd, wfd;
	const char *filename1 = "filename1";
	const char *filename2 = "filename2";
	char read_buffer[2] ;
	char write_buffer[2] = "a";
	struct timeval tv;
	int max;
	fd_set rfds, wfds;
	
	if ( (rfd = open ( filename1, O_CREAT | O_RDWR , 0777 )) == -1 )
	{
		cerr << "Error in opening and creating file: "<<strerror(errno);
		return Unres;
	}
	if ( (wfd = open ( filename2 , O_CREAT | O_RDWR , 0777 )) == -1 )
	{
		cerr << "Error in opening and creating file: "<<strerror(errno);
		return Unres;
	}
	max = (rfd > wfd )? rfd : wfd;
	
	//setting timeval structure
	tv.tv_sec = 0;
	tv.tv_usec = 1;
	
	FD_ZERO ( &rfds );
	FD_SET ( rfd, &rfds );
	FD_ZERO ( &wfds );
	FD_SET ( wfd, &wfds );
	
	if ( read( rfd, read_buffer, 1 ) == -1 )
	{
		cerr << "Error in read system call: "<<strerror(errno);
		return Unres;
	}
	if ( write( wfd, write_buffer, 1 ) == -1 )
	{
		cerr << "Error in write system call: "<<strerror(errno);
		return Unres;
	}
    
	 if (  select( max+1, &rfds, &wfds, NULL, &tv ) != 2 ) // must return 2 because 
	 {                                                    //  rfds set contains 1 descriptor, 
	                                                      // and  wfds contains 1 descriptor
		 cerr << "Select failed "<<strerror(errno);
		 return Fail;
	 }

    if ( unlink ( filename1 ) == -1 )
    {
		cerr << "Error in unlinking file: "<<strerror(errno);
		return Fail;
	}
    if ( unlink ( filename2 ) == -1 )
    {
		cerr << "Errror in unlinking file: "<<strerror(errno);
		return Fail;
	}

	return Success;

}
Status SelectTest :: SelectTestNormalCase2Func()
{
	int fd;
	const char *filename = "filename";
	fd_set rfds, wfds;
	struct timeval tv;
	
	if ( (fd = open ( filename, O_CREAT | O_RDWR, 0777 )) == -1 )
	{
		cerr << "Error in opening and creating file: "<<strerror(errno);
		return Unres;
	}
	
	//setting timeval structure
	tv.tv_sec = 0; 
	tv.tv_usec = 1;
	
	FD_ZERO( &rfds );
	FD_SET( fd, &rfds );
	FD_ZERO( &wfds );
	FD_SET( fd, &wfds );
	
	if ( select ( fd+1, &rfds, &wfds, NULL, &tv ) !=  2 ) //must return 2 because
	 {                                                 //rfds set contains 1 descriptor and wfds set contains 1 descriptor 
                                                       //no matter if it's the same
		cerr << "Select failed: "<<strerror(errno);    
		return Fail;
	}
	if ( unlink ( filename ) == -1 )
	{
		cerr << "Error in unlinking file: "<<strerror(errno);
		return Fail;
	}
	return Success;
}

Status SelectTest:: SelectTestNormalCase3Func()
{
	const char *filename = "filename1";
    int fd;
    fd_set rfds;
    struct timeval tv;
    
    if ( (fd = open( filename, O_CREAT | O_RDWR, 0777 )) == -1 )
    {
		cerr << "Error in opening and creating file: "<<strerror(errno);
		return Unres;
	}
	FD_ZERO( &rfds );
	FD_SET( fd, &rfds );
	
	//setting timeval structure
	tv.tv_sec = 0;
	tv.tv_usec = 1;
	if ( select( fd, &rfds, NULL, NULL, &tv ) != 0 ) 
	{
		cerr << "Select failed" <<strerror(errno);
		return Fail;
	}
	
	if ( unlink( filename ) == -1 )
	{
		cerr << "Error in unlinking file: "<<strerror(errno);
		return Fail;
	}
	return Success;
}

Status SelectTest:: SelectTestNormalPipeCaseFunc()
{
	int pipefd[2];
	struct timeval tv;
	fd_set rfds;
	if ( pipe( pipefd ) == -1 )
	{
		cerr << "Error in creating pipe: "<<strerror(errno);
		return Unres;
	}
	 FD_ZERO( &rfds );
	 FD_SET( pipefd[0], &rfds );

	//setting timeval structure
	tv.tv_sec = 0;
	tv.tv_usec = 1;
	
	if ( select( pipefd[0]+1, &rfds, NULL, NULL, &tv ) != 1 )
	{
		cerr << "Select failed: "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}

