//      engine.cpp
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Authors:
//      	Ruzanna Karakozova <r.karakozova@gmail.com>
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

#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <UnixCommand.hpp>
#include <sys/stat.h>
#include <sys/types.h>
using namespace std;

int main(int argc, char ** argv)
{
	if ( argc < 4 )
	{
		cerr << "Usage: " << argv[0] << " <dir_to_process> <output_dir> <engine_dir>" << endl;
		return 1;
	}
	
	//cout << "Processing folder " << argv[1] << endl;
	
	UnixCommand find("find");
	
	vector<string> args;
	
	args.push_back(argv[1]);
	args.push_back("-name");
	args.push_back("*.xml"); 
	
	ProcessResult *res = find.Execute(args);
	
	if ( res == NULL || res->GetStatus() != Success )
	{
		cerr << "Cannot execute the `find` command. Error: " << strerror(errno);
		return 2;
	}
	
	string output = res->GetOutput();
	
	stringstream str(output);
	string file;
	
	while ( str.good() ) 
	{
		str >> file;
		//cout << "File " << file << endl;
		int fd = open(file.c_str(), O_RDONLY);
		if ( fd == -1 )
		{
			cerr << "Cannot process file " << file << " Error: " << strerror(errno) << endl;
			continue;
		}
		
		struct stat buf;
		if ( stat(file.c_str(), &buf) == -1 )
		{
			cerr << "Cannot stat file " << file << ". Error: " << strerror(errno) << endl;
			continue;
		}
		
		char * read_buf = new char[buf.st_size + 1];
		if ( read(fd, read_buf, buf.st_size) == -1 )
		{
			cerr << "Canot read file " << file << ". Error: " << strerror(errno) << endl;
			close(fd);
			continue;
		}
		close(fd);
		read_buf[buf.st_size] = 0;
		
		string contents(read_buf);
			
		const int CODE_TAGS_NUM = 4;
		string code_tags[CODE_TAGS_NUM];		
		code_tags[0] = "Code";
		code_tags[1] = "Header";
		code_tags[2] = "Footer";
		code_tags[3] = "Internal";
		for(int i = 0; i < CODE_TAGS_NUM; i++)
		{
			size_t pos1 = 0, pos2;
			//cerr << "Processing tag " << code_tags[i] << endl;
			do
			{
				pos1 = contents.find("<" + code_tags[i]+ ">", pos1);
				if ( pos1 == string::npos )
					break;
				pos1 += 6;
				pos2 = contents.find("</" + code_tags[i] + ">", pos1);
				
				//cout << "pos1 " << pos1 << "\tpos2 " << pos2 << endl;
				
				string part = contents.substr(pos1, pos2-pos1);
				
				size_t pos = 0;
				while ( true )
				{
					pos = part.find("&", pos + 1);
					if ( pos == string::npos )
						break;
						//cout<<"first  "<<pos<<endl;
					part.replace(pos, 1, "&amp;");
				}
					
				pos = 0;
				while ( true )
				{
					pos = part.find("<", pos + 1);
					if ( pos == string::npos )
						break;
					//cout<<"second  "<<pos<<endl;
					part.replace(pos, 1, "&lt;");
				}
				
				contents.replace(pos1, pos2-pos1, part);
			}
			while (true);
		}
		
		file = file.replace(0, strlen(argv[1]), argv[2]);
		
		//cout << "output file : " << file << endl;
		ofstream of(file.c_str());
		of << contents;
		of.close();
	}
	
	// Now process the xml files with XSLT.
	UnixCommand xslt("xsltproc");
	vector<string> xslt_args;
	
	xslt_args.push_back("-o");
	xslt_args.push_back(((string)argv[2]) + "/module.cpp");
	xslt_args.push_back("-novalid");
	xslt_args.push_back("--stringparam");
	xslt_args.push_back("XmlFolder");
	xslt_args.push_back(argv[2]);
	xslt_args.push_back(((string)argv[3]) + "/module.xslt");
	xslt_args.push_back(((string)argv[2]) + "/module.xml");
	
	res = xslt.Execute(xslt_args);
	
	if ( res == NULL )
	{
		cerr << "Error executing xsltproc. Error: " << strerror(errno) << endl;
		return 3;
	}
	
	if ( res->GetStatus() != Success )
	{
		cerr << res->GetOutput() << endl;
		return 4;
	}
	cerr << res->GetOutput() << endl;
	
	return 0;
	//xsltproc -o ${CMAKE_CURRENT_BINARY_DIR}/module.cpp -novalid ${CMAKE_SOURCE_DIR}/engine/module.xslt ${CMAKE_CURRENT_SOURCE_DIR}/module.xml 
}
