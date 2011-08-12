//      ReadvWritev.cpp
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

#ifndef READVWRITEV_H
#define READVWRITEV_H

#include <sys/uio.h>
#include "SyscallTest.hpp"
#include "File.hpp"

// Operations
enum ReadvWritevOps
{
	ReadvWritevBasicOp1,
	ReadvEagainErrorOp,
	ReadvBadfdErrorOp1,
	ReadvBadfdErrorOp2,
	ReadvEfaultErrorOp,
	ReadvEinvalErrorOp1,
	ReadvEinvalErrorOp2,
	ReadvEinvalErrorOp3,
	ReadvEinvalErrorOp4,
	ReadvIsdirErrorOp,
	
	WritevBadfdErrorOp1,
	WritevBadfdErrorOp2,
	WritevEfaultErrorOp,
	WritevEagainErrorOp,
	WritevEinvalErrorOp1,
	WritevEinvalErrorOp2,
	WritevEinvalErrorOp3,
};

class ReadvWritev : public SyscallTest
{			
public:	
	ReadvWritev(Mode mode, int operation, string arguments = "")
		:SyscallTest(mode, operation, arguments, "readv_writev")
	{}
	
	virtual ~ReadvWritev() {}	
	
	Status ReadvWritevBasicFunc1();			//basic functionality
	Status ReadvEagainErrorFunc();			//initiating of EAGAIN error for readv
	Status ReadvBadfdErrorFunc1();			//initiating of EBADF error for readv
	Status ReadvBadfdErrorFunc2();			//initiating of EBADF error for readv
	Status ReadvEfaultErrorFunc();			//initiating of EFAULT error for readv
	Status ReadvEinvalErrorFunc1();			//initiating of EINVAL error for readv
	Status ReadvEinvalErrorFunc2();			//initiating of EINVAL error for readv
	Status ReadvEinvalErrorFunc3();			//initiating of EINVAL error for readv
	Status ReadvEinvalErrorFunc4();			//initiating of EINVAL error for readv
	Status ReadvIsdirErrorFunc();			//initiating of EISDIR error for readv
	
	Status WritevBadfdErrorFunc1();			//initiating of EBADF error for writev
	Status WritevBadfdErrorFunc2();			//initiating of EBADF error for writev
	Status WritevEfaultErrorFunc();			//initiating of EFAULT error for writev
	Status WritevEagainErrorFunc();			//initiating of EAGAIN error for writev
	Status WritevEinvalErrorFunc1();		//initiating of EINVAL error for writev
	Status WritevEinvalErrorFunc2();		//initiating of EINVAL error for writev
	Status WritevEinvalErrorFunc3();		//initiating of EINVAL error for writev
	
protected:
	virtual int Main(vector<string> args);

};
#endif /* READVWRITEV_H */
