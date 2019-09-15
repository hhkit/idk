#pragma once
#include "BufferedObj.h"
namespace idk::vkn
{

	//Mark the start of the cycle, switch the current into old and back into current
	//only if there isn't something else in _old, otherwise nothing changes
	template<typename BufferedData>
	void BufferedObj<BufferedData>::UpdateCurrent(size_t curr_index)
	{
		if (curr_index == _cycle && _old)
			_old.reset();
		if (!_old)
		{
			_old = std::move(_current);
			_current = std::move(_back);
			_back.reset();
			_cycle = curr_index;
		}
	}
}