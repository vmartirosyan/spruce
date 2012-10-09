/**
//      Deleter.hpp
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
*/

#ifndef DELETER_H
#define DELETER_H

#include <BenchmarkTest.hpp>

// Operations
/*enum Deleter_Operations
{
	MultipleFilesDeletion
};*/

class DeleterTest : public BenchmarkTest
{
public:
		
	DeleterTest(Mode m, int op, string a) : BenchmarkTest (m, op, a)
	{
		vector<string> arguments = ParseArguments(a);
		
		dirName = arguments[0];
	}
		
	~DeleterTest() {}
	
	int Main(vector<string>);
private:
	string dirName;
	Status MultipleFilesDeletionFunc();
	vector<string> CreateArguments();
};

#endif /* DELETER_H */
