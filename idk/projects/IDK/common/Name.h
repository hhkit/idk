#pragma once
#include <idk.h>
#include <core/Component.h>

namespace idk
{
	class Name
		:public Component<Name>
	{	
	public:

		//Can be subjected to change
		void		replace(const string_view&);
		string_view&	modify();

		string_view		name()const;
	private:
		string_view		_name{};
	};
};