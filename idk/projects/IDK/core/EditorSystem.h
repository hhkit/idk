#pragma once
#include <idk.h>
#include <core/ISystem.h>

namespace idk
{
	class EditorSystem
		: public ISystem
	{
	public:
		virtual void EditorUpdate() = 0;
	};
}