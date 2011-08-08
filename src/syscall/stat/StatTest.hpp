
#ifndef TEST_STATTEST_H
#define TEST_STATTEST_H

#include "SyscallTest.hpp"
// Operations
enum StatSyscalls
{
    STAT_NORM_EXEC,
 
};

class StatTest : public SyscallTest
{
	public:
		StatTest(Mode mode, int operation, string arguments = "") :
            SyscallTest(mode, operation, arguments, "stat") {}
		virtual ~StatTest() {}
        Status NormExec();
     
	protected:
		virtual int Main(vector<string> args);
};

#endif /* TEST_STATTEST_H */
 
