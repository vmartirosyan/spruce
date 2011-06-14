//      LargeFile.hpp
//      
//      Copyright 2011 Eduard Bagrov <ebagrov@gmail.com>
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

#include "BenchmarkTest.hpp"

enum LargeFileOperations
{
	LargeFileCreation
};

class LargeFileTest : public BenchmarkTest
{
public:
		
	LargeFileTest(Mode m, int op, string a) : BenchmarkTest (m, op, a)
	{
		fileSize = "150M";
	}
		
	~LargeFileTest()
	{
	}
	
	int Main(vector<string>);
private:
	string fileSize;
	Status LargeFileCreationFunc();
	string CreateCommand();
};

#endif
