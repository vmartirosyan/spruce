/* 
 * CommonOperation.hpp
 * Author: Narek Saribekyan
 */
#ifndef COMMON_OPERATION_H_
#define COMMON_OPERATION_H_

#include <string>

/*
 * An abstract base class representing common file system operations
 * copy, ls, touch, etc...
 */ 
class CommonOperation 
{
public:
	virtual int execute(std::string args) = 0;
	virtual ~CommonOperation() = 0;	
};
#endif
