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
            case StatErrNoAccess:
                return ErrNoAccess();
            case StatErrNameTooLong:
                return ErrNameTooLong();
            case StatErrNotDir:
                return ErrNotDir();
            case StatErrLoopedSymLinks:
                return ErrLoopedSymLinks();
            case StatErrNonExistantFile:
                return ErrNonExistantFile();

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
        cerr << "stat returned non zero value";
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

// EACCES Search permission is denied for a component of the path prefix.
Status StatTest::ErrNoAccess ()
{
    string dirPath = this->_statDir + "/stat_dir";
    string filePath = dirPath + "/stat_file";
    struct passwd * noBody;
    struct stat stat_buf;
    const int FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
   
   
    mkdir (dirPath.c_str(), FILE_MODE);
   
    StatFile file (filePath);
   
    if (0 != chmod (filePath.c_str(), FILE_MODE)) {
        cerr<<"Can not chmod file";
        return Unres;
    }
    if (0 != chmod (dirPath.c_str(), FILE_MODE)) {
        cerr<<"can not chmod directory";
        return Unres;
    }
    
    // Change root to nobody
    if((noBody = getpwnam("nobody")) == NULL) {
        cerr<< "Can not set user to nobody";
        return Unres;
    }
    if (0 != setuid(noBody->pw_uid)) {
        cerr<<"Can not set uid";
        return Unres;
    }
    
    
    int ret = stat (filePath.c_str(), &stat_buf);
    
    if (ret != -1) {
        cerr<<"stat must return -1 when search(execute) bit is not set for a "
            "component of the path prefix";
        return Fail;
    }
    
    if (errno != EACCES) {
        cerr<<"stat must set EACCES error when search(execute) bit is not set "
            "for a component of the path prefix";
        return Fail;
    }
    return Success;
}

// ENAMETOOLONG The length of the path argument exceeds {PATH_MAX} or a pathname
// component is longer than {NAME_MAX}.
Status StatTest::ErrNameTooLong ()
{
    struct stat stat_buf;
    char tooLongPath[PATH_MAX + 1];
    
    for(int i = 0; i <= PATH_MAX; i++)
        tooLongPath[i] = 'a';    
    
    int ret = stat (tooLongPath, &stat_buf);
    if (ret != -1) {
        cerr<<"stat must return -1 when length of the path argument exceeds "
            "{PATH_MAX}";
        return Fail;
    }
    
    if (errno != ENAMETOOLONG) {
        cerr<<"stat must set ENAMETOOLONG error when length of the path "
            "argument exceeds {PATH_MAX}";
        return Fail;
    }
    
    return Success;
    
}

// ENOTDIR A component of the path prefix is not a directory.
Status StatTest::ErrNotDir ()
{
    struct stat stat_buf;
    string notDirPath = this->_statDir + "/stat_not_dir";
    string filePath = notDirPath + "/stat_file";
    
    StatFile file (notDirPath);
   
    int ret = stat (filePath.c_str(), &stat_buf);
    
    if (ret != -1) {
        cerr<<"stat must return -1 when a component of the path prefix is not "
            "a directory.";
        return Fail;
    }
    
    if (errno != ENOTDIR) {
        cerr<<"stat must set ENOTDIR error when a component of the path prefix "
            "is not a directory.";
        return Fail;
    }
    return Success;
    
}

// EIO An error occurred while reading from the file system.
Status StatTest::ErrFailToRead ()
{}

// ENOENT A component of path does not name an existing file or path is an 
// empty string.
Status StatTest::ErrNonExistantFile ()
{
    struct stat stat_buf;

    int ret = stat ("./non_existant_file", &stat_buf);
    
    if (ret != -1) {
        cerr<<"stat must return -1 when a component of path does not name an "
            "existing file or path is an empty string.";
        return Fail;
    }
    
    if (errno != ENOENT) {
        cerr<<"stat must set ENOENT error when a component of path does not name an "
            "existing file or path is an empty string.";
        return Fail;
    }
    return Success;
}

// ELOOP A loop exists in symbolic links encountered during resolution of the
// path argument.
Status StatTest::ErrLoopedSymLinks ()
{

}
