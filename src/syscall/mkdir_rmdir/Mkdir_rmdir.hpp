//      Mkdir_rmdir.hpp
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//			Giorgi Gulabyan <gulabyang@gmail.com>
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

#ifndef MKDIR_RMDIR_H
#define MKDIR_RMDIR_H

#include "SyscallTest.hpp"
// Operations
/*enum Mkdir_rmdirOperations
{
	MKDIR_RMDIR_NORMAL, // +
	MKDIR_EACCES, // +
	MKDIR_EEXIST, // +
	MKDIR_EFAULT, // +
	MKDIR_ELOOP,
	MKDIR_EMLINK,
	MKDIR_ENAMETOOLONG, // +
	MKDIR_ENOENT, // +
	MKDIR_ENOMEM, // -
	MKDIR_ENOSPC, // -
	MKDIR_ENOTDIR, // +
	MKDIR_EPERM,
	MKDIR_EROFS, // -
	RMDIR_EACCES, // +
	RMDIR_EBUSY,
	RMDIR_EFAULT, // +
	RMDIR_EINVAL, // +
	RMDIR_ELOOP,
	RMDIR_ENAMETOOLONG, // +
	RMDIR_ENOENT, // +
	RMDIR_ENOMEM, // -
	RMDIR_ENOTDIR, // +
	RMDIR_ENOTEMPTY, // +	
	RMDIR_EPERM,
	RMDIR_EROFS // -
};*/

class Mkdir_rmdir : public SyscallTest
{			
public:	
	Mkdir_rmdir(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "Mkdir_rmdir")
	{			
		long size;
		_cwd = NULL;
		size = pathconf(".", _PC_PATH_MAX);

		if ((_cwd = (char *)malloc((size_t)size)) != NULL)
			_cwd = getcwd(_cwd, (size_t)size);
	}
	virtual ~Mkdir_rmdir() 
	{
		free(_cwd);
	}
	Status mkdirRmdirNormal();
	
	Status mkdirNoAccess();
	Status mkdirFault();
	Status mkdirExists();	
	Status mkdirNotDir();
	Status mkdirPathNotEnt();	
	Status mkdirNameTooLong();

	Status rmdirNoAccess();
	Status rmdirFault();
	Status rmdirNotEmpty();
	Status rmdirInval();
	Status rmdirNotDir();
	Status rmdirPathNotEnt();
	Status rmdirNameTooLong();

protected:
	virtual int Main(vector<string> args);	
	char *_cwd;
};
#endif
