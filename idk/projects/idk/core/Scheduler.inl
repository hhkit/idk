#include "Scheduler.h"
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
	}

	template<UpdatePhase phase, typename System, typename ... Cs>
	void Scheduler::SchedulePass(void(System::*mem_fn)(span<Cs>...), const char* name)
	{
		Scheduler::Lock read_bitset = detail::SchedulerHelper<Cs...>::GetReadLock();
		Scheduler::Lock write_bitset = detail::SchedulerHelper<Cs...>::GetWriteLock();
		auto call = [mem_fn]()
		{
			std::invoke(mem_fn, Core::GetSystem<System>(), GameState::GetGameState().GetObjectsOfType<Cs>()...);
		};

		SchedulePass<phase>(Pass{ read_bitset, write_bitset, call, name });
	}

	template<UpdatePhase phase>
	inline void Scheduler::SchedulePass(Pass p)
	{
		switch (phase)
		{
		case UpdatePhase::Update:
			_always_update.emplace_back(p);
			break;
		case UpdatePhase::Fixed:
			_fixed_update.emplace_back(p);
			break;
		case UpdatePhase::PreRender:
			_prerender_update.emplace_back(p);
			break;
		case UpdatePhase::PostRender:
			_postrender_update.emplace_back(p);
		};
	}

	template<UpdatePhase phase, typename System, typename ...Cs>
	inline void Scheduler::ScheduleFencedPass(void(System::*mem_fn)(span<Cs> ...), const char* name)
	{
		Scheduler::Lock all_true;
		all_true.set();

		auto call = [mem_fn]()
		{
			std::invoke(mem_fn, Core::GetSystem<System>(), GameState::GetGameState().GetObjectsOfType<Cs>()...);
		};

		SchedulePass<phase>(Pass{ all_true, all_true, call, name });
	}
}