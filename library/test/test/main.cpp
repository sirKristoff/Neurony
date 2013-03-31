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
	}

	TcResult
	tc01()
	{
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

