//      Link.cpp
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

#include <Bdflush.hpp>
#include <sys/kdaemon.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include "File.hpp"
#include "StatFile.hpp"

int BdFlushTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case BDFLUSH_EPERM:
				return BdFlushPermuissionDenied();
			
			
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status BdFlushTest::BdFlushPermuissionDenied()
{	
	int func=2;
	int ret_val = -1;
    struct passwd * noBody;
    
    // Change root to nobody
    if((noBody = getpwnam("nobody")) == NULL) {
        cerr<< "Can not set user to nobody";
        return Unres;
    }
    if (0 != setuid(noBody->pw_uid)) {
        cerr<<"Can not set uid";
        return Unres;
    }
    
    ret_val = bdflush(func,ret_val);
     
    if (ret_val != -1) {
           cerr<<"bdflush should return -1 when caller does not have the CAP_SYS_ADMIN capability, but it returned 0";
       return Fail;
    }
    
    if (errno != EPERM) {
           cerr << "Incorrect error set in errno when caller does not have the CAP_SYS_ADMIN capability."<<strerror(errno);
        return Fail;
    }
    return Success;
	
	
}





