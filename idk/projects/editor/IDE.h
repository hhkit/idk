#pragma once
#include <editor/IEditor.h>
#include <editor/Vulkan_ImGui_Interface.h>

namespace idk
{
	namespace vkn {
		class VulkanState;
	};
	class IDE : public IEditor
	{
	public:
		IDE();

		void Init() override;
		void Shutdown() override;
		void EditorUpdate() override;
		void EditorDraw() override;
	private:
		edt::VI_Interface edtInterface;
	};
}