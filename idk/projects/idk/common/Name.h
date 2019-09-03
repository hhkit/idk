#pragma once
#include <idk.h>
#include <core/Component.h>

namespace idk
{
	class Name
		:public Component<Name>
	{	
	public:
		string		name{};
	};
};