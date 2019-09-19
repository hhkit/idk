#pragma once
#include <utility>
#include <variant>

namespace idk::monadic
{
	// a struct stolen from rust
	// lets us define a result, or return an error otherwise
	template<typename Result, typename Error>
	class result
		: protected std::variant<Result, Error>
	{
	protected:
		using Base = std::variant<Result, Error>;
	private:
		static constexpr auto Success = 0;
		static constexpr auto Failure = 1;
		static_assert(std::is_default_constructible_v<Error>, "Error must be default constructible");
	public:
		using value_type = Result;
		using Base::Base;
		using Base::operator=;
	
		// accessors
		Result& value();
		Error& error();
		const Result& value() const;
		const Error&  error() const;
		
		// monadic operators
		template<class Fn>              auto map(Fn&& visitor) const;
		template<class Fn>              auto and_then(Fn&& visitor) const;
		template<class Fn, class ErrFn> auto and_then(Fn&& visitor, ErrFn&& err_visitor) const;
		template<class ErrFunc>         auto or_else(ErrFunc&& err_visitor) const;
		
		// operator overloads 
		explicit operator bool() const;
		Result& operator*();
		Result* operator->();
		const Result&  operator*() const;
		const Result*  operator->() const;
	};
}
#include "result.inl"