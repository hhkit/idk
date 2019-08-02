#pragma once
#include <string>

#include "Handleable.h"

namespace idk
{
	class Entity : public Handleable<Entity>
	{
		std::string s{"thisisawayfortestinthedestructorcall"};
	public:
		Entity() = default;
	};
}