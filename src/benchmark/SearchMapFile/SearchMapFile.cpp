//      SearchMapFile.cpp
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

#include <sys/mman.h>
#include <stdlib.h>
#include <memory>
#include <algorithm>
#include <fcntl.h>
#include <SearchMapFile.hpp>
#include "UnixCommand.hpp"
#include "File.hpp"

using std::auto_ptr;

bool cmp(int ch)
{
	if (ch == 48)
		return true;
}

int SearchMapFileTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{
		switch (_operation)
		{
			case SearchMapFile:
				return SearchMapFileFunc();
			default:
				cerr << "Unsupported operation.";
				return Unres;
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status SearchMapFileTest::SearchMapFileFunc()
{
	UnixCommand command("${CMAKE_INSTALL_PREFIX}bin/createfile");
    auto_ptr<ProcessResult> result(command.Execute(CreateArguments()));
    cerr << result->GetOutput() << " ";
    
    if ((Status)result->GetStatus() != Success)
		return (Status)result->GetStatus();
    
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1)
    {
		cerr << strerror(errno);
		return Unres;
	}
	
	void *map = mmap(NULL, 150 * 1024 * 1024, PROT_READ, MAP_SHARED, fd, 0);
	if (map == MAP_FAILED)
	{
		cerr << strerror(errno);
		close(fd);
		return Unres;
	}
	
	char *data = static_cast<char *>(map);
	
	bool found = false;
	int len = strlen(data);
	for (int i = 0; i < len; ++i)
		if (data[i] == '1')
		{
			found = true;
			break;
		}
	
    if (munmap(map, 150 * 1024 * 1024) == -1) 
    {
		cerr << strerror(errno);
		close(fd);
		return Unknown;
    }
    
    close(fd);
    
    // expected not to find the '1', because the file was filled with '0's
	if (found)
		return Fail;
	
    return Success;
}

vector<string> SearchMapFileTest::CreateArguments()
{
	vector<string> command;
	command.push_back(filename);
	command.push_back(fileSize);
	return command;
}
