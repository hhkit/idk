#include "pch.h"
#include "time_log.h"
#include <meta/meta.inl>
#include <ds/span.inl>
namespace idk::vkn::dbg
{
	static inline constexpr bool enabled = false;
	void time_log::log(string_view section, milliseconds duration)
	{
		if constexpr (enabled)
		{
			//Track this in the current level
			if (push_stack.size())
				push_stack.back().emplace_back(nodes.size());
			else
			{
				master_indices.emplace_back(nodes.size());
			}
			//Add it in
			auto& n = nodes.emplace_back(node{ record{ section, duration } });
			//Add popped list in, if applicable.
			if (last_popped)
			{
				n.sub_nodes = std::move(*last_popped);
				last_popped.reset();
			}
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

}
