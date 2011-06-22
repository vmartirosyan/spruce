//      ReadWriteFile.hpp
//      
//      Copyright 2011 Gurgen Suren
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

#ifndef CHMOD_H
#define CHMOD_H

#include "SyscallTest.hpp"

enum ChmodSyscalls
{

	CHMOD_S_IRWXU,
	CHMOD_S_IRUSR,
	CHMOD_S_IWUSR,
	CHMOD_S_IXUSR,
	
	CHMOD_S_IRWXG,
	CHMOD_S_IRGRP,
	CHMOD_S_IWGRP,
	CHMOD_S_IXGRP,
	
	CHMOD_S_IRWXO,
	CHMOD_S_IROTH,
	CHMOD_S_IWOTH,
	CHMOD_S_IXOTH,
	CHMOD_S_ISUID,
	
	ERR_ENOTDIR,
	ERR_ENAMETOOLONG,
	ERR_ENOENT,
	ERR_EACCES,
	ERR_ELOOP,
	ERR_EPERM,
	ERR_EROFS,
	ERR_EFAULT,
	ERR_EIO,
	ERR_EFTYPE


};

class Chmod : public SyscallTest
{			
public:	
	Chmod(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "chmod")
	{			
	}
	virtual ~Chmod() {}	
	Status Chmod_S_IRWXU();	
	Status PermissionsTest(int open_mode);
	Status ErrEfault();

protected:
	virtual int Main(vector<string> args);	
};
#endif
