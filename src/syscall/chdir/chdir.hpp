//      chdir.hpp
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

#ifndef CHDIR_H
#define CHDIR_H

#include "SyscallTest.hpp"
// Operations
enum ChdirOperations
{

	CHDIR_S_IRWXU,
	CHDIR_S_IRUSR,
	CHDIR_S_IWUSR,
	CHDIR_S_IXUSR,
	
	CHDIR_S_IRWXG,
	CHDIR_S_IRGRP,
	CHDIR_S_IWGRP,
	CHDIR_S_IXGRP,
	
	CHDIR_S_IRWXO,
	CHDIR_S_IROTH,
	CHDIR_S_IWOTH,
	CHDIR_S_IXOTH,
	CHDIR_S_ISUID,
	
	CHDIR_ERR_ENOTDIR,
	CHDIR_ERR_ENAMETOOLONG,
	CHDIR_ERR_ENOENT,
	CHDIR_ERR_EACCES,
	CHDIR_ERR_ELOOP,
	CHDIR_ERR_EPERM,
	CHDIR_ERR_EROFS,
	CHDIR_ERR_EFAULT,
	CHDIR_ERR_EIO,
	CHDIR_ERR_EFTYPE



};

class Chdir : public SyscallTest
{			
public:	
	Chdir(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "chmod")
	{			
	}
	virtual ~Chdir() {}	
	/*Status Chmod_S_IRWXU();	
	Status PermissionsTest(int open_mode);
	Status ErrEfault();
	Status ChmodTooLongPath();
	Status ChmodFileNotExist();
	Status ChmodIsNotDirectory();*/

protected:
	virtual int Main(vector<string> args);	
};
#endif
