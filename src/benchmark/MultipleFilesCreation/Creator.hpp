//      Creator.hpp
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

#ifndef CREATOR_H
#define CREATOR_H

#include "BenchmarkTest.hpp"

enum CreatorOperations
{
	MultipleFilesCreation
};

class CreatorTest : public BenchmarkTest
{
public:
		
	CreatorTest(Mode m, int op, string a) : BenchmarkTest (m, op, a)
	{
		filesNumber = "10";
		fileSize = "1M";
	}
		
	~CreatorTest()
	{
	}
	
	int Main(vector<string>);
private:
	string filesNumber;
	string fileSize;
	Status MultipleFilesCreationFunc();
	vector<string> CreateArguments();
};

#endif /* CREATOR_H */
