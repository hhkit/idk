#include "pch.h"
#include "IGE_GfxDebugWindow.h"

#include <imgui/imgui_stl.h>

#include <vkn/TextureTracker.h>

#include <vkn/VknRenderTarget.h>

#include <gfx/GraphicsSystem.h>

#include <gfx/GfxDebugData.h>

namespace idk
{
	struct IGE_GfxDebugWindow::Pimpl
	{
		string new_var = "New Var";
		int index = 0;

		bool show_hidden = false;

		std::string dump;
	};

	IGE_GfxDebugWindow::IGE_GfxDebugWindow() : IGE_IWindow{"Graphics Debug",false}, _pimpl{std::make_unique<Pimpl>()}
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
		RenderExtraVars(Core::GetSystem<GraphicsSystem>().extra_vars);
	}
//#pragma optimize("",off)
	void RenderLifetimeStuff(std::string& dump,const gfxdbg::FgRscLifetimes& lifetimes)
	{
		size_t max_len = 0;
		size_t max_lifetime = 0;
		auto name_func = [](auto& sublifetime)
		{
			return //std::to_string(sublifetime.rsc_id);/*
				sublifetime.rsc_name;//*/
		};
		auto desc_func = [](auto& sublifetime)
		{
			return string{ std::to_string(sublifetime.rsc_id) };/*
				sublifetime.rsc_name;//*/
		};
		for (auto& lifetime : lifetimes)
		{
			for (auto& sublifetime : lifetime)
			{
				auto name = name_func(sublifetime)+desc_func(sublifetime);
				max_len = std::max(max_len, name.length()+2);
				max_lifetime = std::max(max_lifetime, sublifetime.end);
			}
		}
		auto pad = [max_len](string& str)
		{
			str.resize(max_len, '_');
		};

		using cell_line_t = vector<std::pair<string, string>>;
		vector<cell_line_t> cells;

		cells.resize(lifetimes.size(), cell_line_t(max_lifetime+1));
		size_t rsc_index =0;
		for (auto& lifetime : lifetimes)
		{
			for (auto& sublifetime : lifetime)
			{
				for (auto index = sublifetime.start; index <= sublifetime.end; ++index)
				{
					cells[rsc_index][index] = { name_func(sublifetime),desc_func(sublifetime) };
				}
			}
			rsc_index++;
		}
		ImGui::Separator();
		ImGui::Text("Resource Lifetimes: ");
		dump = {};
		for (auto& cell_line : cells)
		{
			for (auto& cell : cell_line)
			{
				auto& [name,desc] = cell;
				pad(name);
				name = "["+name+"]";
				//line += name;
				ImGui::Text("%s", name.c_str());
				if (ImGui::IsItemHovered())
				{
					if(!desc.empty())
						ImGui::SetTooltip(desc.data());
				}
				auto derp = ":" + desc;
				dump += name.replace(name.size()-derp.size()-2,derp.size(),derp);
				ImGui::SameLine();
			}
			ImGui::NewLine();
			dump+="\n";
		}
		ImGui::InputTextMultiline("Resource Lifetimes: ",&dump);
	}

	struct test
	{

		struct DeleteGuard
		{
			bool del = false;
			DeleteGuard(const string& str)
			{
				ImGui::PushID(str.c_str());
				ImGui::Checkbox("Delete", &del);
				ImGui::SameLine();
			}
			operator bool()const {
				return del;
			}
			~DeleteGuard()
			{
				ImGui::PopID();
			}
		};
		bool operator()(string_view name, int& num)
		{
			string str = name;//to ensure that string is delimited
			DeleteGuard del(str);
			if(!del)
				ImGui::DragInt(str.c_str(), &num);
			return static_cast<bool>(del);
		}
		bool  operator()(string_view name, float& num)
		{
			string str = name;//to ensure that string is delimited
			DeleteGuard del(str);
			if (!del)
				ImGui::DragFloat(str.c_str(), &num,std::min(num/13,0.01f));
			return static_cast<bool>(del);
		}
		bool  operator()(string_view name, string& val)
		{
			string str = name;//to ensure that string is delimited
			DeleteGuard del(str);
			if (!del)
			{
				std::string v = val;
				ImGui::InputTextMultiline(str.c_str(), &v);
				val = v;
			}
			return static_cast<bool>(del);
		}
		bool   operator()(string_view name, void* )
		{

			DeleteGuard del(name);
			if (!del)
			{
				ImGui::Text("Unable to display void*");
			}
			return static_cast<bool>(del);
		}
		bool   operator()(string_view name, bool& v)
		{
			string str = name;//to ensure that string is delimited
			DeleteGuard del(str);
			if (!del)
			{
				ImGui::Checkbox(str.c_str(), &v);
			}
			return static_cast<bool>(del);
		}
	}extra_visitor;

	void IGE_GfxDebugWindow::RenderExtraVars(ExtraVars& extra_vars)
	{
		ImGui::Checkbox("Show Hidden: ", &_pimpl->show_hidden);
		for (auto& [name, value] : extra_vars)
		{
			ExtraVars::variant_t& v = value;
			std::variant<string_view> str = name;
			if (!_pimpl->show_hidden &&(!name.empty() || name[0] == '_')) //Skip if name starts with underscore (hidden)
			{
				continue;
			}
			bool delete_var = std::visit(extra_visitor, str,v );
			if (delete_var)
			{
				extra_vars.Unset(name);
			}
		}

		std::string v = _pimpl->new_var;
		ImGui::InputText("New Variable: ", &v);
		_pimpl->new_var = v;
		ImGui::SameLine();
		auto& index = _pimpl->index;
		const char*const names[]                = {"bool","float","string","int"};
		ExtraVars::variant_t default_vals[]= {false,1.0f,"",0};
		//ImGui::BeginCombo("Variable type", names[index]);
		ImGui::Combo("Variable Types", &index, names, static_cast<int>(std::size(names)));
		if (ImGui::Button("Add"))
		{
			extra_vars.Set(_pimpl->new_var, default_vals[_pimpl->index]);
		}
		 
		//Render Extra Things
		auto lifetimev = extra_vars.Get<void*>(gfxdbg::kLifetimeName);
		if (lifetimev)
		{
			auto& lifetime = *reinterpret_cast<const gfxdbg::FgRscLifetimes*>(*lifetimev);
			RenderLifetimeStuff(_pimpl->dump,lifetime);
		}

		//ImGui::EndCombo();
	}
	IGE_GfxDebugWindow::~IGE_GfxDebugWindow()
	{
	}
}