#pragma once
#include <exception>

namespace idk
{
	class NullHandleException
		: public std::exception
	{
	public:
		explicit NullHandleException(GenericHandle h);
		GenericHandle GetHandle() const;
	private:
		GenericHandle invalid_handle;
	};
}