#include "stdafx.h"
#include "random.h"
#include <random>

namespace idk
{
	random::random()
	{
		static std::random_device _rd{};
		_seed = _rd();
	}

	random::random(unsigned seed)
		: _seed{ seed }
	{
	}

	float random::rangef(float min, float max)
	{
		return _gen() * (max - min) + min;
	}

	int random::rangei(int min, int max)
	{
		return static_cast<int>(_gen() * (max - min)) + min;
	}

	float random::gaussf()
	{
		return _gen() + _gen() + _gen() + _gen() + _gen() + _gen() +
			_gen() + _gen() + _gen() + _gen() + _gen() + _gen() - 6;
	}

	float random::gaussf(float mean, float x)
	{
		return mean + gaussf() / 3.0f * x;
	}



	constexpr static auto multiplier = 48271ULL;
	float random::_gen()
	{
		// minstd_rand lcg
		_seed = static_cast<unsigned>((_seed * multiplier) % int_max);
		return static_cast<float>(_seed) / int_max;
	}
}