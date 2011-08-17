//      StatTest.hpp
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

#ifndef TEST_STATTEST_H
#define TEST_STATTEST_H

#include "SyscallTest.hpp"
#include <unistd.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <pwd.h>

// Operations
enum StatSyscalls
{
    STAT_NORM_EXEC,
    StatErrNoAccess,
    StatErrNameTooLong,
    StatErrNotDir,
    StatErrLoopedSymLinks,
    StatErrNonExistantFile,
 
};

class StatTest : public SyscallTest
{
	public:
		StatTest(Mode mode, int operation, string arguments = "") :
            
            SyscallTest(mode, operation, arguments, "stat") 
        {
            

            long size;
            
            


            size = pathconf(".", _PC_PATH_MAX);
            _cwd = NULL;


            if ((_cwd = (char *)malloc((size_t)size)) != NULL)
                _cwd = getcwd(_cwd, (size_t)size);
                //cerr << "cwd"<<_cwd<<endl;
            _statDir = (string) _cwd + "/stat_test_dir";
            if (mkdir (_statDir.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)){}
        }
		virtual ~StatTest() {
        //    system (("rm -rf " + _statDir).c_str());
            free(_cwd);
            
        }
        // Tests for basic functionality
        Status NormExec();
        // Tests for error situations
        Status ErrNoAccess ();
        Status ErrNameTooLong ();
        Status ErrNotDir ();
        Status ErrFailToRead ();
        Status ErrLoopedSymLinks ();
        Status ErrNonExistantFile ();
        
        string GetTestDirPath() {
            return _statDir;
        }
     
	protected:
		virtual int Main(vector<string> args);
    private:
        string _statDir;
        char *_cwd;
};

#endif /* TEST_STATTEST_H */
 
