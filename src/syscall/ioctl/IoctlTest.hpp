//      IoctlTest.hpp
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//			Suren Grigoryan <suren.grigoryan@gmail.com>
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

#ifndef TEST_IOCTL_H
#define TEST_IOCTL_H

#include "SyscallTest.hpp"

// Operations
/*enum IoctlSyscalls
{
    IOCTL_INVALID_FD,
    IOCTL_INVALID_ARGP,
	IOCTL_SETGET_VERSION,
};*/

class IoctlTest : public SyscallTest
{
	public:
		IoctlTest(Mode mode, int operation, string arguments = "") :
			SyscallTest(mode, operation, arguments, "ioctl"){}
		virtual ~IoctlTest() {}
        
        Status SetGetFlags(int i);
        Status SetGetVersion();
        Status InvalidFD();
        Status InvalidArgp();
	protected:
		virtual int Main(vector<string> args);
};

#endif /* TEST_IOCTL_H */
