//      DupFileDescriptor.hpp
//
//      Copyright 2011 Shahzadyan Khachik <qwerity@gmail.com>
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

#ifndef FCNTL_H
#define FCNTL_H

#include <fcntl.h>
#include "SyscallTest.hpp"

// Operations
enum fnctlFDSyscalls
{
	fcntlFDGetSetFileDescriptorFlags,
	fcntlFDGetSetFileStatusFlags,
	fcntlFDGetSetFileStatusFlagsIgnore,
	fcntlFDGetSetFileStatusFlagsIgnoreRDONLY
};

class fcntlFD : public SyscallTest
{
	public:
		fcntlFD(Mode mode, int operation, string arguments = "") :
			SyscallTest(mode, operation, arguments, "fcntl"){}
		virtual ~fcntlFD() {}

	// Test Functions
		Status get_setFileDescriptorFlags();
		Status get_setFileStatusFlags();
		Status get_setFileStatusFlagsIgnore();
		Status get_setFileStatusFlagsIgnoreRDONLY();

	protected:
		virtual int Main(vector<string> args);
};

#endif /* FCNTL_H */
