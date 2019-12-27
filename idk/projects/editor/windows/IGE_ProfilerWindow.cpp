#include "pch.h"
#include "IGE_ProfilerWindow.h"
#include <core/Scheduler.h>
#include <ds/circular_buffer.inl>

namespace idk
{
	IGE_ProfilerWindow::IGE_ProfilerWindow()
		:IGE_IWindow{ "Profiler##IGE_ProfilerWindow",false,ImVec2{ 300,600 },ImVec2{ 450,150 } } 
	{		//Delegate Constructor to set window size
			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		window_flags = ImGuiWindowFlags_NoCollapse;
	}

	void IGE_ProfilerWindow::BeginWindow()
	{
	}

	void IGE_ProfilerWindow::Update()
	{
		auto display_plots = [](string_view phase_name, UpdatePhase phase)
		{
			char curr_fps[32] = {};
			auto passes = Core::GetScheduler().GetPasses(phase);
			if (passes.size())
				ImGui::Text(phase_name.data());

			for (auto& pass : passes)
			{
				vector<float> history;
				auto& graph = pass.Graph();
				history.reserve(graph.size());
				auto last = 0.f;
				bool paused = false;
				for (auto& pt : graph)
				{
					last = pt.time.count();
					history.push_back(last);
					paused = pt.paused;
				}
				if (!paused)
					sprintf_s(curr_fps, "%.5f s", last);
				else
					sprintf_s(curr_fps, "(PAUSED) %.5f s", last);
				ImGui::PlotHistogram(pass.Name().data(), history.data(), static_cast<int>(history.size()), 0, curr_fps, 0.f, .025f);
			};
		};

		display_plots("Fixed Update", UpdatePhase::Fixed);
		display_plots("Always Update", UpdatePhase::MainUpdate);
		display_plots("Prerender", UpdatePhase::PreRender);
		display_plots("Render", UpdatePhase::Render);
	}
}