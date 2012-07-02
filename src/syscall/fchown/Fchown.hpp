//      Chown.hpp
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

#ifndef FCHOWN_H
#define FCHOWN_H

#include "SyscallTest.hpp"
// Operations
enum ChmodSyscalls
{

FCHOWN_CHANGE_OWNER,
FCHOWN_ERR_EBADF,
FCHOWN_ERR_ENOENT
	


};

class Fchown : public SyscallTest
{			
public:	
	Fchown(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "fchown")
	{			
	}
	virtual ~Fchown() {}	
	Status FchownTest();	
	Status FchownFileNotExist();
	Status FchownBadFd();

protected:
	virtual int Main(vector<string> args);	
};
#endif
