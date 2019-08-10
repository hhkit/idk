#pragma once
#include <idk.h>
#include <core/ISystem.h>

namespace idk
{
	class IEditor
		: public ISystem
	{
	public:
		virtual void EditorUpdate() = 0;
		virtual void EditorDraw() = 0;
	};
}