//      Getcwd.hpp
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

#ifndef GETCWD_H
#define GETCWD_H

#include "SyscallTest.hpp"
// Operations
/*enum GetcwdOperations
{
	GETCWD_NORMAL_FUNC,
	GETCWD_ERR_EACCES,
	GETCWD_ERR_EFAULT,
	GETCWD_ERR_EINVAL,
	GETCWD_ERR_ENOENT,
	GETCWD_ERR_ERANGE
};*/

class Getcwd : public SyscallTest
{			
public:	
	Getcwd(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "getcwd"){}
	virtual ~Getcwd() {}	
	Status GetcwdNormalFunc();
	Status GetcwdNoAcces();
	Status GetcwdFault();
	Status GetcwdIncorrectValue();
	Status GetcwdDirUnlinked();
	Status GetcwdRange();
	

protected:
	virtual int Main(vector<string> args);	
};
#endif
