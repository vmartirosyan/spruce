//      Lchown.hpp
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
//      MA 02110-1301, USA.ranklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.

#ifndef LCHOWN_H
#define LCHOWN_H

#include "SyscallTest.hpp"
// Operations

/*enum LchmodSyscalls
{
	
	LCHOWN_CHANGE_OWNER,
	LCHOWN_ERR_ENAMETOOLONG,
	LCHOWN_ERR_ENOENT,
	LCHOWN_ERR_ENOTDIR,
	LCHOWN_SYMLINK


};
*/
class Lchown : public SyscallTest
{			
public:	
	Lchown(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "lchown")
	{			
	}
	virtual ~Lchown() {}	
	Status LchownTest();	
	Status LchownTooLongPath();
	Status LchownFileNotExist();
	Status LchownIsNotDirectory();
	Status LchownSymlinktest();

protected:
	virtual int Main(vector<string> args);	
};
#endif
