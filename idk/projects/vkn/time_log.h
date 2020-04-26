#pragma once
#include <idk.h>
#include <ds/span.h>
#include <vkn/vector_span.h>
#include <vkn/vector_span_builder.h>
namespace idk::vkn::dbg
{
	class time_log
	{
	public:
		using milliseconds=std::chrono::duration<float, std::milli>;
		struct record
		{
			string_view section;
			milliseconds duration;
		};
		struct node
		{
			record data;
			vector<size_t> sub_nodes;
		};
		
		void log_n_store(string name, milliseconds duration);
		void log(string_view name,  milliseconds duration);

		struct push_guard
		{
			time_log* logger;
			push_guard(time_log * log):logger{log}{}
			push_guard(push_guard&& rhs)noexcept:logger{ rhs.logger } { rhs.logger = nullptr; }
			~push_guard()
			{
				if (logger)
					logger->pop_level();
			}
		};

		time_log* push_level()
		{
			push_stack.emplace_back();
			return this;
		}
		void pop_level()
		{
			last_popped = std::move(push_stack.back());
			push_stack.pop_back();
		}

		void reset();
		struct record_tree
		{
			span<const node> nodes;
			span<const size_t> indices;
		};
		//record string_views are invalidated upon reset.
		record_tree get_records()const;
	private:
		using indices = vector<size_t>;

		vector<node> nodes;
		vector<size_t> master_indices;
		std::optional<vector<size_t>> last_popped;
		vector<vector<size_t>> push_stack{};

		vector<string> str_owners;
	};
}