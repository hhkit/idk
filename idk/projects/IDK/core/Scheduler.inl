#pragma once
namespace idk
{
	template<UpdatePhase phase, typename System, typename ... ComponentSpans>
	void Scheduler::SchedulePass(void(System::*)(ComponentSpans...), const char* name)
	{

	}
}