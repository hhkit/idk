#pragma once
#include <idk.h>
#include <idk_config.h>

#include <bitset>
namespace idk
{
	namespace detail { template<typename ... Cs> struct SchedulerHelper; }

	enum class UpdatePhase
	{
		Update,
		Fixed,
		PreRender,
		PostRender,
	};

	class Scheduler
	{
	public:
		Scheduler();

		template<UpdatePhase phase, typename System, typename ... Components>
		void SchedulePass(void (System::* memfn)(span<Components>...), const char* name = "");

		template<UpdatePhase phase, typename System, typename ... Components>
		void ScheduleFencedPass(void(System::*)(span<Components>...), const char* name);

		void Setup();
		void SequentialUpdate();
		void ParallelizedUpdate();
		seconds GetDeltaTime();
		seconds GetRealDeltaTime();
	private:
		using Lock = std::bitset<ComponentCount>;
		using FnPtr = function<void()>;
		Lock       _access{};
		time_point _last_frame;
		time_point _this_frame;
		seconds    _real_dt;
		seconds    _accumulated_dt;
		seconds    _fixed_dt = seconds{ 1.0 / 60 };

		struct Pass {
			Lock read_components;
			Lock write_components;
			FnPtr call;
			string_view update_name;
		};

		template<UpdatePhase phase> void SchedulePass(Pass p);

		vector<Pass> _always_update;
		vector<Pass> _fixed_update;
		vector<Pass> _prerender_update;
		vector<Pass> _postrender_update;

		template<typename ... Ts>
		friend struct detail::SchedulerHelper;
	};
}

#include "Scheduler.inl"