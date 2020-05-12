#pragma once
#include <idk.h>
#include <idk_config.h>
#include <ds/circular_buffer.h>
#include <bitset>
namespace idk
{
	namespace detail { 
		template<typename ... Cs> struct SchedulerHelper;
		template<typename T> struct SchedulerSystemHelper; 
	}

	enum class UpdatePhase
	{
		FrameStart,
		NetworkPredict,
		MainUpdate,
		Fixed,
		NetworkTickStart,
		NetworkTickEnd,
		PreRender,
		Render,
		MAX
	};

	template<typename ... Ts>
    struct PausedSystemConfig {};

	class Scheduler
	{
	public:
		real time_scale = 1.f;

		class Pass;

		Scheduler();

		template<UpdatePhase phase, typename System, typename ... Components>
		Pass& SchedulePass(void (System::* memfn)(span<Components>...), const char* name = "");

		template<UpdatePhase phase, typename System, typename ... Components>
		Pass& ScheduleFencedPass(void(System::*)(span<Components>...), const char* name);

		void Setup();
		void SequentialUpdate();
		void ResetTimings();
		void ParallelizedUpdate();
		template<typename ... Ts>
		void SetPauseState(PausedSystemConfig<Ts...>);
		seconds GetFixedDeltaTime()noexcept;
		seconds GetRealDeltaTime()noexcept;
		seconds GetPredictedDeltaTime() noexcept { return _fixed_dt; }
		seconds GetUnscaledDeltaTime() noexcept;
		seconds GetRemainingTime() noexcept;
		seconds GetNetworkTick() const noexcept { return _fixed_dt; }
		time_point GetProgramStart() noexcept;
		seconds GetTimeSinceStart() noexcept;

		span<Pass> GetPasses(UpdatePhase) noexcept;
	private:
		using Lock = std::bitset<ComponentCount>;
		using FnPtr = function<bool()>;
		Lock       _access{};
		time_point _program_start;
		time_point _last_frame;
		time_point _this_frame;
		seconds    _real_dt;
		seconds    _accumulated_fixed_dt;
		seconds    _accumulated_game_dt;
		seconds    _accumulated_network_dt;
		seconds    _accumulated_predict_dt;
		seconds    _fixed_dt       = seconds{ 1.0 / 15 };

		vector<Pass> _passes[static_cast<int>(UpdatePhase::MAX)];

		std::array<bool, SystemCount> _systems_paused{};

		template<UpdatePhase phase> Pass& SchedulePass(Pass&& p);


		template<typename ... Ts>
		friend struct detail::SchedulerHelper;

		template<typename T>
		friend struct detail::SchedulerSystemHelper;
	};

	class Scheduler::Pass {
	public:
		struct Call
		{
			seconds time;
			bool paused;
		};
		using PerformanceGraph = circular_buffer < Call, 50>;
		Pass(Lock read, Lock write, FnPtr call, string_view update_name);
		template<typename System, typename ... Components>
		Pass& IfPausedThen(void (System::* memfb)(span<Components>...));

		string_view Name() const;
		const PerformanceGraph& Graph() const;
	private:
		Lock read_components;
		Lock write_components;
		FnPtr call;

		Lock paused_read_components;
		Lock paused_write_components;
		FnPtr paused_call;

		PerformanceGraph previous_frames;

		string_view update_name;
		friend Scheduler;
	};
}
