#pragma once
#include <limits>

namespace idk
{
	class random
	{
	public:
		constexpr static int int_max = std::numeric_limits<int>::max();
		constexpr static float float_max = std::numeric_limits<float>::max();

		// initialize random number generator with random seed
		random();

		explicit random(unsigned seed);

		// returns float in the range [min, max]
		float rangef(float min = 0, float max = float_max);

		// returns int in the range [min, max)
		int rangei(int min = 0, int max = int_max);

		// approximated gaussian distribution in the range ~[-3, 3]
		float gaussf();

		// approximated gaussian distribution in the range ~[mean - x, mean + x]
		float gaussf(float mean, float x);
	
	private:
		unsigned _seed;
		float _gen();
	};
}