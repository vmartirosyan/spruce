//      FDataSyncTest.hpp
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

#ifndef TEST_FDATASYNCTEST_H
#define TEST_FDATASYNCTEST_H

#include "SyscallTest.hpp"
#include "File.hpp"
#include <../stat/StatFile.hpp>
#include <Directory.hpp>


#include <linux/fs.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <pwd.h>

// Operations
enum FDataSyncSyscalls
{
    FDataSyncNormExec,
    FDataSyncErrInvalidFD,
    FDataSyncErrNonFsyncFD,
 
};

class FDataSyncTest : public SyscallTest
{
	public:
		FDataSyncTest (Mode mode, int operation, string arguments = "") :
            
            SyscallTest(mode, operation, arguments, "fdatasync"),
            _tmpDir("fdatasync_test_dir"),
            dir(_tmpDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
        {
            
        }
		virtual ~FDataSyncTest () {
        }
        // Tests for basic functionality
        Status NormExec ();
        // Tests for error situations
        Status ErrInvalidFD ();
        Status ErrNonFsyncFD ();        
        
	protected:
		virtual int Main (vector<string> args);
    private:
        string _tmpDir;
        Directory dir;
};

#endif /* TEST_FDATASYNCTEST_H */
 
