#include "pch.h"
#include <idk.h>
#include <event/Dispatcher.inl>

static int yolo = 0;

int my_test(const int& i )
{
	yolo += i;
	return i;
}

TEST(Dispatcher, DispatcherTypeErasedFunction)
{
	using namespace idk;

	yolo = 0;
	Dispatcher<int(const int&)> dispatcher{ my_test };
	for (int i = 0; i < 50; ++i)
		dispatcher.enqueue(i);
	EXPECT_EQ(dispatcher.invoke(25), 25);
	EXPECT_EQ(dispatcher.invoke(), 25);
	EXPECT_EQ(yolo, 50 * 49 / 2);
}

TEST(Dispatcher, DispatcherFunctionPtr)
{
	using namespace idk;

	yolo = 0;
	Dispatcher<int(const int&), decltype(&my_test)> dispatcher{ my_test };
	for (int i = 0; i < 50; ++i)
		dispatcher.enqueue(i);
	EXPECT_EQ(dispatcher.invoke(25), 25);
	EXPECT_EQ(dispatcher.invoke(), 25);
	EXPECT_EQ(yolo, 50 * 49 / 2);
}