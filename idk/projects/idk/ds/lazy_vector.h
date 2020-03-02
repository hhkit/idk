#pragma once
#include <vector>

namespace idk
{
	//a wrapper for a vector that resizes when you attempt access out of bounds.
	template<typename T,typename _Alloc =std::allocator<T>, typename base_t = std::vector<T, _Alloc>>
	struct lazy_vector : base_t
	{
	public:
		using base_t::base_t;
		using base_t::operator=;
		auto& operator[](size_t i)
		{
			if (i >= this->size())
			{
				this->resize(i + 1);
			}
			return this->at(i);
		}
		decltype(auto) operator[](size_t i)const 
		{
			return this->at(i);
		}
	};
}