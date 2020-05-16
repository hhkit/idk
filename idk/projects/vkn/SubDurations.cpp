#include "pch.h"
#if 0
#include "SubDurations.h"
#include "time_log.h"
namespace idk::vkn::dbg
{	void SubDurationStack::push()
	{
		sub_durations.emplace_back();
	}
	void SubDurationStack::add(string_view name, float duration)
	{
		curr().add(name, duration);
	}
	SubDurations SubDurationStack::pop()
	{
		auto result = sub_durations.back();
		sub_durations.pop_back();
		return result;
	}
	void SubDurationStack::pop(time_log& log)
	{
		auto&& back = pop();
		for (auto& [name, duration] : back.durations)
		{
			log.log(name, time_log::milliseconds{ duration });
		}
	}
	SubDurations& SubDurationStack::curr()
	{
		return sub_durations.back();
	}
	void SubDurations::add(string_view name, float duration)
	{
		durations[name] += duration;
	}
	void SubDurations::clear()
	{
		for (auto& [name, duration] : durations)
		{
			name;
			duration = 0;
		}
	}
	SubDurationStack& duration_stack()
	{
		static SubDurationStack persist{};
		return persist;
	}

}
#endif