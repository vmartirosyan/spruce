//      UnlinkAt.cpp
//      
// 		Copyright (C) 2011, Institute for System Programming	
//                          of the Russian Academy of Sciences (ISPRAS)
//
//      Author: Ani Tumanyan <ani.tumanyan92@gmail.com>
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


#include <UnlinkAt.hpp>


#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>
#include <errno.h>
#include <string.h>

int UnlinkAtTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case UnlinkAtTestIsNotDirectory:
				return UnlinkAtTestIsNotDirectoryFunction();
			case UnlinkAtTestBadFlagValue:
			   return UnlinkAtTestBadFlagValueFunction();
			case UnlinkAtTestBadFileDescriptor1:
				return UnlinkAtTestBadFileDescriptor1Function();
		    case  UnlinkAtTestBadFileDescriptor2:
		        return  UnlinkAtTestBadFileDescriptor2Function();
		     case UnlinkAtTestNormalUnlink:
				return UnlinkAtTestNormalUnlinkFunction();
			case  UnlinkAtTestNormalRemove:
				return UnlinkAtTestNormalRemoveFunction();
			case UnlinkAtTestRemove1:
				return UnlinkAtTestRemove1Function();
			case UnlinkAtTestRemove2:
				return UnlinkAtTestRemove2Function();
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status UnlinkAtTest::UnlinkAtTestIsNotDirectoryFunction()
{
	const char *dirname = "directory_s1";
	const char *pathname = "file_s1";
	
	if ( mkdir(dirname,0777) == -1  )
	{
		cerr << "Unable to creat directory "<< strerror(errno) ;
		return Unres;
	}
	if( creat( pathname, 0777 ) == -1 )
	{
		cerr << "Unable to creat file " <<strerror(errno);
		return Unres;
	}
	int fd = open( pathname, O_RDWR );
	
	if ( fd == -1 )
	{
		cerr << "Unable to open file" << strerror(errno) ;
		return Unres;
	}
	
    if ( unlinkat( fd, dirname, AT_REMOVEDIR ) != -1 ) 
	{
			cerr<< "returns 0 in case of not directory  descriptor ";
	if ( rmdir( dirname ) == -1 )
		cerr << "Error: unable to remove directory "<<strerror(errno) ;
	if ( unlink ( pathname ) == -1 )
	    cerr << "Error: unable to unlink file "<<strerror(errno);
		return Fail;
	}
	
	if ( errno != ENOTDIR )
	{
		cerr << "Incorrect error set in errno in case of not direcrory descriptor" <<strerror(errno);
		if ( rmdir ( dirname ) == -1 )
			cerr << "Error: unable to remove directory "<<strerror(errno);
		if ( unlink ( pathname ) == -1 )
			cerr << "Error: unable to unlink file "<<strerror(errno);
		return Fail; 
	}
	if ( unlink( pathname ) == -1 )
	{
		cerr << "Error: unable to unlink file "<<strerror(errno);
		return Fail;
	}
	if ( rmdir( dirname ) == -1 )
	{
		cerr<<"Error: unable to remove directory "<<strerror(errno);
		return Fail;
	}
	return Success;
	
}     
Status UnlinkAtTest:: UnlinkAtTestBadFlagValueFunction()
{  
	int ret_unlinkat ;
	const char *pathname = "something1";
	
	if (creat( pathname, 0777 ) == -1 )
	{
		cerr << "Error: unable to creat file "<< strerror(errno) ;
		return Unres;
	}

	 ret_unlinkat = unlinkat( AT_FDCWD, pathname, O_RDWR );
	 if ( ret_unlinkat != -1 )
	{
		cerr << "returns 0 in case of invalid flags value specified in flags" ;
		if ( unlink( pathname ) == -1 )
			cerr << "unable to unlink file "<< strerror(errno);
          return Fail;
	}
	if ( errno != EINVAL )
	{
		cerr << "Incorrect error set in errno in case of invalid flags value "<<strerror(errno) ;
		if ( unlink ( pathname ) == -1 )
		  cerr << "unable to unlink file "<<strerror(errno);
		return Fail;
	}
	if ( unlink( pathname ) == -1 )
	    cerr << "unable to unlink file "<< strerror(errno);
	    
    return Success;
 }
 
