#include "pch.h"
#include "../IDK/idk.h"
#include "../IDK/event/Dispatcher.h"
#include <iostream>

int my_test(const int& i )
{
	std::cout << i << ",";
	return i;
}

TEST(Dispatcher, TestDispatcher)
{
	using namespace idk;

	Dispatcher<int(const int&)> dispatcher{ my_test };
	for (int i = 0; i < 50; ++i)
		dispatcher.enqueue(i);
	EXPECT_EQ(dispatcher.invoke(25), 25);
	EXPECT_EQ(dispatcher.invoke(), 25);
}