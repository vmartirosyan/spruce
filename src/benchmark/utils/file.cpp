//      file.cpp
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

#include <iostream>
#include <fstream>
#include <string.h>
#include <cstdlib>

using namespace std;

// returns the suffix of the string: 
// c = 1, w = 2, b = 512, kB = 1000, K = 1024, MB = 1000*1000, 
// M = 1024*1024, xM = M GB = 1000*1000*1000, G = 1024*1024*1024
string getSuffix(const char *str)
{
	string s = "";
	
	for (unsigned int i = 0; i < strlen(str); ++i)
		if (!isdigit(str[i]))
			s += str[i];
	return s;
}

long long getMeasure(const string &suffix)
{
	long long bytes = 0;
	if (suffix == "c")
		bytes += 1;
	else if (suffix == "w")
		bytes += 2;
	else if (suffix == "b")
		bytes += 512;
	else if (suffix == "kB")
		bytes += 1000;
	else if (suffix == "K")
		bytes += 1024;
	else if (suffix == "MB")
		bytes += 1000 * 1000;
	else if (suffix == "M")
		bytes += 1024 * 1024;
	else if (suffix == "xM")
		bytes += 1024 * 1024;
	else if (suffix == "GB")
		bytes += 1000 * 1000 * 1000;
	else if (suffix == "G")
		bytes += 1024 * 1024 * 1024;
		
	return bytes;
}

int main(int argc, char *argv[])
{
	long long n = 0, bytes = 0;
	char *filename;
	
	if (argc >= 2)
	{
		filename = argv[1];
	}
	
	if (argc >= 3)
	{
		string suffix = getSuffix(argv[2]);
		bytes = getMeasure(suffix);
	
		n = atoi(argv[2]);
		
		bytes *= n;
	}
	
	char *buffer = new char[bytes];
	memset(buffer, '0', bytes);
	
	ofstream fout(filename);
	
	fout << buffer;
	
	delete buffer;
	
	return 0;
}
