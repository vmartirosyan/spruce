//      MmapMemory.hpp
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

#ifndef MMAP_MEMORY_H
#define MMAP_MEMORY_H

#include "SyscallTest.hpp"
#include <sys/stat.h>

// Operations 
enum MmapMemorySyscalls
{
	Mmap,
	MmapErrEINVAL,
	MmapErrEBADF,
	MmapErrEACCES,
	Munmap,
	MunmapErr,
	Mremap,
	MremapErrEINVAL,
	Mmap2,
	Mmap2ErrEINVAL,
	Mmap2ErrEBADF,
	Mmap2ErrEACCES,	
};

class MmapMemoryTest: public SyscallTest
{
public:
	MmapMemoryTest(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "mmap")
	{			
	}
	
	virtual ~MmapMemoryTest() {}
	
protected:
	virtual int Main(vector<string> args);
	
private:
    int MapTest(void * (*map)(void *, size_t, int, int, int, off_t));
	int MmapTest(vector<string> args);
	int MmapErrEINVALTest(vector<string> args);
	int MmapErrEBADFTest(vector<string> args);
	int MmapErrEACCESTest(vector<string> args);
	int MunmapTest(vector<string> args);
	int MunmapErrTest(vector<string> args);
	int MremapTest(vector<string> args);
	int MremapErrENOMEMTest(vector<string> args);
	int MremapErrEINVALTest(vector<string> args);
	int Mmap2Test(vector<string> args);
	int Mmap2ErrEINVALTest(vector<string> args);
	int Mmap2ErrEBADFTest(vector<string> args);
	int Mmap2ErrEACCESTest(vector<string> args);
};

#endif /*MMAP_MEMORY_H*/
