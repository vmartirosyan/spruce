//      Unlink.cpp
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


#include <Unlink.hpp>


#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>
#include <errno.h>
#include <string.h>
#include "File.hpp"



int UnlinkTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case UnlinkTestTooLongPath:			
				return UnlinkTestTooLongPathFunc();				
			case UnlinkTestNormalFile:			
				return UnlinkTestNormalFileFunc();	
			case UnlinkTestNormalCase:
				return UnlinkTestNormalCaseFunc();
			case UnlinkTestIsNotDirectory:			
				return UnlinkTestIsNotDirectoryFunc();				
			case UnlinkTestNoSuchFile:			
				return UnlinkTestNoSuchFileFunc();				
			case UnlinkIsDirectory:			
				return UnlinkIsDirectoryFunc();				 
			case UnlinkTestEmptyPath:			
				return UnlinkTestEmptyPathFunc();				
			case UnlinkTestNegativeAdress:			
				return UnlinkTestNegativeAdressFunc();				 
			case UnlinkTestPermissionDenied:			
				return UnlinkTestPermissionDeniedFunc(); 
				 				
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status UnlinkTest::UnlinkTestNormalFileFunc ()
{
	
	int ret_unlink;
	const char *pathname = "file.txt";
	
	if ( open( pathname, O_CREAT | O_RDWR, 0775 ) == -1 )
	{
		cerr << "Error in opening and creating file: "<<strerror(errno);
		return Unres;
	}
   
    ret_unlink = unlink ( pathname );
	if ( ret_unlink == -1)
	{
		cerr<< "Error occured during unlinking normal file" << strerror(errno);
		return Unres;
	}
	
	 // Check if file exists
	if ( access( pathname, F_OK ) == 0 )
	{
		cerr<< " Error: unlinked file still exists ";
		return Fail;
	}
    
	return Success;

}

