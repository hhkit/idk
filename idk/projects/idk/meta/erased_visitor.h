#pragma once
#include <functional>
#include <memory>

namespace idk
{
	template<typename ... Funcs>
	class erased_visitor;

	template<typename Rets, typename ... OverloadedParams, typename ... ExtraParams>
	class erased_visitor<Rets(OverloadedParams, ExtraParams...) ...>
		: std::shared_ptr<void>, std::function<Rets(OverloadedParams, ExtraParams...)>...
	{
	public:
		using FunctorStorage = std::shared_ptr<void>;

		template<typename Visitor, typename = std::enable_if_t<
			!std::is_same_v<std::decay_t<Visitor>, erased_visitor>>
			>
		erased_visitor(Visitor&& functor)
			: std::shared_ptr<void>{ std::make_shared<Visitor>(std::forward<Visitor>(functor)) }
			, std::function<Rets(OverloadedParams, ExtraParams...)>
			{
				std::reference_wrapper<Visitor>(*static_cast<Visitor*>(static_cast<FunctorStorage&>(*this).get()))
			} ...
		{
		}

		template<typename Visitor, typename = std::enable_if_t<
			!std::is_same_v<std::decay_t<Visitor>, erased_visitor>>
		>
		erased_visitor(const Visitor& functor)
			: std::shared_ptr<void>{std::make_shared<Visitor>(functor)}
			, std::function<Rets(OverloadedParams, ExtraParams...)>
			{
				std::reference_wrapper<Visitor>(*static_cast<Visitor*>(static_cast<FunctorStorage&>(*this).get()))
			} ...
		{
		}

		using std::function<Rets(OverloadedParams, ExtraParams...)>::operator()...;
	};
}