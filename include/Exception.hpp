//      exception.hpp
//      
//		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//      	Narek Saribekyan <narek.saribekyan@gmail.com>
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
#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <errno.h>
#include <string>
using std::string;

class Exception 
{
public:
	Exception(string message) : _message(message), _errno(errno)  {}
	Exception(Exception const & other) : _message(other._message), _errno(other._errno) {}
	virtual ~Exception() {}
		
	virtual string GetMessage() const 
	{
		return _message;
	}	
	virtual int GetErrno() const 
	{
		return _errno;
	}	
private:
	string _message;
	int _errno;
};
#endif /* EXCEPTION_H */
