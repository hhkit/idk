#include "pch.h"
#include "IGE_GfxDebugWindow.h"

#include <imgui/imgui_stl.h>

#include <vkn/TextureTracker.h>

#include <vkn/VknRenderTarget.h>

#include <gfx/GraphicsSystem.h>

#include <gfx/GfxDebugData.h>

#include "IDE.h"
#include <gfx/ColorGrade.h>

#include <editor/imguidk.h>

#include <vkn/VulkanWin32GraphicsSystem.h>
#include <vkn/time_log.h>

//#pragma optimize("",off)
namespace idk
{
	class TGAWriter
	{
	public:
		static vector<byte> SaveTGA(const SimpleTexData& data);
	private:
#pragma pack(push,1)
		struct ColorMapSpec
		{
			uint16_t  first_entry_index{};
			uint16_t  color_map_length{};
			uint8_t   color_map_entry_size{};
		};
		struct ImageSpec
		{
			uint16_t x_origin{};
			uint16_t y_origin{};
			uint16_t width{};
			uint16_t height{};
			uint8_t  bits_per_pixel{};
			uint8_t  image_descriptor{};
		};
		struct TGAHeader
		{
			uint8_t id_length{};
			uint8_t color_map_type{};
			uint8_t img_type{};
			ColorMapSpec cm_spec{};
			ImageSpec img_spec{};
		};
#pragma pack(pop)
	};
	void MapRgbToBgr(void* data, size_t len,uint8_t stride = 3)
	{
		auto ptr = reinterpret_cast<unsigned char*>(data);
		for (auto end = ptr+len;ptr+stride<=end;ptr+=stride)
		{
			std::swap(*(ptr + 0), *(ptr + 2));
		}
	}
	vector<byte> TGAWriter::SaveTGA(const SimpleTexData& data)
	{
		vector<byte> tga_data(data.data.size() + sizeof(TGAHeader));
		const auto& dim = data.dimensions;
		constexpr uint8_t kUncompressedRGB = 2;

		[[maybe_unused]]auto& header = *new (tga_data.data()) TGAHeader{ 0ui8,0ui8,kUncompressedRGB,ColorMapSpec{0,0,0},ImageSpec{0,0,static_cast<uint16_t>(dim.x),static_cast<uint16_t>(dim.y),24,1<<5} };
		static_assert(sizeof(TGAHeader) == 18, "TGA header has been padded.");
		auto ptr = tga_data.data() + sizeof(TGAHeader);
		std::memcpy(ptr, data.data.data(), data.data.size());
		MapRgbToBgr(ptr, data.data.size());
		return tga_data;
	}
}
namespace idk
{
	struct RtlnData
	{
		float align_x;
		float total;

	};
	template<typename Nodes>
	void RenderTimeLogNode(const RtlnData& extra,const Nodes& nodes, const vkn::dbg::time_log::node& curr, string& offset)
	{
		auto& record = curr.data;
		string txt = record.section;
		ImGui::Text("%s%s", offset.c_str(),txt.c_str());
		ImGui::SameLine(extra.align_x);
		auto duration = record.duration.count();
		ImGui::Text("\t%8.3f ms \t %.3f%%", duration, 100 * duration / extra.total);
		offset += "\t";
		for (auto index : curr.sub_nodes)
		{
			RenderTimeLogNode(extra,nodes, nodes[index], offset);
		}
		offset.pop_back();
	}

	void RenderTimeLog(vkn::dbg::time_log& log)
	{
		
		if (ImGui::CollapsingHeader("Time log"))
		{
			auto&& [nodes,master_indices] = log.get_records();
			
			size_t max_chars = 0;
			if (!nodes.size())
				return;
			float total = nodes[master_indices.back()].data.duration.count();
			{
				vector<size_t> indices{ master_indices.begin(),master_indices.end() };
				std::reverse(indices.begin(), indices.end());
				size_t last_max = indices.size();
				size_t i = 0;
				while (!indices.empty())
				{
					if (i++ == last_max)
					{
						max_chars += 2;//tablen
						last_max = indices.size();
					}
					auto index = indices.back();
					indices.pop_back();
					auto& node = nodes[index];
					auto& record = node.data;
					indices.insert(indices.end(), node.sub_nodes.begin(), node.sub_nodes.end());

					max_chars = std::max(record.section.size(), max_chars);
				}
			}
			string txt(max_chars,'A');
			auto max_size = ImGui::CalcTextSize(txt.c_str());
			string offset;
			RtlnData extra{ max_size.x,total };
			for (auto& index : master_indices)
			{
				RenderTimeLogNode(extra,nodes, nodes[index], offset);
			}
			/*
			for (auto& record : records)
			{
				txt = record.section;
				ImGui::Text("%s", txt.c_str());
				ImGui::SameLine(max_size.x);
				auto duration = record.duration.count();
				ImGui::Text("%8.3f ms \t %.3f%%", duration, 100*duration/total );
			}*/
		}
		log.reset();
	}

	struct IGE_GfxDebugWindow::Pimpl
	{
		string new_var = "New Var";
		int index = 0;

		bool show_hidden = false;

		RscHandle<Texture> selected_tex = {};

		std::string lut_path = "../default_lut.tga";
		std::string dump;
	};

	IGE_GfxDebugWindow::IGE_GfxDebugWindow() : IGE_IWindow{ "Graphics Debug",false,ImVec2{1920/4.5f,650.0f} }, _pimpl{ std::make_unique<Pimpl>() }
	{
	}
	void IGE_GfxDebugWindow::Update()
	{
		{
			RscHandle<Texture>& tex = _pimpl->selected_tex;
			ImGuidk::InputResource("Texture Selection:", &tex);
			if (ImGui::Button("Select Texture"))
				Core::GetSystem<IDE>().SelectAsset(tex);
		}

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
		auto& ide = Core::GetSystem<IDE>();
		if (ImGui::Button("Select Default Render Target"))
		{
			ide.SelectAsset(RscHandle<RenderTarget>{}, false, true);
		}
		if (ImGui::Button("Select Editor Render Target"))
		{
			ide.SelectAsset(ide.GetEditorCamera()->render_target, false, true);
		}
		if (ImGui::CollapsingHeader("More Gfx Debug Stuff"))
		{
			RenderTimeLog(Core::GetSystem<vkn::VulkanWin32GraphicsSystem>().TimeLog());
			ImGui::InputText("LUT Save path: ", &_pimpl->lut_path);
			if (ImGui::Button("Save Default Color Grade LUT"))
			{
				auto cg_lut = GenerateRgbDefaultColorGradeTexData();
				auto data = TGAWriter::SaveTGA(cg_lut);
				std::ofstream file{ _pimpl->lut_path,std::ios::binary | std::ios::out | std::ios::trunc };
				if(file)
					file<<std::string{ reinterpret_cast<const char*>(data.data()), std::size(data) }<<std::flush;
				file.close();
			}
			RenderExtraVars(Core::GetSystem<GraphicsSystem>().extra_vars);
		}
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
		{
			auto tmp = extra_vars.Get<void*>("445");
			if (tmp)
			{
				ImGui::Image(*tmp, ImVec2{ 100,100 });
			}
		}

		if (!ImGui::CollapsingHeader("ExtraVars"))
			return;
		ImGui::Checkbox("Show Hidden: ", &_pimpl->show_hidden);
		for (auto& [name, value] : extra_vars)
		{
			ExtraVars::variant_t& v = value;
			std::variant<string_view> str = name;
			if (!_pimpl->show_hidden &&(name.empty() || name[0] == '_')) //Skip if name starts with underscore (hidden)
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