//      PreadPwrite.cpp
//      
// 		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author: 
// 			Tigran Piloyan <tigran.piloyan@gmail.com>
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

#ifndef PREADPWRITE_H
#define PREADPWRITE_H

#include <sys/uio.h>
#include "SyscallTest.hpp"
#include "File.hpp"

// Operations
/*enum PreadPwriteOps
{
	PreadPwriteBasicOp1,
	PreadPwriteBasicOp2,
	
	PreadEspipeErrorOp,
	PreadBadfdErrorOp1,
	PreadBadfdErrorOp2,
	PreadBadfdErrorOp3,
	PreadEfaultErrorOp,
	PreadEinvalErrorOp1,
	PreadEinvalErrorOp2,
	PreadIsdirErrorOp,
	
	PwriteBadfdErrorOp1,
	PwriteBadfdErrorOp2,
	PwriteBadfdErrorOp3,
	PwriteEfaultErrorOp,
	PwriteEspipeErrorOp,
	PwriteEinvalErrorOp1,
	PwriteEinvalErrorOp2,
	PwriteEinvalErrorOp3,
	
};*/

class PreadPwrite : public SyscallTest
{			
public:	
	PreadPwrite(Mode mode, int operation, string arguments = "")
		:SyscallTest(mode, operation, arguments, "pread_pwrite")
	{}
	
	virtual ~PreadPwrite() {}	
	
	Status PreadPwriteBasicFunc1();				//basic functionality
	Status PreadPwriteBasicFunc2();				//basic functionality
	
	Status PreadEspipeErrorFunc();			//initiating of ESPIPE error for readv
	Status PreadBadfdErrorFunc1();			//initiating of EBADF error for readv
	Status PreadBadfdErrorFunc2();			//initiating of EBADF error for readv
	Status PreadBadfdErrorFunc3();			//initiating of EBADF error for readv
	Status PreadEfaultErrorFunc();			//initiating of EFAULT error for readv
	Status PreadEinvalErrorFunc1();			//initiating of EINVAL error for readv
	Status PreadEinvalErrorFunc2();			//initiating of EINVAL error for readv
	Status PreadIsdirErrorFunc();			//initiating of EISDIR error for readv
	
	Status PwriteBadfdErrorFunc1();			//initiating of EBADF error for writev
	Status PwriteBadfdErrorFunc2();			//initiating of EBADF error for writev
	Status PwriteBadfdErrorFunc3();			//initiating of EBADF error for writev
	Status PwriteEfaultErrorFunc();			//initiating of EFAULT error for writev
	Status PwriteEspipeErrorFunc();			//initiating of ESPIPE error for writev
	Status PwriteEinvalErrorFunc1();		//initiating of EINVAL error for writev
	Status PwriteEinvalErrorFunc2();		//initiating of EINVAL error for writev
	Status PwriteEinvalErrorFunc3();		//initiating of EINVAL error for writev
	
protected:
	virtual int Main(vector<string> args);

};
#endif /* PREADPWRITE_H */
