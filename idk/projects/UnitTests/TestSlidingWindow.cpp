#include "pch.h"

#include <ds/circular_buffer.inl>
#include <ds/sliding_window.inl>
#include <ds/zip.inl>

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
	using namespace idk;

	{
		sliding_window<int, 8> window;
		EXPECT_TRUE(window.empty());

		for (int iters = 0; iters < 2; ++iters)
		{
			for (int i = 0; i < 10; ++i)
				window.emplace_back(i);

			EXPECT_FALSE(window.empty());

			//for (int i = 0; i < 8; ++i)
			//	EXPECT_EQ(window[i], i);

			window.pop_front();
			EXPECT_FALSE(window.empty());

			window.emplace_back(5);

			for(auto [key, val] : window)
			{
				std::cout << '(' << key << ',' << val << ')';
			}
			array<int, 8> res{ 1, 2, 3, 4, 5, 6, 7, 5 };
			auto itr = res.begin();
			for (auto [key, val] : window)
			{
				EXPECT_EQ(val, *itr++);
			}

			for (int i = 0; i < 8; ++i)
				window.pop_front();

			EXPECT_TRUE(window.empty());
		}
	}
	{
		sliding_window<int, 8> window;

		for (int i = 0; i < 8; ++i)
			window.emplace_back(i);

		const auto& cwindow = window;
		for (auto [key, value] : cwindow)
			EXPECT_EQ(key, value);

		EXPECT_THROW(cwindow[9], std::out_of_range);
		window.pop_front();
		window.emplace_back(0);
		EXPECT_THROW(cwindow[0], std::out_of_range);
	}
	std::cout << '\n';
}