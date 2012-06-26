//      Xattr.cpp
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//			Vahram MArtirosyan <vmartirosyan@gmail.com>
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

#include <Xattr.hpp>
#include "File.hpp"

int Xattr::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case XATTR_GET_SET:
				return XattrGetSet();
			
	        default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status Xattr::XattrGetSet()
{
	try
	{
		const char * FileName = "xattr_get_set";
		const char * AttrName = "XattrSetGetTest";
		const char * ValueSet = "asdf";		
		const int ValueLen = strlen(ValueSet);
		char * ValueGot = new char[strlen(ValueSet)];
		File file(FileName);
		
		if ( removexattr(FileName, AttrName) == -1 && errno != ENOATTR )
		{
			if ( errno == ENOTSUP )
			{
				cerr << "Xattrs are not supported" << endl;
				return Unsupported;
			}
			cerr << "Cannot remove xattr" << endl;
			return Unres;
		}
		
		if ( setxattr(FileName, AttrName, ValueSet, ValueLen, XATTR_CREATE) == -1 )
		{
			cerr << "Cannot set xattr" << endl;
			return Fail;
		}
		
		if ( getxattr(FileName, AttrName, (void*)ValueGot, ValueLen ) != ValueLen )
		{
			cerr << "Cannot get xattr" << endl;
			return Fail;
		}
		
		if ( strncmp(ValueSet, ValueGot, ValueLen ) )
		{
			cerr << "Different xattr values: " << ValueSet << " : " << ValueGot << endl;
			return Fail;
		}
		return Success;
		
	}
	catch (Exception e)
	{
		cerr << "Exception was thrown: " << e.GetMessage() << endl;
		return Unres;
	}
}
