//      Utime.cpp
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



#include <Utime.hpp>
#include "File.hpp"

int UtimeTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case UtimeNormalNotNull:
				return NormallNotNull();
			case UtimeNormalNull:
				return NormallNull();
			case UtimeErrAccess1:
				return ErrAccess1();
			case UtimeErrAccess2:
				return ErrAccess2();
			case UtimeErrAccess3:
				return ErrAccess3();
			case UtimeErrAccess4:
				return ErrAccess4();
			case UtimeErrLoop:
				return ErrLoop();
			case UtimeErrNameTooLong1:
				return ErrNameTooLong1();
			case UtimeErrNameTooLong2:
				return ErrNameTooLong2();
			case UtimeErrNoEnt1:
				return ErrNoEnt1();
			case UtimeErrNoEnt2:
				return ErrNoEnt2();
			case UtimeErrNotDir:
				return ErrNotDir();
			case UtimeErrPerm:
				return ErrPerm();
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status UtimeTest::NormallNotNull()
{
	cerr << "Not implemented yet.";
	return Unsupported;
}
Status UtimeTest::NormallNull()
{
	cerr << "Not implemented yet.";
	return Unsupported;
}
Status UtimeTest::ErrAccess1()
{
	cerr << "Not implemented yet.";
	return Unsupported;
}
Status UtimeTest::ErrAccess2()
{
	cerr << "Not implemented yet.";
	return Unsupported;
}
Status UtimeTest::ErrAccess3()
{
	cerr << "Not implemented yet.";
	return Unsupported;
}
Status UtimeTest::ErrAccess4()
{
	cerr << "Not implemented yet.";
	return Unsupported;
}
Status UtimeTest::ErrLoop()
{
	cerr << "Not implemented yet.";
	return Unsupported;
}
Status UtimeTest::ErrNameTooLong1()
{
	cerr << "Not implemented yet.";
	return Unsupported;
}
Status UtimeTest::ErrNameTooLong2()
{
	cerr << "Not implemented yet.";
	return Unsupported;
}
Status UtimeTest::ErrNoEnt1()
{
	cerr << "Not implemented yet.";
	return Unsupported;
}
Status UtimeTest::ErrNoEnt2()
{
	cerr << "Not implemented yet.";
	return Unsupported;
}
Status UtimeTest::ErrNotDir()
{
	cerr << "Not implemented yet.";
	return Unsupported;
}
Status UtimeTest::ErrPerm()
{
	cerr << "Not implemented yet.";
	return Unsupported;
}