Status UnlinkAtTest:: UnlinkAtTestBadFileDescriptor1Function()
{
	
	const char *pathname = "something";
    // trying to unlink with invalid descriptor equal to -1
    if ( unlinkat( -1, pathname , 0 ) != -1 )
    {
		cerr<< "returns 0 in case of not valid  descriptor" ;
		return Fail;
	}
	if ( errno != EBADF )
	{
		cerr << "Incorrect error set in errno in case of not valid descriptor "<<strerror(errno);
		return Fail;
	}
	return Success;
}

Status UnlinkAtTest:: UnlinkAtTestBadFileDescriptor2Function()
{
	int dirdesc;
	const char *dirname = "some_dir_1";
	const char *filename = "some_file_1";
	//creating directory
	if ( mkdir ( dirname, 0777 ) == -1 )
	{
		cerr << "Error: unable to create directory "<< strerror(errno); 
		return Unres;
	}
	//open and close directory to make directory descriptor invalid
	 dirdesc = open ( dirname, O_DIRECTORY ) ;
	if ( dirdesc == -1 )
	{
		cerr << " Unable to open directory "<< strerror(errno);
		return Unres;
	}
	if ( close ( dirdesc ) == -1 )
	{
		cerr << "Error: unable to close directory "<< strerror(errno);
		return Unres;
	}
	
	//trying to unlink file with invalid directory descriptor
	if ( unlinkat( dirdesc, filename, 0) != -1 )
	{
		cerr << "returns 0 in case of invalid descriptor ";
		return Fail;
	}
	
	if ( errno != EBADF )
	{
		cerr << "Incorrect error set in errno in case of invalid descriptor "<< strerror(errno);
		return Fail;
	}
	if ( rmdir( dirname ) == -1 )
	{
		cerr << "Error: unable to remove directory "<< strerror(errno);
		return Fail;
	}
	
	return Success;
	
}

Status UnlinkAtTest:: UnlinkAtTestNormalUnlinkFunction(  )
{

	int dirfd_, ret_unlinkat;
	string path = "unlinkat_file1.txt";
	string dir = "unlinkat_dir1";
	string abs_path = dir+"/"+path;
	
	//setting up
	if ( mkdir( dir.c_str() , 0777 ) == -1 )
	{
		cerr << "Error: unable to make directory "<< strerror(errno);
		return Unres;
	}
	dirfd_ = open ( dir.c_str(), O_DIRECTORY );
	if ( dirfd_ < 0 )
	{
		cerr << "Error: unable to open directory "<< strerror(errno);
		return Unres;
	}
    if ( creat( abs_path.c_str(), 0777 ) == -1 )
    {
		cerr << "Error: unable to creat file "<<strerror(errno);
		return Unres;
	}
	
    ret_unlinkat = unlinkat( dirfd_, path.c_str(), 0 );
    if ( ret_unlinkat !=0 )
    {
		cerr << "Error: in case of Success "<<strerror(errno);
		return Fail;
	}
	
	//checking 
	if ( open ( abs_path.c_str(), O_RDWR ) != -1 )
	{
		cerr << "Error: opened unlinked file "<<endl;
		return Fail;
	}
	
	//cleaning up
    if ( rmdir ( dir.c_str() ) == -1 )
    {
		cerr << "Error: unable to remove directory "<<strerror(errno);
		return Fail;
	}

	return Success;

}

