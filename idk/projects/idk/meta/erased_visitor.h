#pragma once
#include <functional>
#include <memory>

namespace idk
{
	template<typename ... Funcs>
	class erased_visitor;
/*
	template<typename Ret, typename Tuple, typename ...ExtraParams>
	struct test {};

	template<typename Ret, typename OverloadedParam,typename ...OverloadedParams, typename ...ExtraParams>
	struct test<Ret, std::tuple<OverloadedParam, OverloadedParams...>, ExtraParams...>
		: std::function<Ret(OverloadedParam,ExtraParams...)>, test<Ret,std::tuple<OverloadedParams...>,ExtraParams...>
	{
		using FunctorStorage = std::shared_ptr<void>;
		using base2 = test<Ret, std::tuple<OverloadedParams...>, ExtraParams...>;

		template<typename Visitor, typename = std::enable_if_t<
			!std::is_same_v<std::decay_t<Visitor>, erased_visitor>>
			>
			erased_visitor(Visitor && functor)
			: std::shared_ptr<void>{ std::make_shared<Visitor>(std::forward<Visitor>(functor)) }
			, std::function<Rets(OverloadedParam, ExtraParams...)>
		{
			std::reference_wrapper<Visitor>(*static_cast<Visitor*>(static_cast<FunctorStorage&>(*this).get()))
		} , base2{std::forward<Visitor>(functor)}
		{
		}

			template<typename Visitor, typename = std::enable_if_t<
				!std::is_same_v<std::decay_t<Visitor>, erased_visitor>>
				>
				erased_visitor(const Visitor & functor)
				: std::shared_ptr<void>{ std::make_shared<Visitor>(functor) }
				, std::function<Rets(OverloadedParams, ExtraParams...)>
			{
				std::reference_wrapper<Visitor>(*static_cast<Visitor*>(static_cast<FunctorStorage&>(*this).get()))
			} ...
			{
			}

				using std::function<Rets(OverloadedParams, ExtraParams...)>::operator()...;
	};
*/
	template<typename ...T>
	struct Inheritor : T...
	{
		template<typename...Args>
		Inheritor(Args&& ... args) : T{args}...
		{

		}

		using T::operator()...;
	};

	template<typename Rets, typename ... OverloadedParams, typename ... ExtraParams>
	class erased_visitor<Rets(OverloadedParams, ExtraParams...) ...>
		: std::shared_ptr<void>, Inheritor< std::function<Rets(OverloadedParams, ExtraParams...)>...>//, std::function<Rets(OverloadedParams, ExtraParams...)>...
	{
	public:
		using FunctorStorage = std::shared_ptr<void>;
		using base_t =Inheritor< std::function<Rets(OverloadedParams, ExtraParams...)>...>;
		
		template<typename Visitor, typename = std::enable_if_t<
			!std::is_same_v<std::decay_t<Visitor>, erased_visitor>>
			>
		erased_visitor(Visitor&& functor)
			: std::shared_ptr<void>{ std::make_shared<Visitor>(std::forward<Visitor>(functor)) }
			, base_t{ std::function<Rets(OverloadedParams, ExtraParams...)>
			{
				std::reference_wrapper<Visitor>(*static_cast<Visitor*>(static_cast<FunctorStorage&>(*this).get()))
			} ... }
		{
		}

		template<typename Visitor, typename = std::enable_if_t<
			!std::is_same_v<std::decay_t<Visitor>, erased_visitor>>
		>
		erased_visitor(const Visitor& functor)
			: std::shared_ptr<void>{std::make_shared<Visitor>(functor)}
			, base_t{ std::function<Rets(OverloadedParams, ExtraParams...)>
			{
				std::reference_wrapper<Visitor>(*static_cast<Visitor*>(static_cast<FunctorStorage&>(*this).get()))
			} ... }
		{
		}
		//template<typename OverloadedParam>
		//using func_t = Rets(OverloadedParam, ExtraParams...);
		//template<typename OverloadedParam>
		//using func2_t = std::function<func_t<OverloadedParam>>;
		//using func2_t<OverloadedParams>::operator()...;
		using base_t::operator()...;
	};
}