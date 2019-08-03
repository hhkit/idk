#include "pch.h"
#include <vector>
#include <random>
#include <chrono>

struct StrangeObject
{
	bool active = true;
	void ExpensiveOperation()
	{
		std::vector<int> v;
		for (int i = 0; i < 100; ++i)
			v.emplace_back();
	}
};

using myclock = std::chrono::high_resolution_clock;

TEST(SpeedTest, ActivesTogether)
{
	std::vector<StrangeObject> v;
	for (int i = 0; i < 500; ++i)
		v.emplace_back(StrangeObject{ true });
	for (int i = 0; i < 500; ++i)
		v.emplace_back(StrangeObject{ false });

	myclock c;
	auto iters = 500;
	auto dur = myclock::duration{};
	for (int i = 0; i < iters; ++i)
	{
		auto start = c.now();
		for (int i = 0; i < 500; ++i)
			v[i].ExpensiveOperation();
		dur += c.now() - start;
	}
	std::cout << (dur / iters).count() << std::endl;
}

TEST(SpeedTest, ActivesApart)
{
	std::vector<StrangeObject> v;
	int actives = 0;
	for (int i = 0; i < 1000; ++i)
	{
		if (actives < 500)
			v.emplace_back(StrangeObject{ (bool)(actives += rand() % 2) });
		else
			v.emplace_back(StrangeObject{ false });
	}

	myclock c;
	auto iters = 500;
	auto dur = myclock::duration{};
	for (int i = 0; i < iters; ++i)
	{
		auto start = c.now();
		for (auto& elem : v)
			if (elem.active)
				elem.ExpensiveOperation();
		dur += c.now() - start;
	}
	std::cout << (dur / iters).count() << std::endl;
}