#ifndef STAT_FILE_H
#define STAT_FILE_H

#include "File.hpp"

class StatFile : public File 
{
public:
    StatFile(string pathname) : File(pathname) {
        _uid = getuid();
        _gid = getgid();
    }
    uid_t GetUID() {
        return _uid;
    }
    gid_t GetGID() {
        return _gid;
    }
private:
    uid_t _uid;
    gid_t _gid;
};



#endif /*STAT_FILE_H*/