Status UnlinkAtTest::UnlinkAtTestNormalRemoveFunction()
{
	int dirfd_, ret_unlinkat;
	string dirname = "unlinkat_dir1";
	string subdirname = "unlinkat_subdir1";
	string abs_path = dirname + "/" + subdirname;
	
	//setting up
	if ( mkdir( dirname.c_str(), 0777 ) == -1 )
	{
		cerr << "Error: unable to make directory "<< strerror(errno);
		return Unres;
	}
	 dirfd_ = open( dirname.c_str(), O_DIRECTORY );
	 if ( dirfd_ < 0 )
	 {
		 cerr << "Error: unable to open directory "<< strerror(errno);
		 return Unres;
	 }
	if ( mkdir( abs_path.c_str(), 0777 ) == -1)
	{
		cerr << "Error: unable to make directory "<<strerror(errno);
		return Unres;
	}
	
	ret_unlinkat = unlinkat( dirfd_, subdirname.c_str(), AT_REMOVEDIR );
	if ( ret_unlinkat == -1 )
	{
		cerr << "Error: in case of Success "<<strerror(errno);
		return Fail;
	}
	
	//checking 
	if ( open ( abs_path.c_str(), O_DIRECTORY ) != -1 )
	{
		cerr << "Error: opened removed directory "<<strerror(errno);
		return Fail;
	}
	
	//cleaning up
	if ( rmdir( dirname.c_str() ) == -1 )
	{
		cerr << "Error: unable to remove directory "<<strerror(errno);
		return Fail;
	}
	
	
	return Success;
	
}

//EINVAL 
// setting _flags to AT_REMOVEDIR and '.' as last component of the path
Status UnlinkAtTest::UnlinkAtTestRemove1Function()
{
	int dirfd , ret_unlinkat;
	string dir = "unlinkat_rm_dir0";
	string subdir = ".";
	string abs_path = dir + "/" + subdir;
	
	//setting up
	if ( mkdir( dir.c_str(), 0777 ) == -1 )
	{
		cerr << "Error: unable to make directory "<<strerror(errno);
		return Unres;
	}
	 dirfd = open( dir.c_str(), O_DIRECTORY );
	 if ( dirfd < 0 )
	 {
		 cerr << "Error: unable to open directory "<<strerror(errno);
		 return Unres;
	 } 
	 
	 ret_unlinkat = unlinkat( dirfd, subdir.c_str(), AT_REMOVEDIR );
	 if ( ret_unlinkat != -1 )
	 {
		 cerr << "unlinkat returs 0 in case of path contains '.' as last component ";
		 return Fail;
	 }
	 if ( errno != EINVAL )
	 {
		 cerr << "Incorrect error set in errno in case of path contains"
		          " '.' as last component "<< strerror(errno);
		 return Fail;
	 }
	 
	 
	 //cleaning up
	 if ( rmdir( dir.c_str() ) == -1 )
	 {
		 cerr << "Error : unable to remove directory ";
		 return Fail;
	 }
	 
	 return Success;
}
	
//ENOTEMPTY
//setting _flags to AT_REMOVEDIR and '..' as last component of file path
Status UnlinkAtTest:: UnlinkAtTestRemove2Function ()	
{
    int dirfd , ret_unlinkat;
	string dir = "unlinkat_rm_dir0";
	string subdir = "..";
	string abs_path = dir + "/" + subdir;
	
	//setting up
	if ( mkdir( dir.c_str(), 0777 ) == -1 )
	{
		cerr << "Error: unable to make directory "<<strerror(errno);
		return Unres;
	}
	 dirfd = open( dir.c_str(), O_DIRECTORY );
	 if ( dirfd < 0 )
	 {
		 cerr << "Error: unable to open directory "<<strerror(errno);
		 return Unres;
	 } 
	 
	 ret_unlinkat = unlinkat( dirfd, subdir.c_str(), AT_REMOVEDIR );
	 if ( ret_unlinkat != -1 )
	 {
		 cerr << "unlinkat returs 0 in case of path contains '..' as last component ";
		 return Fail;
	 }
	 if ( errno != ENOTEMPTY )
	 {
		 cerr << "Incorrect error set in errno in case of path contains"
		          " '..' as last component "<< strerror(errno);
		 return Fail;
	 }
	 
	 //cleaning up
	 if ( rmdir( dir.c_str() ) == -1 )
	 {
		 cerr << "Error : unable to remove directory ";
		 return Fail;
	 }
	 
	  
	  return Success;
}
