//      FSyncTest.cpp
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

#include <FSyncTest.hpp>
#include "File.hpp"

int FSyncTest::Main(vector<string> args)
{
    
    if ( geteuid() != 0 )
	{
		// The tests should be executed by the root user;
		cerr << "Only root can execute these tests..." << endl;
		return Unres;
	}
    
	if ( _mode == Normal )
	{
		switch (_operation)
		{
            case FSyncNormExec:
                return NormExec();
         
            default:
				cerr << "Unsupported operation.";
				return Unres;
		}
	}

	cerr << "Test was successful";

	return Success;
}


Status FSyncTest::NormExec ()
{
    const char * buf = "abcdefg";
    File file (this->_tmpDir + "/fsync_test");
    
    if (write (file.GetFileDescriptor(), buf, strlen(buf)) == -1) {
        cerr << "write returned -1";
        return Unres;
    }
    
    if (fsync (file.GetFileDescriptor()) != 0) {
        cerr << "fsync returned non zero value";
        return Fail;
    } 
    
    return Success;
}

