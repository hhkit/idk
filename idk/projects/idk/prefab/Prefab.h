#pragma once

#include <idk.h>
#include <res/Resource.h>

namespace idk
{
	class Prefab : public Resource<Prefab>
	{
	public:


	private:
		vector<reflect::dynamic> _components;
		vector<Prefab> _children;
	};
}