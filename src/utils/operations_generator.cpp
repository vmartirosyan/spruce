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
bool GenerateOperationsHeader(string);

vector<string> Operations;

int main(int argc, char ** argv)
{
	if ( argc < 2 )
		return Usage(argv);
	cout << " ++ Generating operations header file..." << endl;
	//rename("/home/vmartirosyan/workspace/build/include/operations.hpp.old", "/home/vmartirosyan/workspace/build/include/operations.hpp");
	
	// Find all the header files in the source tree
	vector<string> files = GetHeaderFiles(static_cast<string>(argv[1]));
	
	// Procses each of them making the original enums comments 
	// Also collect all the operation enum values into the Operations vector
	for (vector<string>::iterator i = files.begin(); i != files.end(); ++i)
		ProcessFile(*i);
	
	// Generate the operations.hpp file...
	if (! GenerateOperationsHeader(static_cast<string>(argv[1])) )
		return -1;
	
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
			files.push_back(file);
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
				}
				break;
			case 1:
				if ( line.find("{") != string::npos )
				{						
					state = 2;
				}
				line = "// " + line;
				break;
			case 2:
				if ( line.find("}") == string::npos )
				{
					stringstream s(line);
					s >> operation;
					if ( operation == "//" )
						s >> operation;
					else
						line = "// " + line;
						
					if ( operation == "" )
						break;
						
					if ( operation[operation.size() - 1] == ',' )
						operation = operation.substr(0, operation.size() - 1);
						
					Operations.push_back(operation);
					cout << operation << endl;
				}
				else
				{
					cout << "Leaving enum;" << endl;
					state = 3;
					line = "// " + line;
				}
				
				break;
		}
		of << line << endl;
	}
	inf.close();	
	
	if ( state == 3 ) // This means that there was operations enum in this file. Replace it with the *.new one
	{
		of.close();
		rename((file+".new").c_str(), file.c_str());
	}
}

bool GenerateOperationsHeader(string root)
{
	string FileContents = "//      operations.hpp \n\
//      \n\
//      Copyright 2011 Vahram Martirosyan <vmartirosyan@gmail.com>\n\
//      \n\
//      This program is free software; you can redistribute it and/or modify \n\
//      it under the terms of the GNU General Public License as published by \n\
//      the Free Software Foundation; either version 2 of the License, or \n\
//      (at your option) any later version. \n\
//      \n\
//      This program is distributed in the hope that it will be useful, \n\
//      but WITHOUT ANY WARRANTY; without even the implied warranty of \n\
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \n\
//      GNU General Public License for more details.\n\
//      \n\
//      You should have received a copy of the GNU General Public License \n\
//      along with this program; if not, write to the Free Software \n\
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, \n\
//      MA 02110-1301, USA. \n\
\n\
#ifndef OPERATIONS_HPP \n\
#define OPERATIONS_HPP \n\
 \n\
#include <string> \n\
#include <map> \n\
 \n\
using std::string; \n\
using std::map; \n\
\n\
enum Operations \n\
{ \n\
	%ENUM% \n\
}; \n\
\n\
class Operation \n\
{ \n\
private: \n\
	static map<string, int> MapToInt; \n\
	static map<int, string> MapToString; \n\
public: \n\
	static string ToString( Operations ) { %TO_STRING% } \n\
	static Operations Parse( string ) { %PARSE% } \n\
	friend void InitMaps(); \n\
}; \n\
\n\
inline void InitMaps() \n\
{ \n\
	%INIT_MAPS% \n\
} \n\
\n\
#endif /* OPERATIONS_HPP */ \n\
\n";
	cout << "++ generating file " << (root + "/../include/operations.hpp") << endl;

	ofstream of ((root + "/../include/operations.hpp").c_str());
	
	string EnumContents = "";
	
	for ( vector<string>::iterator i = Operations.begin(); i != Operations.end(); ++i )
	{
		EnumContents += *i;
		EnumContents += ",\n";
	}
	
	// Generate the enum contents
	size_t EnumStart = FileContents.find("%ENUM%");
	
	if ( EnumStart == string::npos )
	{
		cerr << "Cannot find %ENUM% in file contents" << endl;
		return false;
	}
	
	FileContents = FileContents.replace(EnumStart, 	6, EnumContents);
	
	// Generate the ToString method body
	size_t ToStringStart = FileContents.find("%TO_STRING%");
	
	if ( ToStringStart == string::npos )
	{
		cerr << "Cannot find %TO_STRING% in file contents" << endl;
		return false;
	}
	
	FileContents = FileContents.replace(ToStringStart, 11, "return \"\";");
	
	// Generate the Parse method body
	size_t ParseStart = FileContents.find("%PARSE%");
	
	if ( ParseStart == string::npos )
	{
		cerr << "Cannot find %PARSE% in file contents" << endl;
		return false;
	}
	
	FileContents = FileContents.replace(ParseStart, 7, "return (Operations)0;");
	
	// Generate the InitMaps body
	size_t InitMapsStart = FileContents.find("%INIT_MAPS%");
	
	if ( InitMapsStart == string::npos )
	{
		cerr << "Cannot find %INIT_MAPS% in file contents" << endl;
		return false;
	}
	
	FileContents = FileContents.replace(InitMapsStart, 11, "return;");
	
	of << FileContents;
	
	of.close();
	
	return true;
}
