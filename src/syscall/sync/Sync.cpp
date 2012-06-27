//      FSyncTest.cpp
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

#include <Sync.hpp>

int Sync::Main(vector<string> args)
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
            case SyncNormExec:
                return NormExec();
                      
         
            default:
				cerr << "Unsupported operation.";
				return Unres;
		}
	}

	cerr << "Test was successful";

	return Success;
}


Status Sync::NormExec ()
{
    const char * buf = "abcdefg";
    File file (this->_tmpDir + "/sync_test");
    sleep(1);
    
    
    time_t time_before = time(NULL);

    if (write (file.GetFileDescriptor(), buf, strlen(buf)) == -1) {
        cerr << "write returned -1";
        return Unres;
    }    
    
    sync ();
    close(file.GetFileDescriptor());
    time_t time_after = time(NULL);
     
    
    struct stat stat_buf;
    
    if(stat (file.GetPathname().c_str(), &stat_buf) == -1) {
		cerr << "stat returned -1";
		return Unres;
	}

    if (stat_buf.st_mtime < time_before || stat_buf.st_mtime > time_after) {
        cerr << "sync hasn't updated last modification time of the file"<<endl;
        return Fail;
    }
    return Success;
}



