#include "pch.h"

#include <ds/circular_buffer.inl>

TEST(ElementAccess, ElementAccessTest)
{
	using namespace idk;

	{
		circular_buffer<int, 10> buf;
		for (int i = 0; i < 5; ++i)
			buf.emplace_back(i);

		for (int i = 0; i < 5; ++i)
			EXPECT_EQ(buf[i], i);

		buf.pop_front();

		for (int i = 0; i < 4; ++i)
			EXPECT_EQ(buf[i], i + 1);
	}
	
	{
		circular_buffer<int, 4> buf; 

		for (int i = 0; i < 6; ++i)
			buf.emplace_back(i);
		/* 4 5 2 3 */

		buf.pop_front();

		/* 4 5 _ 3 */

		for (int i = 0; i < 3; ++i)
			EXPECT_EQ(buf[i], i + 3);
	}
}

TEST(SlidingWindowAccess, SlidingWindowTest)
{

}