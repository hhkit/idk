#pragma once
#include <idk.h>
#include <idk_config.h>

#include <bitset>
namespace idk
{
	namespace detail { 
		template<typename ... Cs> struct SchedulerHelper;
		template<typename T> struct SchedulerSystemHelper; 
	}

	enum class UpdatePhase
	{
		Update,
		Fixed,
		PreRender,
		PostRender,
	};

	template<typename ... Ts>
	struct PausedSystemConfig {};

	class Scheduler
	{
	public:
		class Pass;

		Scheduler();

		template<UpdatePhase phase, typename System, typename ... Components>
		Pass& SchedulePass(void (System::* memfn)(span<Components>...), const char* name = "");

		template<UpdatePhase phase, typename System, typename ... Components>
		Pass& ScheduleFencedPass(void(System::*)(span<Components>...), const char* name);

		void Setup();
		void SequentialUpdate();
		void ParallelizedUpdate();
		template<typename ... Ts>
		void SetPauseState(PausedSystemConfig<Ts...>);
		seconds GetDeltaTime();
		seconds GetRealDeltaTime();
	private:
		using Lock = std::bitset<ComponentCount>;
		using FnPtr = function<bool()>;
		Lock       _access{};
		time_point _last_frame;
		time_point _this_frame;
		seconds    _real_dt;
		seconds    _accumulated_dt;
		seconds    _fixed_dt = seconds{ 1.0 / 60 };

		vector<Pass> _always_update;
		vector<Pass> _fixed_update;
		vector<Pass> _prerender_update;
		vector<Pass> _postrender_update;

		std::array<bool, SystemCount> _systems_paused{};

		template<UpdatePhase phase> Pass& SchedulePass(Pass&& p);


		template<typename ... Ts>
		friend struct detail::SchedulerHelper;

		template<typename T>
		friend struct detail::SchedulerSystemHelper;
	};

	class Scheduler::Pass {
	public:
		Pass(Lock read, Lock write, FnPtr call, string_view update_name);
		template<typename System, typename ... Components>
		Pass& IfPausedThen(void (System::* memfb)(span<Components>...));
	private:
		Lock read_components;
		Lock write_components;
		FnPtr call;

		Lock paused_read_components;
		Lock paused_write_components;
		FnPtr paused_call;

		string_view update_name;
		friend Scheduler;
	};
}

#include "Scheduler.inl"