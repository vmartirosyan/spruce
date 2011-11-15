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
#include <sys/socket.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>


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
 	       case LseekIllegalSeek1:
			   return LseekTestIllegalSeek1Func();
		   case LseekIllegalSeek2:
			   return LseekTestIllegalSeek2Func();
		   case LseekIllegalSeek3:
			   return LseekTestIllegalSeek3Func();
		   case LseekNormalCase1: 
	 	       return LseekTestNormalCase1Func();
		   case LseekNormalCase2:
			   return LseekTestNormalCase2Func();
		   case LseekNormalCase3:
			   return LseekTestNormalCase3Func();
		 
		   case Lseek64InvalidArg1:
			   return Lseek64TestInvalidArg1Func();
		   case Lseek64InvalidArg2:
			   return Lseek64TestInvalidArg2Func();
		   case Lseek64BadFileDesc1:
			   return Lseek64TestBadFileDesc1Func();
		   case Lseek64BadFileDesc2:
			   return Lseek64TestBadFileDesc2Func();
		   case Lseek64NormalCase:
			   return Lseek64TestNormalCaseFunc(); 
		
		 
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
//case 1: descriptor assosiated with the pipe
Status LseekTest:: LseekTestIllegalSeek1Func()
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

//ESPIPE
//case 2: descriptor assosiated with the socket
Status LseekTest:: LseekTestIllegalSeek2Func()
{
	int whence = SEEK_CUR;
    int sd ;
    if ( (sd= socket(AF_INET, SOCK_STREAM, 0)) == -1 )
    {
		cerr << "Error in socket system call: "<<strerror(errno);
		return Unres;
	}
    
    if ( lseek( sd, (loff_t)0, whence ) != -1 )
    {
		cerr << "returns 0 in case of illegal seek";
		return Fail;
	}
	
	if ( errno != ESPIPE )
	{
		cerr << "Incorrect error set in errno in case of illegal seek: "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}

//ESPIPE
// case 3: descriptor assosiated with the FIFO
Status LseekTest:: LseekTestIllegalSeek3Func()
{
	
     const char *filename = "somefilename"; 
	 int whence = SEEK_CUR;
	
    int fifodesc;
    if ( (fifodesc = mkfifo(filename,  0)) == -1)
    {
		cerr << "Error in creating FIFO file: "<<strerror(errno);
		return Unres;
	}
    
    if ( lseek( fifodesc, (loff_t)0, whence ) != -1 )
    {
		cerr << "returns 0 in case of illegal seek" ;
		return Fail;
	}
	
	if (errno != ESPIPE )
	{
		cerr << "Incorrect error set in errno in case of illegal seek "<<strerror(errno);
		return Fail;
	}
	if ( unlink( filename ) == -1 )
	{
		cerr << "Error in unlinking file: "<<strerror(errno);
		return Fail;
	}
	return Success;
}

//Normal functionality tests

Status LseekTest :: LseekTestNormalCase1Func()
{
	int fd; 
	const char *filename = "somefile";
	off_t ret_lseek;
	int somenumber = rand();
	int whence  = SEEK_SET;
	
	if ( (fd = open( filename , O_CREAT | O_RDWR, 0777 )) == -1 )
	{
		cerr << "Error in opening and creating file: "<<strerror(errno);
		return Unres;
	}
	
	if ( (ret_lseek = lseek( fd, (loff_t)somenumber , whence )) == -1 )
	{
		cerr << "Error in lseek system call: "<<strerror(errno);
		return Fail;
	}
	
	if ( ret_lseek != somenumber )
	{
		cerr << "lseek failed "; 
		return Fail;
	}
	
	if ( unlink( filename ) == -1 )
	{
		cerr << "Error in unlinking file: "<<strerror(errno);
		return Fail;
	}
	return Success;
}

Status LseekTest :: LseekTestNormalCase2Func()
{
     int fd;
	const char *filename = "somefilename1";
    char write_buffer1[BUFSIZ];
	char write_buffer2[BUFSIZ];
	char read_buffer[BUFSIZ];
	struct stat stat_buf;
	size_t file_size;
	off_t offset , ret_offset;
	
	strcpy( write_buffer1, "abcd" );
	strcpy( write_buffer2, "efg" );
	
	if ( (fd= open( filename, O_CREAT | O_RDWR, 0777 )) == -1 )
	{
		cerr << "Error in opening and creating file: "<<strerror(errno);
		return Unres;
	}
	
	if ( write( fd, &write_buffer1, strlen(write_buffer1) ) != strlen(write_buffer1) )
	{
		cerr << "Error in write system call: "<<strerror(errno);
		return Unres;
	}
	
	if ( fstat( fd, &stat_buf ) ==-1 )
	{
		cerr << "Error in fstat system call: "<<strerror(errno);
		return Unres;
	}
	
	file_size =  stat_buf.st_size;
	
	offset = file_size + strlen(write_buffer2);
	
	if ( (ret_offset =  lseek( fd, offset, SEEK_SET )) == -1 )
	{
		cerr << "Error in lseek system call: "<<strerror(errno);
		return Fail;
	}
	
	if ( ret_offset != offset )
	{
		cerr << "lseek failed 1"<<endl;
		return Fail;
	}
  	
	if ( write( fd, &write_buffer2, strlen(write_buffer2) ) != strlen(write_buffer2) )
	{
		cerr << "Error in write system call: "<<strerror(errno);
		return Fail;
	}

	if ( unlink( filename ) == -1 )
	{
		cerr << "Error in unlinking file: "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}

Status LseekTest :: LseekTestNormalCase3Func()
{
	int fd;
	
	const char *filename = "new_lseek_file";
	char write_buffer1[BUFSIZ];
	char write_buffer2[BUFSIZ];
	char read_buffer[BUFSIZ];
 	struct stat stat_buf;
    size_t file_size;
	off_t offset , ret_offset;
	
	strcpy( write_buffer1, "abcd" );
	strcpy( write_buffer2, "efg" );
	
	
     if ( (fd = open( filename, O_CREAT | O_RDWR, 0777 )) == -1 )
     {
		 cerr << "Error in opening and creating file: "<<strerror(errno);
		 return Unres;
	 }
	
	if ( write( fd, &write_buffer1, strlen(write_buffer1) ) != strlen(write_buffer1) )
	{
		cerr << "Error in write system call: "<<strerror(errno);
		return Unres;
	}
	
	//getting size of file after writing to it
	if ( fstat( fd, &stat_buf ) == -1 )
	{
		cerr << "Error in fstat system call: "<<strerror(errno);
		return Unres;
	}
	
	file_size =  stat_buf.st_size;
	
	offset = file_size + strlen(write_buffer2);
	
	if ( (ret_offset =  lseek( fd, offset, SEEK_SET )) == -1 )
	{
		cerr << "Error in lseek system call: "<<strerror(errno);
		return Fail;
	}
	
	if ( ret_offset != offset )
	{
		cerr << "lseek failed "<<endl;
		return Fail;
	}
  	
	if ( write( fd, &write_buffer2, strlen( write_buffer2 ) ) != strlen(write_buffer2) )
	{
		cerr << "Error in write system call: "<<strerror(errno);
		return Fail;
	}
	
    if ( lseek( fd, (loff_t)0, SEEK_SET)  == -1 )
    {
		 cerr << "Error in lseek system call: "<<strerror(errno);
		 return Fail;
     }
	 
	if ( read( fd, &read_buffer, (offset +strlen(write_buffer2)) ) == -1 )
	{
		cerr << "Error in read system call: "<<strerror(errno);
		return Unres;
	}
	
	if ( strncmp( read_buffer, write_buffer1,strlen(write_buffer1) ) != 0  )
	{
		cerr << "lseek failed ";
		return Fail;
	}
	
	if ( strncmp( &read_buffer[offset], write_buffer2, strlen(write_buffer2) ) != 0 )
	{
		cerr << "lseek failed 2";
		return Fail;
	}
	
	if ( unlink( filename ) == -1 )
	{
		cerr << "Error in unlinking file: "<<strerror(errno);
		return Fail;
	}
	return Success;
	
}

             /*tests for lseek64 system call*/

//EINVAL
//case 1
Status LseekTest:: Lseek64TestInvalidArg1Func()
{
	
    int fd;  
	int whence = SEEK_SET;
	const char *filename = "filename_lseek";
  
  	if ( (fd = open( filename, O_CREAT | O_RDWR, 0777 )) == -1 )
  	{
		cerr << "Error in opening and creating file: "<<strerror(errno);
		return Unres;
	}
	
	//setting invalid ( negative ) value to offset
	
	if ( lseek64( fd, (off64_t)-1, whence ) != -1 )
	{
		cerr << "returns 0 in case of invalid argument";
		return Fail;
	}
    if ( errno != EINVAL )
    {
		cerr << " Incorrect error set in errno in case of invalid argument "<<strerror(errno);
		return Unres;
	}
	
	if ( unlink( filename ) == -1 )
	{
		cerr << "Error in unlinking file: "<<strerror(errno);
		return Fail;
	}
	 
	return Success;
}
//EINVAL
//case 2
Status LseekTest:: Lseek64TestInvalidArg2Func()
{
	int fd;
	const char *filename = "filename_lseek";
	
	if ( (fd = open( filename, O_CREAT| O_RDWR, 0777 )) == -1 )
	{
		cerr<<"Error in opening and creating file: "<<strerror(errno);
		return Unres;
	}
	
	//setting invalid value to whence
	
	if ( lseek64( fd, (off64_t)0, -1 ) != -1 )
	{
		cerr << "returns 0 in case of in valid argument";
		return Fail;
	}
	
     if ( errno != EINVAL )
     {
		 cerr << "Incorrect error set in errno in case of invalid argument "<<strerror(errno);
		 return Fail;
	 }	
	 
	 
	 if ( unlink( filename ) == -1 )
	 {
		 cerr << "Error in unlinking file: "<<strerror(errno);
		 return Fail;
	 }
	 
	return Success;
}

//EBADF
//case 1
Status LseekTest :: Lseek64TestBadFileDesc1Func()
{
	int whence = SEEK_SET;
	
	//setting invalid file descriptor
	
	if ( lseek64( -1, (off64_t)0, whence ) != -1 )
	{
		cerr << "returns 0 in case of bad file descriptor";
		return Fail;
	}
	if ( errno != EBADF )
	{
		cerr << "Incorrect error set in errno in case of bad file descriptor "<<strerror(errno);
		return Fail;
	}
	
	return Success;
} 

//EBADF
//case 2
Status LseekTest :: Lseek64TestBadFileDesc2Func()
{
	int fd;
	const char *filename = "filename_lseek64";
	int whence = SEEK_CUR;
	
	//open and close file to make descriptor invalid
	if ( (fd = open( filename, O_CREAT | O_RDWR , 0777 )) == -1 )
	{
		cerr << "Error in opening and creating file: "<<strerror(errno);
		return Unres;
	}
	
	if ( close( fd ) )
	{
		cerr << "Error in closing file: "<<strerror(errno);
		return Unres;
	}
	
	if ( lseek64( fd, (off64_t)0, whence ) != -1 )
	{
		cerr << "returns 0 in case of bad file descriptor";
		return Fail;
	}
	
	if ( errno != EBADF )
	{
		cerr << "Incorrect error set in errno in case of bad file descriptor "<<strerror(errno);
		return Fail;
	}
	
	if ( unlink( filename ) == -1 )
	{
		cerr << "Error in unlinking file: "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}

//tests lseek64  normal functionality

Status LseekTest :: Lseek64TestNormalCaseFunc()
{
	int whence = 1;
	int fd, ret_lseek64;
	const char *filename = "lseek64_file";
	off64_t offset = rand();
	
	if ( ( fd = open( filename, O_CREAT | O_RDWR , 0777 )) == -1 )
	{
		cerr << "Error in opening and creating file: "<<strerror(errno);
		return Unres;
	}
	
	if ( (ret_lseek64 = lseek64( fd, ( off64_t )offset, whence )) == -1 )
	{
		cerr << "Error in lseek64 system call: "<<strerror(errno);
		return Fail;
		
	}
	
	if ( ret_lseek64 != offset )
	{
		cerr << "lseek64 failed ";
		return Fail;
	}
	
	if ( unlink( filename ) == -1 )
	{
		cerr << "Error in unlinking file: "<<strerror(errno);
		return Fail;
	}
	
	return Success;
	

}
   
