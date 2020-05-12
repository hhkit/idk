#pragma once
#include <idk.h>
namespace idk::vkn::dbg
{
	class time_log;
	struct SubDurations
	{
		hash_table<string_view, float> durations{};
		void clear();
		void add(string_view name, float duration);
	};
	struct SubDurationStack : SubDurations
	{
		vector<SubDurations> sub_durations;

		void push();
		void add(string_view name, float duration);
		SubDurations pop();
		void pop(time_log&);
	private:
		SubDurations& curr();
		//depth, durations
		vector<std::pair<size_t,SubDurations>> _popped_durations;

	};
	SubDurationStack& duration_stack();
}