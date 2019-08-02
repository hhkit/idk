#pragma once
#include <utility>
#include <tuple>
#include <string_view>

#include "../idk.h"
#include "Dispatcher_detail.h"

namespace idk
{
	template<typename Signature, typename Functor = function<Signature>>
	class Dispatcher;

	template<typename Ret, typename ... Params, typename Functor>
	class Dispatcher<Ret(Params...), Functor>
	{
	public:
		template<typename Fwd, typename = std::enable_if_t<!std::is_same_v<Dispatcher, Fwd>>>
		Dispatcher(Fwd&& functor, size_t reserve = 1000);

		template<typename ... Args>
		void enqueue(Args&& ...);

		size_t invoke(size_t count = size_t{});
		size_t invoke_all();
		size_t clear();
	private:
		using ParamTuple = tuple<detail::storage_type_t<Params>...>;

		Functor f;
		vector<ParamTuple> stored_parameters;
	};
}

#include "Dispatcher.inl"