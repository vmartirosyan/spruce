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
/*enum ChdirOperations
{
	CHDIR_ERR_EACCES,
	CHDIR_ERR_EFAULT,
	CHDIR_ERR_EIO,
	CHDIR_ERR_ELOOP,
	CHDIR_ERR_ENAMETOOLONG,
	CHDIR_ERR_ENOENT,
	CHDIR_ERR_ENOMEM,
	CHDIR_ERR_ENOTDIR,
	CHDIR_ERR_EBADF,
	CHDIR_NORMAL_FUNC

};*/

class Chdir : public SyscallTest
{			
public:	
	Chdir(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "chdir")
	{			
		long size;
		_cwd = NULL;
		size = pathconf(".", _PC_PATH_MAX);

		if ((_cwd = (char *)malloc((size_t)size)) != NULL)
			_cwd = getcwd(_cwd, (size_t)size);
	}
	virtual ~Chdir() 
	{
		free(_cwd);
	}	
	Status ChdirTooLongPath();
	Status ChdirFileNotExist();
	Status ChdirIsNotDirectory();
	Status ChdirNormalFunc();
	Status ChdirLoopInSymLink();
	Status ChdirNoAcces();

protected:
	virtual int Main(vector<string> args);	
	char *_cwd;
};
#endif
