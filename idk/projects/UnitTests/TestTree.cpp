#include "pch.h"
#include <ds/slow_tree.inl>

TEST(SlowTree, SlowTreeTraversal)
{
	using namespace idk;

	slow_tree<int> hi{-1};
	hi.emplace_child(0).emplace_child(1).emplace_child(2);
	hi.emplace_child(3);
	hi.emplace_child(4).emplace_child(5);
	hi.emplace_child(6);

	int indent = 0;
	hi.visit([&indent](auto elem, int depth) 
	{
		indent += depth;
		for (auto i = 0; i < indent; ++i)
			std::cout << "\t";

		std::cout << elem << "\n";

	});

	indent = 0;
	hi.visit([&indent](auto elem, int depth)->bool
		{
			indent += depth;
			for (auto i = 0; i < indent; ++i)
				std::cout << "\t";

			std::cout << elem << "\n";

			return elem != 0;
		});

	EXPECT_EQ(hi.size(), 4);
}