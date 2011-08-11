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




int UnlinkTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case UnlinkTestTooLongPath:
				return UnlinkTestTooLongPathFunction ();
			case UnlinkTestNormalFile:
			   return UnlinkTestNormalFileFunction();
			case UnlinkTestIsNotDirectory:
				return UnlinkTestIsNotDirectoryFunction();
			case UnlinkTestNoSuchFile :
				return UnlinkTestNoSuchFileFunction();
			case UnlinkIsDirectory:
				return UnlinkIsDirectoryFunction(); 
			case UnlinkTestEmptyPath:
				return UnlinkTestEmptyPathFunction();
			case UnlinkTestNegativeAdress:
				 return UnlinkTestNegativeAdressFunction();
			case UnlinkTestPermissionDenied:
				 return UnlinkTestPermissionDeniedFunction();
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status UnlinkTest::UnlinkTestNormalFileFunction ()
{
	
	int ret_unlink;
	const char *pathname = "file.txt";
	
	if( creat( pathname, 0775 ) == -1 )
	{
		cerr << "Unable to creat file "<< strerror(errno) ;
		return Unres;
	}
   
    ret_unlink = unlink ( pathname );
	if ( ret_unlink == -1)
	{
		cerr<< "Error occured during unlinking normal file" << strerror(errno);
		return Unres;
	}
	;
	if ( open( pathname, O_RDWR )!= -1 )
	{
		cerr<< " Error: opened unlinked file ";
		return Fail;
	}
	
	return Success;

}

Status UnlinkTest::UnlinkTestTooLongPathFunction ()
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

Status UnlinkTest::UnlinkTestIsNotDirectoryFunction ()
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

Status UnlinkTest::UnlinkTestNoSuchFileFunction ()
{
	int ret_unlink;
	int fd;
	const char *pathname ="unlink.txt";
	//checking: if file exists unlink it 
	if ( fd = open( pathname, 0777 ) >= 0 )
	{
		if ( close( fd ) == -1 )
		{
			cerr << "Error in closing file :" <<strerror(errno);
			return Unres;
		}
		if( unlink(pathname) == -1 )
		{
			cerr<<"Error in unlinking:"<< strerror(errno);
			return Fail;
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

Status UnlinkTest::UnlinkIsDirectoryFunction ()
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

Status UnlinkTest::UnlinkTestEmptyPathFunction ()
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

Status UnlinkTest::UnlinkTestNegativeAdressFunction ()
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

Status UnlinkTest::UnlinkTestPermissionDeniedFunction()
{
	int ret_unlink;
	const char *pathname = "/dev/loop1";
	
	ret_unlink = unlink ( pathname );
	
	if ( ret_unlink == 0 )
	{
		cerr << " returns 0 in case of permission denied ";
		return Fail;
	}
	
	if ( errno != EACCES )
	{
		cerr << "Incorrect error set in errno in case of permission denied "<<strerror(errno);
		return Fail;
	}

	return Success;
	
}
