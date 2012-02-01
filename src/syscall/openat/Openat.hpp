//      Openat.hpp
//      
//      Copyright (C) 2011, Institute for System Programming
//       	                of the Russian Academy of Sciences (ISPRAS)
//		Author(s):
//      Gurgen Torozyan <gurgen.torozyan@gmail.com>
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

#ifndef OPENAT_H
#define OPENAT_H

#include "SyscallTest.hpp"
#include <stdlib.h>
#include <unistd.h>
#include <linux/fs.h>
#include <pwd.h>
#include <sys/stat.h>

// Operations
enum OpenatSyscalls
{
OPENAT_EACCES,
OPENAT_EEXIST,
OPENAT_EFAULT,
OPENAT_EISDIR,
OPENAT_ELOOP,
OPENAT_EMFILE,
OPENAT_ENAMETOOLONG,
OPENAT_ENODEV,
OPENAT_ENOENT,
OPENAT_ENOTDIR,
OPENAT_ENXIO,
OPENAT_ETXTBSY,
OPENAT_NORMAL

};

class Openat : public SyscallTest
{			
public:	
	Openat(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "Openat")
	{			
			  long size;
            _cwd = NULL;
            size = pathconf(".", _PC_PATH_MAX);

            if ((_cwd = (char *)malloc((size_t)size)) != NULL)
                _cwd = getcwd(_cwd, (size_t)size);
            
            _statDir = (string) _cwd + "/openat_test_dir";
            _buildDir = (string) _cwd;
            mkdir (_statDir.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		
	}
	virtual ~Openat() 
	{ 
			system (("rm -rf " + _statDir).c_str());
		
			free(_cwd);
    }
  
	Status openatTooLongPath();		//done
	Status openatNoAcces();    		//done
	Status openatOutside();    		//done
	Status openatPathAlreadyExist();  //done
	Status openatPathIsDir();         //done
	Status openatMaxFileNumOpened();  //done
	Status openatPathIsTooLong();     //done
	Status openatNoSuchDev();         //done
	Status openatPathIsNotDir();		//done
	Status openatLoopInSymLink();		//done
	Status openatPathisExecutable();  //done
	Status openatPathCompIsNotDir();  //done
	Status openatNoDevWRBL();         //done
	Status openatNormal();				//done
	
protected:
	virtual int Main(vector<string> args);	
	
private:
    string _statDir;
    string _buildDir;
    char *_cwd;
	
};
#endif
