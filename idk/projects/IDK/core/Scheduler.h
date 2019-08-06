#pragma once
#include <idk.h>
#include <idk_config.h>

#include <bitset>
namespace idk
{
	enum class UpdatePhase
	{
		Setup,
		Fixed,
		Render,
	};

	class Scheduler
	{
	public:
		Scheduler();

		template<UpdatePhase phase, typename System, typename ... ComponentSpans>
		void SchedulePass(void(System::*)(ComponentSpans...), const char* name);

		template<UpdatePhase phase, typename System, typename ... ComponentSpans>
		void ScheduleLogicPass(void(System::*)(ComponentSpans...), const char* name);

		void Setup();
		void SequentialUpdate();
		void ParallelizedUpdate();
	private:
		using Lock = std::bitset<ComponentCount>;
		using FnPtr = void(*)();
		Lock _access{};

		struct Pass {
			Lock read_components;
			Lock write_components;
			FnPtr call;
		};

		vector<Pass> _always_update;
		vector<Pass> _fixed_update;
		vector<Pass> _draw_update;
	};
}

#include "Scheduler.inl"