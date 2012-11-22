//      FSyncTest.hpp
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

#ifndef SYNC_H
#define SYNC_H

#include "SyscallTest.hpp"
#include "File.hpp"


#include <linux/fs.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <Directory.hpp>
#include <pwd.h>

// Operations
/*enum SyncSyscalls
{
    SyncNormExec
};*/

class Sync : public SyscallTest
{
	public:
		Sync (Mode mode, int operation, string arguments = "") :
            
            SyscallTest(mode, operation, arguments, "sync"),
            _tmpDir("sync_test_dir"),
            dir(_tmpDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
        {
        }
		virtual ~Sync () {
        }
        // Tests for basic functionality
        Status NormExec ();
      
        
	protected:
		virtual int Main (vector<string> args);
    private:
        string _tmpDir;
        Directory dir;
};

#endif /* TEST_FSYNCTEST_H */
 
