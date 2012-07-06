//      Chmod.hpp
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

#ifndef FCHMOD_H
#define FCHMOD_H

#include "SyscallTest.hpp"
// Operations
enum FchmodSyscalls
{
	FCHMOD_S_IRWXU,
	FCHMOD_S_IRUSR,
	FCHMOD_S_IWUSR,
	FCHMOD_S_IXUSR,
	
	FCHMOD_S_IRWXG,
	FCHMOD_S_IRGRP,
	FCHMOD_S_IWGRP,
	FCHMOD_S_IXGRP,
	
	FCHMOD_S_IRWXO,
	FCHMOD_S_IROTH,
	FCHMOD_S_IWOTH,
	FCHMOD_S_IXOTH,
	FCHMOD_S_ISUID,
	FCHMOD_ERR_BADFD
	



};

class Fchmod : public SyscallTest
{			
public:	
	Fchmod(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "fchmod")
	{			
	}
	virtual ~Fchmod() {}	
	
	Status PermissionsTest(mode_t open_mode);
	Status FchmodBadFileDesc();


protected:
	virtual int Main(vector<string> args);	
};
#endif
