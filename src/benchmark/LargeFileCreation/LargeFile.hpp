//      LargeFile.hpp
//      
// 		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//
//      Author: Eduard Bagrov <ebagrov@gmail.com>
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

#ifndef LARGE_FILE_CREATOR_H
#define LARGE_FILE_CREATOR_H

#include <BenchmarkTest.hpp>

// Operations
/*enum LargeFileOperations
{
	LargeFileCreation
};*/

class LargeFileTest : public BenchmarkTest
{
public:
		
	LargeFileTest(Mode m, int op, string a) : BenchmarkTest (m, op, a)
	{
		vector<string> arguments = ParseArguments(a);
		
		filename = arguments[0];
		fileSize = arguments[1];
	}
	
	~LargeFileTest()
	{
	}
	
	int Main(vector<string>);
private:
	string filename;
	string fileSize;
	Status LargeFileCreationFunc();
	vector<string> CreateArguments();
};

#endif