Status UnlinkTest::UnlinkTestTooLongPathFunc ()
{
	int ret_unlink;
	char longpathname[PATH_MAX + 2];
	int ind;

	for ( ind = 0; ind <= PATH_MAX + 1; ++ind ) 
	{
		longpathname[ind] = 'a';
	}
	ret_unlink = unlink( longpathname );
	if ( ret_unlink == 0 )
	{
		cerr << "unlink returns 0 in case of too long  path name";
	    return Fail;
	} 
  	if( errno != ENAMETOOLONG )
	{
				
		cerr << "Incorrect error set in errno in case of too long file name "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}

Status UnlinkTest::UnlinkTestIsNotDirectoryFunc ()
{
	int ret_unlink;
	const char *pathname = "something/file.txt";
	
	if( creat( "something", 0777 ) == -1 )
	{
		cerr << "Unable to creat file " << strerror(errno);
		return Unres;
	}
	ret_unlink = unlink( pathname );
	
	if ( ret_unlink == 0 )
	{
		cerr << "unlink returns 0 in case of  not directory in path name";
	    return Fail;
	} 
	if ( errno != ENOTDIR )
	{
		cerr << "Incorrect error set in errno in case of not directory in path name "<<strerror(errno);
		return Fail;
	}
	
	if ( unlink( "something" ) == -1 )
	{
		cerr << " Unable to unlink file `something` "<< strerror(errno);
		return Fail;
	}
	
	return Success;
}

Status UnlinkTest::UnlinkTestNoSuchFileFunc ()
{
	int ret_unlink;
	int fd;
	const char *pathname ="unlink.txt";
	//checking: if file exists unlink it 
	if ( access ( pathname, F_OK ) == 0 )
	{
		if( unlink(pathname) == -1 )
		{
			cerr<<"Error in unlinking:"<< strerror(errno);
			return Unres;
		}	
	}
	//trying to unlink non existance file
	ret_unlink = unlink ( pathname );
	
	if ( ret_unlink == 0 )
	{
		cerr << "unlink returns 0 in case of no such file exists";
	    return Fail;
	} 
	if ( errno != ENOENT )
	{
		cerr << "Incorrect error set in errno in case of no such file exists "<<strerror(errno);
		return Fail;
	}
	
	 return Success;
}

Status UnlinkTest::UnlinkIsDirectoryFunc ()
{
	int ret_mkdir;
	int ret_unlink;
	const char *pathname = "a";
	ret_mkdir = mkdir( pathname, 0777 );
	
	if (ret_mkdir == -1)
	{
		cerr << "Unable to creat directory" ;
		return Unres;
	}
	ret_unlink = unlink( pathname );
	
	if (ret_unlink == 0 )
	{
		cerr << "returns 0 in case of path name is directory" ;
		return Fail;
	} 
	if ( errno != EISDIR )
	{
		cout<<"Incorrect error set in errno in case of path name is directory "<< strerror(errno) ;
		return Fail;
	}
	rmdir( pathname );
	
	return Success;
}

Status UnlinkTest::UnlinkTestEmptyPathFunc ()
{
	int ret_unlink;
	const char *pathname = "";
    ret_unlink = unlink ( pathname );
	if (ret_unlink == 0)
	{
		cerr<<"returns 0 in case of empty path name ";
		return Fail;
	}
	if ( errno != ENOENT )
	{
		cout<<"Incorrect error set in errno in case of empty path "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}

Status UnlinkTest::UnlinkTestNegativeAdressFunc ()
{
	const char *pathname =(char *)-1;
	int ret_unlink = unlink ( pathname );
	if ( ret_unlink == 0 )
	{
		cerr<<"returns 0 in case of negative adress ";
		return Fail;
	}
	if ( errno != EFAULT )
	{
		cerr << "Incorrect error set in errno in case of negative adress "<<strerror(errno);
		return Fail; 
	}
	
	return Success;
}

Status UnlinkTest::UnlinkTestPermissionDeniedFunc()
{
	if ( getuid() ==  0 )
	{
		//This test is not for root
		return Success;
	}
	string dirname = "dirname_unlink", filename = "filename_unlink";
	string pathname = dirname + "/" + filename;
	int fd;
	
	if ( mkdir ( dirname.c_str(), 0777 ) == -1 )
	{
		cerr << "Error in creating directory: "<<strerror(errno);
		return Unres;
	}
	
	if ( (fd = open( pathname.c_str(), O_CREAT | O_RDWR, 0777 )) == -1 )
	{
		cerr << "Error in opening and creating file: "<<strerror(errno);
		return Unres;
	}
	
	if ( chmod( dirname.c_str(), 0000) == -1 )
    {
		cerr << "Error in changing mode of directory: "<<strerror(errno);
		return Unres;
	}
	
	if ( unlink( pathname.c_str() ) != -1 )
	{
		cerr << "returns 0 in case of permission denied. ";
		return Fail;
	}
	
	if ( errno != EACCES )
	{
		cerr << "Incorrect error set in errno in case of  permission denied "<<strerror(errno);
		return Fail;
	}
	
	//cleaning up
	if ( chmod( dirname.c_str(), 0777 ) == -1 )
	{
		cerr << "Error in changing mode of directory: "<<strerror(errno);
		return Unres;
	}
	
	if ( unlink( pathname.c_str() ) == -1 )
	{
		cerr << "Error in unlinking file: "<<strerror(errno);
		return Unres;
	}
	
	if ( rmdir( dirname.c_str() ) == -1 )
	{
		cerr << "Error in removing directory: "<<strerror(errno);
		return Unres;
	}
	
	return Success;
}

Status UnlinkTest:: UnlinkTestNormalCaseFunc()
{
	int fd;
	const char *filename = "file_";
	const char *symlink_name = "symlink";
	try
	{
		File file( filename );
		fd = file.GetFileDescriptor();
		
		if ( symlink( file.GetPathname().c_str(), symlink_name ) == -1 )
		{
			cerr << "Error in symlink() : "<<strerror(errno);
			return Fail;
		}
		
		if ( unlink( symlink_name ) == -1 )
		{
			cerr << "Unlink failed: "<<strerror(errno);
			return Fail;
		}
		// Check if file exists
		if( access ( file.GetPathname().c_str(), F_OK ) == 0 )
		{
			cerr << "Unlink failed.";
			return Fail;
		}
		
	}
	catch( Exception ex )
	{
		cerr << ex.GetMessage();
		return Unres;
	}
	return Success;
}
