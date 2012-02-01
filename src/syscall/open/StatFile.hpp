//      StatFile.hpp
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

#ifndef STAT_FILE_H
#define STAT_FILE_H

#include "File.hpp"

class StatFile : public File 
{
public:
    StatFile(string pathname) : File(pathname) {
        _uid = getuid();
        _gid = getgid();
        _st_atime = time(NULL);
        _st_mtime = _st_atime;
        _st_ctime = _st_mtime;
        
    }
    uid_t GetUID() {
        return _uid;
    }
    gid_t GetGID() {
        return _gid;
    }
    time_t GetATime() {
        return _st_atime;
    }
    time_t GetMTime() {
        return _st_mtime;
    }
    time_t GetCTime() {
        return _st_ctime;
    }
private:
    uid_t _uid;
    gid_t _gid;
    time_t _st_atime;
    time_t _st_mtime;
    time_t _st_ctime;
};



#endif /*STAT_FILE_H*/
