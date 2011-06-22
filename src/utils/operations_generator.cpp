//      operations_generator.cpp
//      
//      Copyright 2011 Vahram Martirosyan <vmartirosyan@gmail.com>
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

/*
 * This programm is created to generate the contents of the file /include/operations.hpp.
 * Must be executed during the build process, before all the other parts of the system are built.
*/

#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
using std::ifstream;
using std::ofstream;
using std::iostream;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::string;
using std::stringstream;

#include "UnixCommand.hpp"

int Usage(char ** argv);
vector<string> GetHeaderFiles(const string & root);
void ProcessFile(string file);

vector<string> Operations;

int main(int argc, char ** argv)
{
	if ( argc < 2 )
		return Usage(argv);
	cout << " ++ Generating operations header file..." << endl;
	rename("/home/vmartirosyan/workspace/build/include/operations.hpp.old", "/home/vmartirosyan/workspace/build/include/operations.hpp");
	
	vector<string> files = GetHeaderFiles(static_cast<string>(argv[1]));
	
	for (vector<string>::iterator i = files.begin(); i != files.end(); ++i)
		ProcessFile(*i);
	
	
	//ProcessFile("/home/vmartirosyan/workspace/spruce/src/fs/ext4fs/ioctl_test.hpp");
	return 0;
}

int Usage(char ** argv)
{
	cerr << "Usage: " << argv[0] << " <source_tree>" << endl;
	return -1;
}
 
vector<string> GetHeaderFiles(const string & root)
{
	vector<string> files;
	
	vector<string> args;
	args.push_back(root.c_str());
	args.push_back("-name");
	args.push_back("*.hpp");
	
	UnixCommand cmd("find");
	
	ProcessResult * res = NULL;
	
	if ( ( res = cmd.Execute(args) ) )
	{
		string file;
		string FindOutput = res->GetOutput();
		
		stringstream s(FindOutput);
		
		while (s.good())
		{
			s >> file;
			cout << file << endl;
		}
		//cout << "Files found\n" << FindOutput << endl;
		
		delete res;
	}
	
	return files;
}

void ProcessFile(string file)
{
	ifstream inf(file.c_str());
	ofstream of((file + ".new").c_str());
	/* States
	 * 0 - Initial
	 * 1 - Operations enum found
	 * 2 - Listing operations
	 * 3 - Out of operations
	 */
	int state = 0;
	
	string line;
	char buf[256];
	
	while ( inf.good() )
	{
		inf.getline(buf, 256);
		line = buf;
		//cout << line << endl;
		string operation;
		switch (state)
		{
			case 0:
				if ( line.find("// Operations") != string::npos )
				{
					state = 1;
					cout << "Comment detected!!!!" << endl;
				}
				break;
			case 1:
				if ( line.find("{") != string::npos )
				{
					
					cout << "Entering enum!!!!" << endl;
					
					state = 2;
				}
				line = "// " + line;
				break;
			case 2:
				if ( line.find("}") == string::npos )
				{
					stringstream s(line);
					s >> operation;
					Operations.push_back(operation);
					cout << operation << endl;
				}
				else
				{
					cout << "Leaving enum;" << endl;
					state = 3;
				}
				line = "// " + line;
				break;
		}
		of << line << endl;
	}
	inf.close();
	of.close();
	
	if ( state == 3 ) // This means that there was operations enum in this file. Replace it with the *.new one
	{
		rename((file+".new").c_str(), file.c_str());
	}
}
