//      Chmod.hpp
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//			Gurgen Torozyan <gurgen.torozyan@gmail.com>
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

#ifndef OPEN_H
#define OPEN_H

#include "SyscallTest.hpp"
#include <stdlib.h>
#include <unistd.h>
#include <linux/fs.h>
#include <pwd.h>
#include <sys/stat.h>
#include <Directory.hpp>
// Operations
/*enum OpenSyscalls
{
OPEN_EACCES,		//done
OPEN_EEXIST,		//done
OPEN_EFAULT,        //done
OPEN_EISDIR,		//done
OPEN_ELOOP,         //done
OPEN_EMFILE,		//done
OPEN_ENAMETOOLONG,  //done
OPEN_ENODEV,		//done
OPEN_ENOENT,        //done
OPEN_ENOTDIR,       //done
OPEN_ENXIO,			//done
OPEN_ETXTBSY,		//done
OPEN_NORMAL			//done

};*/

class Open : public SyscallTest
{			
public:	
	Open(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "open"),
		_statDir("open_test_dir"),
		dir(_statDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
	{			
		
	}
	virtual ~Open()
	{
    }	
	Status openTooLongPath();		//done
	Status openNoAcces();    		//done
	Status openOutside();    		//done
	Status openPathAlreadyExist();  //done
	Status openPathIsDir();         //done
	Status openMaxFileNumOpened();  //done
	Status openPathIsTooLong();     //done
	Status openNoSuchDev();         //done
	Status openPathIsNotDir();		//done
	Status openLoopInSymLink();		//done
	Status openPathisExecutable();  //done
	Status openPathCompIsNotDir();  //done
	Status openNoDevWRBL();         //done
	Status openNormal();            //done

	
	
	 string GetTestDirPath() {
            return _statDir;
        }
protected:
	virtual int Main(vector<string> args);	
 private:
        string _statDir;
        Directory dir;
	
};
#endif
