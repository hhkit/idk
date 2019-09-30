//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_ShadowMapWindowWindow.cpp
//@author	Tan Heng Chee
//@param	Email : t.hengchee\@digipen.com
//@date		30 SEPT 2019
//@brief	

/*
This window displays the shadow maps.
Derived using IDE_SceneView.cpp as a base/reference
*/
//////////////////////////////////////////////////////////////////////////////////



#include "pch.h"
#include <editor/windows/IGE_ShadowMapWindow.h>
#include <app/Application.h>
#include <common/Transform.h> //transform
#include <gfx/Camera.h> //camera
#include <core/GameObject.h>
#include <gfx/RenderTarget.h>
#include <iostream>
#include <math/euler_angles.h>
#include <gfx/GraphicsSystem.h>
#include <imgui/ImGuizmo.h>
#include <IDE.h>

#include <vkn/VknFramebuffer.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
namespace idk {
	struct IGE_ShadowMapWindow::Pimpl
	{
		struct LightInfo
		{
			const LightData* data;
			string name;
		};
		vector<LightData> buffered_light_data;
		vector<LightInfo> info;
		void UpdateLightInfo()
		{
			if(info.size()< buffered_light_data.size())
				info.resize(buffered_light_data.size());
			for (size_t i = 0; i < buffered_light_data.size(); ++i)
			{
				info[i].name = "Light " + std::to_string(i);
				info[i].data = &buffered_light_data[i];
			}
			
		}
		Pimpl()
		{
		}
	};

	IGE_ShadowMapWindow::IGE_ShadowMapWindow()
		:IGE_IWindow{ "Shadow Map Window",false,ImVec2{ 800,600 },ImVec2{ 0,50 } },
		 _pimpl{std::make_unique<Pimpl>()}
	{		//Delegate Constructor to set window size
			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		//window_flags = ImGuiWindowFlags_NoCollapse;
			//| ImGuiWindowFlags_NoScrollWithMouse;
			//| ImGuiWindowFlags_NoScrollbar;

		//size_condition_flags = ImGuiCond_Always;
		//pos_condition_flags = ImGuiCond_Always;



	}

	void IGE_ShadowMapWindow::BeginWindow()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		//ImGuiID dock_id = ImGui::GetID("IGEDOCKSPACE");
		//ImGui::DockBuilder


	}

	void IGE_ShadowMapWindow::Update()
	{
		ImGui::PopStyleVar(3);
		auto& gfx_sys = Core::GetSystem<GraphicsSystem>();
		gfx_sys.BufferedLightData(_pimpl->buffered_light_data);
		_pimpl->UpdateLightInfo();
		auto& light_info= _pimpl->info;
		int index = std::min(s_cast<int>(light_info.size()), _selected_light);

		int i = 0;
		for (auto& light : light_info)
		{
			//ImGui::RadioButton(light.name.c_str(), &index, i);
			auto& rt = *light.data->light_map; 
			auto& img = *rt.GetAttachment(AttachmentType::eDepth, 0);
			vec2 size = vec2{ img.Size() };
			
			ImGui::Image(img.ID(), size*(this->window_size.y*0.5f/size.y));
			ImGui::NewLine();
		}

	}

	IGE_ShadowMapWindow::~IGE_ShadowMapWindow()
	{
	}


}
