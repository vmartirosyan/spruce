//      Chmod.cpp
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//			Gurgen Torozyan <gurgen.torozyan@gmail.com>
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

#include <Fchmod.hpp>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "File.hpp"



int Fchmod::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case FCHMOD_S_IRWXU:
				return PermissionsTest(S_IRWXU);
	        case FCHMOD_S_IRUSR:
				return PermissionsTest(S_IRUSR);
			case FCHMOD_S_IWUSR:
				return PermissionsTest(S_IWUSR);
			case FCHMOD_S_IXUSR:
				return PermissionsTest(S_IXUSR);
				
			case FCHMOD_S_IRWXG:
				return PermissionsTest(S_IRWXG);
			case FCHMOD_S_IRGRP:
				return PermissionsTest(S_IRGRP);
			case FCHMOD_S_IWGRP:
				return PermissionsTest(S_IWGRP);
			case FCHMOD_S_IXGRP:
				return PermissionsTest(S_IXGRP);
				
			case FCHMOD_S_IRWXO:
				return PermissionsTest(S_IRWXO);
			case FCHMOD_S_IROTH:
				return PermissionsTest(S_IROTH);
			case FCHMOD_S_IWOTH:
				return PermissionsTest(S_IWOTH);
			case FCHMOD_S_IXOTH:
				return PermissionsTest(S_IXOTH);
			case FCHMOD_S_ISUID:
				return PermissionsTest(S_ISUID);
			
			case FCHMOD_ERR_BADFD:
				return FchmodBadFileDesc();
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status Fchmod::FchmodBadFileDesc()
{
	int ret_fchmod;
	
	ret_fchmod = fchmod(-1, S_IRGRP);
	
	if(ret_fchmod != -1)
		{
			cerr << "FChmod should return -1 when the file descriptor is not vaild  "<<strerror(errno);
			return Fail;
		}
		
		else 
		{
			
			if(errno != EBADF)
			{
				
				cerr << "Incorrect error set in errno in case of bad file descriptor "<<strerror(errno);
				return Fail;
			}
			
		}
	
}




Status Fchmod::PermissionsTest(int open_mode)
{

	const char *path="FchmodTest.txt";
	int ret_fchmod;
	struct stat mode;
	int fd;
		
	try
	{
		File file(path, open_mode);

		fd = file.GetFileDescriptor();
		
		ret_fchmod = fchmod(fd, open_mode);
		
		if(ret_fchmod == -1)
		{
			cerr << "Chmod  failed: Aborting test "<<strerror(errno);
			return Unres;
		}
		
		int ret = stat(path, &mode);
		
		if( ret == -1 )
		{
				cerr << "stat failed: Aborting test "<<strerror(errno);
			return Unres;
		}
		
		if((mode.st_mode & 4095) !=  open_mode)
		{
			cerr<<"Chmode failed ";
			return Fail;
		}
	
	
	return Success;


	}
	
	
	
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Fail;
	}
}

	

