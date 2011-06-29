//      SplitLargeFile.cpp
//      
//      Copyright 2011 Tigran Piloyan <tigran.piloyan@gmail.com>
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

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include "SplitLargeFile.hpp"

int SplitLargeFile::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case SplitLargeFileOp:
				return SplitLargeFileFunc();
			default:
				cerr << "Unsupported operation.";	
				return Unres;
		}
	}
	cerr << "Test was successful";	
	return Success;
}

Status SplitLargeFile::SplitLargeFileFunc()
{
	int fd = open((this->filename).c_str(), O_RDONLY);
	std::stringstream ss;
	
	//piece of code to determine file size
    struct stat buf;
    fstat(fd, &buf);
    int size = buf.st_size;
    
    int filesSize = size / (this->numberOfFiles); // computing the small files sizes
    
    string buffer = "";
    string smallFiles = "small";
    
    for (int i = 0; i < this->numberOfFiles; ++i) {
		ss << i;
		int fds = open((smallFiles + ss.str()).c_str(), O_WRONLY | O_CREAT); 
		
		if (read(fd, (void *)buffer.c_str(), filesSize) != -1) {
			int writtenBytes = write(fds, (void *)buffer.c_str(), filesSize);
			if(writtenBytes == -1) {
				cerr << "An error occured during writing";
				return Unres;
			}
		} else {
			cerr << "An error occured during reading";
			return Unres;
		}
		
	}
    
    cerr << smallFiles;
	//ofstream large_file ("largefile");
    //if (large_file.is_open()) {
      //large_file << "This is a line.\n";
      //large_file << "This is another line.\n";
      //large_file.close();
    //}
    //else {
		//cout << "Unable to open file";
	//}
	
    //std::auto_ptr<ProcessResult> result();
    //cerr << result->GetOutput() << " ";
    
    //return (Status)result->GetStatus();
    return (Status)Success;
}

