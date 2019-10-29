#include "stdafx.h"
#include "NullHandleException.h"

namespace idk
{
	NullHandleException::NullHandleException(GenericHandle h)
		: std::exception{ "Null Handle Exception" }, invalid_handle{ h }
	{
	}
	GenericHandle NullHandleException::GetHandle() const
	{
		return invalid_handle;
	}
}
