//      StatTest.cpp
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

#include <linux/fs.h>
#include <sys/capability.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <StatTest.hpp>
#include "StatFile.hpp"




int StatTest::Main(vector<string> args)
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
            case STAT_NORM_EXEC:
                return NormExec();

            default:
				cerr << "Unsupported operation.";
				return Unres;
		}
	}

	cerr << "Test was successful";

	return Success;
}


Status StatTest::NormExec ()
{
    struct stat stat_buf;
    
    StatFile file ("./stat_test");
    
    int ret = stat (file.GetPathname().c_str(), &stat_buf);
    
    if (0 != ret) {
        cerr << "stat returned non zero";
        return Fail;
    } 
    
    if (stat_buf.st_uid != file.GetUID()) {
        cerr << "stat filled stat_buf with wrong st_uid value";
        return Fail;
    }
    
    if (stat_buf.st_gid != file.GetGID()) {
        cerr << "stat filled stat_buf with wrong st_gid value";
        return Fail;
    }
    
    return Success;
    
    
}
