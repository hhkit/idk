#pragma once
#include <utility>
#include <tuple>
#include "Dispatcher.h"

namespace idk
{

	template<typename Ret, typename ... Params, typename Functor>
	template<typename Fwd, typename>
	Dispatcher<Ret(Params...), Functor>::Dispatcher(Fwd&& fn, size_t reserve)
		: f{ std::forward<Fwd>(fn) }, stored_parameters{}
	{
		stored_parameters.reserve(reserve);
	}

	template<typename Ret, typename ... Params, typename Functor>
	template<typename ... Args>
	void Dispatcher<Ret(Params...), Functor>::enqueue(Args&& ... args)
	{
		stored_parameters.emplace_back(ParamTuple{ std::forward<Args>(args)... });
	}

	template<typename Ret, typename ... Params, typename Functor>
	size_t Dispatcher<Ret(Params...), Functor>::invoke(size_t count)
	{
		if (count)
		{
			size_t i = 0;
			for (; i < count && i < stored_parameters.size(); ++i)
				std::apply(f, stored_parameters[i]);
			stored_parameters.erase(stored_parameters.begin(), stored_parameters.begin() + i);
			return i;
		}
		else
			return invoke_all();
	}
	template<typename Ret, typename ... Params, typename Functor>
	size_t Dispatcher<Ret(Params...), Functor>::invoke_all()
	{
		for (auto& elem : stored_parameters)
			std::apply(f, elem);
		return clear();
	}
	template<typename Ret, typename ... Params, typename Functor>
	size_t Dispatcher<Ret(Params...), Functor>::clear()
	{
		const auto retval = stored_parameters.size();
		stored_parameters.clear();

		return retval;
	}
}