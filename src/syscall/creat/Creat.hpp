//      Creat.hpp
//      
//      Copyright (C) 2011, Institute for System Programming
//       	                of the Russian Academy of Sciences (ISPRAS)
//		Author(s):
//      Suren Gishyan <sgishyan@gmail.com>
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

#ifndef CREAT_H
#define CREAT_H

#include "SyscallTest.hpp"
#include <stdlib.h>
#include <unistd.h>
#include <linux/fs.h>
#include <pwd.h>
#include <sys/stat.h>
#include <Directory.hpp>

// Operations
/*enum CreatSyscalls
{
	CREAT_ENAMETOOLONG, //done
	CREAT_EMFILE,		//done
	CREAT_EACCES,		//done
	CREAT_EFAULT,		//done
	CREAT_EISDIR, 		//done	 
	CREAT_ELOOP, 		//done
	CREAT_ENOENT, 		//done
	CREAT_ENOTDIR, 		//done
	CREAT_NORMAL  		//done 

};*/

class CreatTest : public SyscallTest
{			
public:	
	CreatTest(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "Creat"),
		_statDir("creat_test_dir"),
		dir(_statDir,  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
	{	
		
	}
	virtual ~CreatTest() {  
        }	
            
	Status CreatTooLongPath();			//done CREAT_ENAMETOOLONG
	Status CreatFileExist();			//done CREAT_EEXIST
	Status CreatMaxProcessFilesOpen();  //done CREAT_EMFILE
	Status CreatNoAcces();    			//done CREAT_EACCES
	Status CreatOutside();    			//done CREAT_EFAULT
	Status CreatPathIsDir();        	//done CREAT_EISDIR
	Status CreatPathIsNotDir();		    //done  CREAT_ENOTDIR
	Status CreatLoopInSymLink();		//done  CREAT_ELOOP
	Status CreatPathCompIsNotDir();     //done  CREAT_ENOENT
	Status CreatNormal();			    //done  CREAT_NORMAL
	
protected:
	virtual int Main(vector<string> args);	
	
private:
	void CreatMaxProcessCleanUp();
	int *buf;
	int file_index;
	char fname[50];
	string _statDir;
    Directory dir;
	
};
#endif
