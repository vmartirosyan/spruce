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
void ProcessFile(string relative_file, string source, string dest);
bool GenerateOperationsHeader(string);
bool GenerateOperationsSource(string);

vector<string> Operations;

int main(int argc, char ** argv)
{
	if ( argc < 3 )
		return Usage(argv);
	cout << " ++ Generating operations header file..." << endl;
	//rename("/home/vmartirosyan/workspace/build/include/operations.hpp.old", "/home/vmartirosyan/workspace/build/include/operations.hpp");
	
	// Find all the header files in the source tree
	vector<string> files = GetHeaderFiles(static_cast<string>(argv[1]));
	
	// Procses each of them making the original enums comments 
	// Also collect all the operation enum values into the Operations vector
	for (vector<string>::iterator i = files.begin(); i != files.end(); ++i)
		ProcessFile(*i, static_cast<string>(argv[1]), static_cast<string>(argv[2]));
	
	// Generate the operations.hpp file...
	if (! GenerateOperationsHeader(static_cast<string>(argv[2])) )
		return -1;
		
		
	// Generate the operations.cpp file...
	if (! GenerateOperationsSource(static_cast<string>(argv[2])) )
		return -1;
	
	//ProcessFile("/home/vmartirosyan/workspace/spruce/src/fs/ext4fs/ioctl_test.hpp");
	return 0;
}

int Usage(char ** argv)
{
	cerr << "Usage: " << argv[0] << " <source_tree> <dest_tree>" << endl;
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
			file = file.substr(root.size());
			files.push_back(file);
			
		}
		//cout << "Files found\n" << FindOutput << endl;
		
		delete res;
	}
	
	return files;
}

void ProcessFile(string file, string source, string dest)
{
	// TODO: mkdir output directory
	mkdir ((dest + file).substr(0, (dest + file).rfind("/") ).c_str(), 0777);
	ifstream inf((source + file).c_str());
	ofstream of((dest + file).c_str());
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
						
					if ( operation == "" || operation == "//" )
						break;
						
					if ( operation[operation.size() - 1] == ',' )
						operation = operation.substr(0, operation.size() - 1);
						
					Operations.push_back(operation);
					//cout << operation << endl;
				}
				else
				{					
					state = 3;
					line = "// " + line;
				}
				
				break;
		}
		of << line << endl;
	}
	inf.close();
	of.close();
		
}

bool GenerateOperationsHeader(string root)
{
	string HeaderFileContents = "//      operations.hpp \n\
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
	static bool MapsInitialized;\n\
	static map<string, int> MapToInt; \n\
	static map<int, string> MapToString; \n\
public: \n\
	static string ToString( Operations op ) { return MapToString[op]; } \n\
	static Operations Parse( string s ) { return (Operations)MapToInt[s]; } \n\
	friend void InitMaps(); \n\
}; \n\
#endif /* OPERATIONS_HPP */ \n\
\n";

	cout << "++ generating file " << (root + "/../include/operations.hpp") << endl;

	ofstream of ((root + "/../include/operations.hpp").c_str());
	
	string EnumContents = "";
	
	for ( vector<string>::iterator i = Operations.begin(); i != Operations.end(); ++i )
	{
		EnumContents += "\t";
		EnumContents += *i;
		EnumContents += ",\n";
	}
	
	// Generate the enum contents
	size_t EnumStart = HeaderFileContents.find("%ENUM%");
	
	if ( EnumStart == string::npos )
	{
		cerr << "Cannot find %ENUM% in file contents" << endl;
		return false;
	}
	
	HeaderFileContents = HeaderFileContents.replace(EnumStart, 	6, EnumContents);
	
	of << HeaderFileContents;
	
	of.close();
	
	return true;
}

bool GenerateOperationsSource(string root)
{
	string SourceFileContents = "//      operations.cpp \n\
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
 \n\
#include \"operations.hpp\" \n\
bool Operation::MapsInitialized = false;\n\
map<string, int> Operation::MapToInt; \n\
map<int, string> Operation::MapToString;\n\
\n\
void InitMaps() \n\
{ \n\
if ( Operation::MapsInitialized )\n\
	return;\n\
%INIT_MAPS% \n\
	Operation::MapsInitialized = true;\n\
}\n\
\n";

	cout << "++ generating file " << (root + "/utils/operations.cpp") << endl;

	ofstream of ((root + "/utils/operations.cpp").c_str());
	
	// Generate the InitMaps body
	size_t InitMapsStart = SourceFileContents.find("%INIT_MAPS%");
	
	string InitMapsContents = "";
	
	for ( vector<string>::iterator i = Operations.begin(); i != Operations.end(); ++i )
	{
		InitMapsContents += "\tOperation::MapToInt[\"" + *i + "\"] = " + *i + ";\n";
		InitMapsContents += "\tOperation::MapToString[" + *i + "] = \"" + *i + "\";\n";		
	}
	
	if ( InitMapsStart == string::npos )
	{
		cerr << "Cannot find %INIT_MAPS% in file contents" << endl;
		return false;
	}
	
	SourceFileContents = SourceFileContents.replace(InitMapsStart, 11, InitMapsContents);
	
	of << SourceFileContents;
	
	of.close();
	
	return true;
}
