//      Chroot.hpp
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

#ifndef CHROOT_H
#define CHROOT_H

#include "SyscallTest.hpp"
// Operations
/*enum ChrootOperations
{
	CHROOT_ERR_EACCES,
	CHROOT_ERR_EFAULT,
	CHROOT_ERR_ELOOP,
	CHROOT_ERR_ENAMETOOLONG,
	CHROOT_ERR_ENOENT,
	CHROOT_ERR_ENOTDIR,
	CHROOT_ERR_EPERM,
	CHROOT_NORMAL_FUNC
};*/

class Chroot : public SyscallTest
{			
public:	
	Chroot(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "chroot"){}
	virtual ~Chroot() {}	
	Status chrootTooLongPath();
	Status chrootFault();
	Status chrootFileNotExist();
	Status chrootIsNotDirectory();
	Status chrootLoopInSymLink();
	Status chrootNoAcces();
	Status chrootNoPerm();
	Status chrootNormalFunc();

protected:
	virtual int Main(vector<string> args);	
	char *_cwd;
};
#endif
