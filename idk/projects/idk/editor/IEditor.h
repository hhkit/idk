#pragma once
#include <idk.h>
#include <core/ISystem.h>

namespace idk
{
	class IEditor
		: public ISystem
	{
	public:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void EditorUpdate() = 0;
		virtual void EditorDraw() = 0;
	};
}