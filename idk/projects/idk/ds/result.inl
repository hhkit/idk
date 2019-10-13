#include "result.h"
#pragma once

namespace idk::monadic
{
	template<typename Result, typename Error>
	inline Result& result<Result, Error>::value()
	{
		return std::get<Success>(*this);
	}
	template<typename Result, typename Error>
	template<typename U, typename>
	Result result<Result, Error>::value_or(U&& result) const
	{
		return bool(*this) ? **this : static_cast<Result>(std::forward<U>(result));
	}
	template<typename Result, typename Error>
	inline Error& result<Result, Error>::error()
	{
		return std::get<Failure>(*this);
	}
	template<typename Result, typename Error>
	inline const Result& result<Result, Error>::value() const
	{
		return std::get<Success>(*this);
	}
	template<typename Result, typename Error>
	inline const Error& result<Result, Error>::error() const
	{
		return std::get<Failure>(*this);
	}
	template<typename Result, typename Error>
	inline result<Result, Error>::operator bool() const
	{
		return Base::index() == Success;
	}
	template<typename Result, typename Error>
	inline Result& result<Result, Error>::operator*()
	{
		return value();
	}
	template<typename Result, typename Error>
	inline Result* result<Result, Error>::operator->()
	{
		return &operator*();
	}
	template<typename Result, typename Error>
	const Result& result<Result, Error>::operator*() const
	{
		return value();
	}
	template<typename Result, typename Error>
	const Result* result<Result, Error>::operator->() const
	{
		return &operator*();
	}
}