#pragma once
#include <editor/IEditor.h>

namespace idk
{
	class IDE : public IEditor
	{
	public:
		void Init() override;
		void Shutdown() override;
		void EditorUpdate() override;
		void EditorDraw() override;
	};
}