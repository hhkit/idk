#pragma once
#include <idk.h>
#include <ds/span.h>
#include <vkn/vector_span.h>
#include <vkn/vector_span_builder.h>
#include <vkn/Stopwatch.h>
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
			void timer_start();
			milliseconds timer_end();
			stopwatch timer;
		};
		struct node
		{
			size_t parent = static_cast<size_t>(-1);
			record data;
			vector<size_t> sub_nodes;
			using subnode_index_t = size_t;
			hash_table<string_view, subnode_index_t> existing_subnodes;
		};
		

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

		time_log* push_level(string_view name);
		void pop_level(milliseconds duration= milliseconds{});

		void start_n_store(string name);
		void start(string_view name);
		void end(milliseconds duration);
		void end();
		void end_then_start(string_view name)
		{
			end();
			start(name);
		}



		//time_log* push_level();
		//void pop_level();

		void reset();
		struct record_tree
		{
			span<const node> nodes;
			span<const size_t> indices;
		};
		//record string_views are invalidated upon reset.
		record_tree get_records()const;
	private:
		string_view store(string&& str);
		void log(string_view name, milliseconds duration);
		void log_n_store(string name, milliseconds duration);
		size_t current=static_cast<size_t >(-1);
		size_t get_sub_node_idx(string_view name);
		node& get_sub_node(string_view name);
		node& Curr();
		using indices = vector<size_t>;

		vector<node> nodes;
		vector<size_t> master_indices;
		std::optional<vector<size_t>> last_popped;
		vector<vector<size_t>> push_stack{};

		vector<string> str_owners;
	};
}