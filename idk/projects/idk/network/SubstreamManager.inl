#pragma once
#include <core/core.inl>
#include <core/GameState.h>
#include <network/SubstreamManager.h>

namespace idk
{

	template<typename Subscriber, typename ...Objects>
	inline void BaseSubstreamManager::OnFrameStart(void(Subscriber::* mem_fn)(span<Objects> ...))
	{
		frame_start_functions.emplace_back([this, mem_fn]()
			{
				auto gs = Core::GetGameState();
				std::invoke(mem_fn, static_cast<Subscriber*>(this), gs.GetObjectsOfType<Objects>()...);
			});
	}

	template<typename Subscriber, typename ...Objects>
	inline void BaseSubstreamManager::OnFrameEnd(void(Subscriber::* mem_fn)(span<Objects> ...))
	{
		frame_end_functions.emplace_back([this, mem_fn]()
			{
				auto gs = Core::GetGameState();
				std::invoke(mem_fn, static_cast<Subscriber*>(this), gs.GetObjectsOfType<Objects>()...);
			});
	}
}