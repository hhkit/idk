#pragma once
#include <idk.h>
namespace idk
{

	template<typename T = float>
	struct FakeMat4
	{
		static constexpr unsigned N = 4;
		tvec<T, N> data[N];

		FakeMat4() = default;
		FakeMat4(const tmat<T, N, N>& m)
		{
			for (uint32_t i = 0; i < N; ++i)
			{
				data[i] = m[i];
			}
		}
	};


}