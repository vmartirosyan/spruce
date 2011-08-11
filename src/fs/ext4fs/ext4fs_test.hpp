//      ext4fs_test.hpp
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//			Vahram Martirosyan <vmartirosyan@gmail.com>
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

#ifndef EXT4FS_TEST_H
#define EXT4FS_TEST_H

#include "test.hpp"

class Ext4fsTestResult : public TestResult
{
public:
	Ext4fsTestResult(ProcessResult pr, int op, string args):
		TestResult(pr, op, args) {}
};

class Ext4fsTest : public Test
{
public:
	Ext4fsTest(Mode m, int op, string a):
		Test(m, op, a)
	{		
	}
};

#endif /* EXT4FS_TEST_H */
