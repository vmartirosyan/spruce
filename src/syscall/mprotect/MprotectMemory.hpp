//      DupFileDescriptor.hpp
//      
//		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//      	Ruzanna Karakozova <r.karakozova@gmail.com>
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

#ifndef MPROTECT_MEMORY_H
#define MPROTECT_MEMORY_H

#include "SyscallTest.hpp"
#include <sys/stat.h>

// Operations
enum MprotectMemorySyscalls
{
	Mprotect,
	MprotectErrEACCES,
	MprotectErrENOMEM
};

class MprotectMemoryTest: public SyscallTest
{
public:
	MprotectMemoryTest(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "mmap")
	{			
	}
	
	virtual ~MprotectMemoryTest() {}
	
protected:
	virtual int Main(vector<string> args);
	
private:
	int MprotectTest(vector<string> args);
	int MprotectErrEACCESTest(vector<string> args);
	int MprotectErrENOMEMTest(vector<string> args);
}; 
#endif /*MPROTECT_MEMORY_H*/
 
