#include "pch.h"
#include "time_log.h"
#include <meta/meta.inl>
#include <ds/span.inl>
namespace idk::vkn::dbg
{
	static inline constexpr bool enabled = true;
	string_view time_log::store(string&& name)
	{

		string_view section;
		auto& str = str_owners.emplace_back(std::move(name));
		section = str;
		return section;
	}
	void time_log::log(string_view section, milliseconds duration)
	{
		if constexpr (enabled)
		{
			//Track this in the current level
			//if (push_stack.size())
			//	push_stack.back().emplace_back(nodes.size());
			//else
			if(current<nodes.size() && nodes.at(current).parent>nodes.size())
			{
				master_indices.emplace_back(nodes.size());
			}
			//Ensure that the refs remain stable within the next ops
			record& sub_node = get_sub_node(section).data;
			sub_node.duration += duration;
			////Add popped list in, if applicable.
			//if (last_popped)	
			//{
			//	n.sub_nodes = std::move(*last_popped);
			//	last_popped.reset();
			//}
		}
	}
	void time_log::log_n_store(string name, milliseconds duration)
	{
		if constexpr (enabled)
		{

			string_view section;
			auto& str = str_owners.emplace_back(std::move(name));
			section = str;
			log(section, duration);
		}

	}
	time_log* time_log::push_level(string_view name)
	{
		current = get_sub_node_idx(name);
		return this;
	}
	void time_log::pop_level(milliseconds duration)
	{
		node& curr = Curr();
		curr.data.duration += duration;
		current = curr.parent;
		//last_popped = std::move(push_stack.back());
		//push_stack.pop_back();
	}
	void time_log::start_n_store(string name)
	{
		start(store(std::move(name)));

	}
	void time_log::start(string_view name)
	{
		push_level(name);
		Curr().data.timer_start();
	}
	void time_log::end(milliseconds duration)
	{
		pop_level(duration);
	}
	void time_log::end()
	{
		end(Curr().data.timer_end());
	}
	void time_log::reset()
	{
		if constexpr (enabled)
		{
			nodes.clear();
			master_indices.clear();
			last_popped.reset();
			push_stack.clear();

			str_owners.clear();
		}
	}

	time_log::record_tree time_log::get_records() const
	{
		if constexpr (enabled)
			return record_tree{nodes,master_indices};
		else
			return record_tree{ };
	}

	time_log::node& time_log::get_sub_node(string_view name)
	{
		return nodes.at(get_sub_node_idx(name));
	}

	time_log::node& time_log::Curr()
	{
		return nodes.at(current);
	}

	size_t time_log::get_sub_node_idx(string_view name)
	{
		if (nodes.size()>current)
		{
			node& curr = Curr();
			auto& existing = curr.existing_subnodes;
			auto itr = existing.find(name);
			if (itr != existing.end())
			{
				return curr.sub_nodes.at(itr->second);
			}
			curr.existing_subnodes.emplace(name, curr.sub_nodes.size());
			curr.sub_nodes.emplace_back(nodes.size());
		}
		auto result = nodes.size();
		nodes.emplace_back(node{ static_cast<size_t>(current),record{name} });
		if (current > nodes.size())
		{
			master_indices.emplace_back(result);
		}
		return result;
	}

	void time_log::record::timer_start()
	{
		timer.start();
	}

	time_log::milliseconds time_log::record::timer_end()
	{
		timer.stop();
		return timer.time();
	}

}
