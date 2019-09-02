#include "stdafx.h"
#include "Name.h"

namespace idk
{
	void Name::replace(const string_view& n_str)
	{
		_name = n_str;
	}
	string_view& Name::modify()
	{
		// TODO: insert return statement here
		return _name;
	}
	string_view Name::name() const
	{
		return _name;
	}

};