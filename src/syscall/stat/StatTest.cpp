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
            case StatErrEmptyPath:
                return ErrEmptyPath ();

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
        cerr << "stat returned non zero value " << strerror(errno);
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
    
    if (stat_buf.st_atime != file.GetATime()) {
        cerr << "stat filled stat_buf with wrong st_atime value, "
			"st_atime = " << stat_buf.st_atime << " but it should be " 
			<< file.GetATime();
        return Fail;
    }
    
    
    if (stat_buf.st_atime != file.GetMTime()) {
        cerr << "stat filled stat_buf with wrong st_atime value, "
			"st_mtime = " << stat_buf.st_mtime << " but it should be " 
			<< file.GetMTime();
        return Fail;
    }
    
    
    if (stat_buf.st_atime != file.GetCTime()) {
        cerr << "stat filled stat_buf with wrong st_atime value, "
			"st_ctime = " << stat_buf.st_ctime << " but it should be " 
			<< file.GetCTime();
        return Fail;
    }
    
    return Success;
}

// EACCES Search permission is denied for a component of the path prefix.
Status StatTest::ErrNoAccess ()
{
    string dirPath = this->_statDir + "/err_noaccess_dir";
    string filePath = dirPath + "/stat_file";
    struct passwd * noBody;
    struct stat stat_buf;
    const int FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
   
   
    if (mkdir (dirPath.c_str(), FILE_MODE) == -1) {
        cerr<<"mkdir() can't create directory. " << strerror(errno);
        return Unres;
    }
   
    StatFile file (filePath);
   
    if (0 != chmod (filePath.c_str(), FILE_MODE)) {
        cerr<<"Can not chmod file " << strerror(errno);
        return Unres;
    }
    if (0 != chmod (dirPath.c_str(), FILE_MODE)) {
        cerr<<"can not chmod directory " << strerror(errno);
        return Unres;
    }
    
    // Change root to nobody
    if((noBody = getpwnam("nobody")) == NULL) {
        cerr<< "Can not set user to nobody "  << strerror(errno);
        return Unres;
    }
    if (0 != setuid(noBody->pw_uid)) {
        cerr<<"Can not set uid "  << strerror(errno);
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
            "for a component of the path prefix. The following error was set "
            << strerror(errno);
        return Fail;
    }
    return Success;
}

// ENAMETOOLONG The length of the path argument exceeds {PATH_MAX} or a pathname
// component is longer than {NAME_MAX}.
Status StatTest::ErrNameTooLong ()
{
    struct stat stat_buf;
    const int length = PATH_MAX + 1;
    char tooLongPath[length];
    
    memset(tooLongPath, 'a', length);    
    
    int ret = stat (tooLongPath, &stat_buf);
    if (ret != -1) {
        cerr<<"stat must return -1 when length of the path argument exceeds "
            "{PATH_MAX}";
        return Fail;
    }
    
    if (errno != ENAMETOOLONG) {
        cerr<<"stat must set ENAMETOOLONG error when length of the path "
            "argument exceeds {PATH_MAX}. The following error was set:"
            << strerror(errno);
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
            "is not a directory. The following error was set: " 
            << strerror(errno);
        return Fail;
    }
    return Success;
    
}

// EIO An error occurred while reading from the file system.
Status StatTest::ErrFailToRead ()
{
	return Unsupported;
}

// ENOENT A component of path does not name an existing file 
Status StatTest::ErrNonExistantFile ()
{
    struct stat stat_buf;

    int ret = stat ("./non_existant_file", &stat_buf);
    
    if (ret != -1) {
        cerr<<"stat must return -1 when a component of path does not name an "
            "existing file.";
        return Fail;
    }
    
    if (errno != ENOENT) {
        cerr<<"stat must set ENOENT error when a component of path does not name an "
            "existing file. The following error was set: " << strerror(errno);
        return Fail;
    }
    return Success;
}
// ENOENT the path is an empty string.
Status StatTest::ErrEmptyPath ()
{
    struct stat stat_buf;

    int ret = stat ("", &stat_buf);
    
    if (ret != -1) {
        cerr<<"stat must return -1 when a path is an empty string.";
        return Fail;
    }
    
    if (errno != ENOENT) {
        cerr<<"stat must set ENOENT error when a path is an empty string."
			"The following error was set: " << strerror(errno);
        return Fail;
    }
    return Success;
}

// ELOOP A loop exists in symbolic links encountered during resolution of the
// path argument.
Status StatTest::ErrLoopedSymLinks ()
{
    struct stat stat_buf;
    string dirPath = this->_statDir + "/err_loop_dir";
    string link1 = dirPath + "/link1";
    string link2 = dirPath + "/link2";
    
    string filePath = link1 + "/stat_file";
    const int FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
   
    if (mkdir (dirPath.c_str(), FILE_MODE) == -1) {
        cerr<<"mkdir() can't create directory. " << strerror(errno);
        return Unres;
    }
    if (symlink (dirPath.c_str(), link1.c_str()) == -1) {
        cerr<<"symlink() can't create symlink. " << strerror(errno);
        return Unres;
    }
    if (symlink (link1.c_str(), link2.c_str()) == -1) {
        cerr<<"symlink() can't create symlink. " << strerror(errno);
        return Unres;
    }
    if (unlink (link1.c_str()) == -1) {
        cerr<<"remove() can't remove symlink. " << strerror(errno);
        return Unres;
    }
    if (symlink (link2.c_str(), link1.c_str()) == -1) {
        cerr<<"symlink() can't create symlink. " << strerror(errno);
        return Unres;
    }    
   
    int ret = stat (filePath.c_str(), &stat_buf);
    
    if (ret != -1) {
        cerr<<"stat must return -1 when a loop exists in symbolic links "
            "encountered during resolution of the path argument.";
        return Fail;
    }    
    if (errno != ELOOP) {
        cerr<<"stat must set ELOOP error when a loop exists in symbolic links "
            "encountered during resolution of the path argument. The following"
            " error was set: " << strerror(errno);
        return Fail;
    }
    return Success;

}
