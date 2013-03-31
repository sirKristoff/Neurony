#include <iostream>

#include "./../testgroup.h"

using namespace std;


class MyTestGroup: public TestGroup
{
public:

	MyTestGroup()
		: TestGroup("Basic Test Group", cout)
	{
		REGISTER_TC( MyTestGroup, tc01 );
		REGISTER_TC( MyTestGroup, tc02 );
	}

	TcResult
	tc01()
	{
		LOG_ASSERT( 2 );
		LOG(endl);

		ASSERT( "3" );

		return trPass;
	}

	TcResult
	tc02()
	{
		LOG_ASSERT( 2 );
		LOG(endl);

		ASSERT( "2" );

		return trPass;
	}

};

//******************************************************************************
int main()
{
	MyTestGroup tg;

	tg.run_group();

	return 0;
}

