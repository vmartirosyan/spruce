//      FSyncTest.hpp
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//			Suren Grigoryan <suren.grigoryan@gmail.com>
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

#ifndef TEST_FSYNCTEST_H
#define TEST_FSYNCTEST_H

#include "SyscallTest.hpp"
#include "exception.hpp"

#include <unistd.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <pwd.h>

// Operations
enum FSyncSyscalls
{
    FSyncNormExec,
 
};

class FSyncTest : public SyscallTest
{
	public:
		FSyncTest(Mode mode, int operation, string arguments = "") :
            
            SyscallTest(mode, operation, arguments, "fsync") 
        {
            long size;
            _cwd = NULL;
            size = pathconf(".", _PC_PATH_MAX);

            if ((_cwd = (char *)malloc((size_t)size)) != NULL)
                _cwd = getcwd(_cwd, (size_t)size);
            
            _tmpDir = (string) _cwd + "/fsync_test_dir";
            mkdir (_tmpDir.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

        }
		virtual ~FSyncTest() {
            system (("rm -rf " + _tmpDir).c_str());
            free(_cwd);
        }
        // Tests for basic functionality
        Status NormExec();
        // Tests for error situations
        
        
	protected:
		virtual int Main(vector<string> args);
    private:
        string _tmpDir;
        char *_cwd;
};

#endif /* TEST_FSYNCTEST_H */
 
