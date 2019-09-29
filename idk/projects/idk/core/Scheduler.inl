#include "Scheduler.h"
#include <core/Core.h>
#pragma once
namespace idk
{
	namespace detail
	{
		template<typename ... Cs>
		struct SchedulerHelper
		{
			using lock_t = Scheduler::Lock;

			static lock_t GetReadLock()
			{
				lock_t retval{};
				auto v = ((retval[index_in_tuple_v<std::decay_t<Cs>, Components>] = true) && ...); (v);
				return retval;
			}

			static lock_t GetWriteLock()
			{
				lock_t retval{};
				auto v = ((retval[index_in_tuple_v<std::decay_t<Cs>, Components>] = !std::is_const_v<Cs>) && ...); (v);
				return retval;
			}
		};

		template<typename T>
		struct SchedulerSystemHelper;

		template<typename ... Ss>
		struct SchedulerSystemHelper<std::tuple<Ss...>>
		{
			template<typename ... PausedSs>
			static constexpr auto PauseSystemFn()
			{
				return std::array<void(*)(Scheduler*), SystemCount>{
					[](Scheduler* scheduler)
					{
						scheduler->_systems_paused[SystemID<Ss>] = index_in_tuple_v<Ss, std::tuple<PausedSs...>> != sizeof...(PausedSs);
					}...
				};
			}
		};
	}

	template<UpdatePhase phase, typename System, typename ... Cs>
	Scheduler::Pass& Scheduler::SchedulePass(void(System::*mem_fn)(span<Cs>...), const char* name)
	{
		Scheduler::Lock read_bitset  = detail::SchedulerHelper<Cs...>::GetReadLock();
		Scheduler::Lock write_bitset = detail::SchedulerHelper<Cs...>::GetWriteLock();
		auto call = [mem_fn, this]()
		{
			auto sys = &Core::GetSystem<System>();
			const auto is_paused = _systems_paused[SystemID<System>];

			if (sys && !is_paused)
			{
				std::invoke(mem_fn, sys, GameState::GetGameState().GetObjectsOfType<Cs>()...);
				return true;
			}
			else
				return false;
		};

		return SchedulePass<phase>(Pass{ read_bitset, write_bitset, call, name });
	}

	template<typename ...Ts>
	inline void Scheduler::SetPauseState(PausedSystemConfig<Ts...>)
	{
		constexpr static auto table = detail::SchedulerSystemHelper<Systems>::PauseSystemFn<Ts...>();
		for (auto& elem : table)
			elem(this);
	}

	template<UpdatePhase phase>
	Scheduler::Pass& Scheduler::SchedulePass(Pass&& p)
	{
		switch (phase)
		{
		default:
		case UpdatePhase::Update:     return _always_update    .emplace_back(std::move(p));
		case UpdatePhase::Fixed:      return _fixed_update     .emplace_back(std::move(p));
		case UpdatePhase::PreRender:  return _prerender_update .emplace_back(std::move(p));
		case UpdatePhase::PostRender: return _postrender_update.emplace_back(std::move(p));
		};
	}

	template<UpdatePhase phase, typename System, typename ...Cs>
	Scheduler::Pass& Scheduler::ScheduleFencedPass(void(System::*mem_fn)(span<Cs> ...), const char* name)
	{
		Scheduler::Lock all_true;
		all_true.set();

		auto call = [mem_fn, this]()
		{
			auto sys = &Core::GetSystem<System>();
			const auto is_paused = _systems_paused[SystemID<System>];
			if (sys && !is_paused)
			{
				std::invoke(mem_fn, sys, GameState::GetGameState().GetObjectsOfType<Cs>()...);
				return true;
			}
			else
				return false;
		};

		return SchedulePass<phase>(Pass{ all_true, all_true, call, name });
	}

	template<typename System, typename ...Components>
	Scheduler::Pass& Scheduler::Pass::IfPausedThen(void(System::** memfb)(span<Components>...))
	{
		Scheduler::Lock read_bitset = detail::SchedulerHelper<Components...>::GetReadLock();
		Scheduler::Lock write_bitset = detail::SchedulerHelper<Components...>::GetWriteLock();
		auto call = [memfb]()
		{
			auto sys = &Core::GetSystem<System>();
			if (sys)
			{
				std::invoke(memfb, sys, GameState::GetGameState().GetObjectsOfType<Components>()...);
				return true;
			}
			return false;
		};
		paused_read_components = read_bitset;
		paused_write_components = write_bitset;
		paused_call = call;
	}
}