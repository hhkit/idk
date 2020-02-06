#include "pch.h"
#include "IGE_GfxDebugWindow.h"

#include <vkn/TextureTracker.h>

#include <vkn/VknRenderTarget.h>

namespace idk
{
	struct IGE_GfxDebugWindow::Pimpl
	{

	};

	IGE_GfxDebugWindow::IGE_GfxDebugWindow() : IGE_IWindow{"Graphics Debug",false}
	{
	}
	void IGE_GfxDebugWindow::Update()
	{
		vector<std::string> names = {"Texture", "Cubemap", "FontAtlas"};
		auto& trackers = vkn::dbg::TextureTracker::Insts();
		size_t i = 0;
		for (auto& tracker : trackers)
		{
			auto map = tracker.dump();
			string name = names[i] + " allocations: " + std::to_string(map.size());
			if (ImGui::CollapsingHeader(name.c_str()))
			{
				for (const auto& [handle, info] : map)
				{
					ImGui::BulletText("Handle %llx, allocation index: %llx, size: %llu", handle, info.alloc_id, info.size);
				}
			}
			++i;
		}
		bool srgb = RscHandle<RenderTarget>{}->Srgb();
		if (ImGui::Checkbox("Default Render Target SRGB", &srgb))
		{
			RscHandle<RenderTarget>{}->Srgb(srgb);
		}
	}
	IGE_GfxDebugWindow::~IGE_GfxDebugWindow()
	{
	}
}