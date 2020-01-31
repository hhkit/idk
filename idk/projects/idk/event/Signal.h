/******************************************************************************/
/*!
\project	SientEngine
\file		Signal.h
\author		Hoe Kau Wee, Malody
\brief
Basic signal/slot implementation.

All content (C) 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once
#include <idk.h>

namespace idk
{

	// All Signals are derived from SignalBase.
	// This by itself is useless -- user needs to cast it manually to Signal<T>.
	class SignalBase
	{
	public:
		using SlotId = unsigned;
		virtual ~SignalBase() {}
	};

	// Basic signal/slot mechanism. Args... are the signal arguments.
	template<typename... Params>
	class Signal : public SignalBase
	{
	public:
		// The function signature, which is: void Fn(Args...)
		using Fn = function<void(Params...)>;


		size_t ListenerCount() const { return _slots.size(); }

		// Listens for this signal. Pass in a function of signature: void fn(Ts...)
		// Returns a SlotId, unique only for this signal. Use it to unlisten.
		template<typename Func>
		SlotId Listen(Func&& f);

		// Unlistens the slot with the given id.
		void Unlisten(SlotId id);

		// Emit the signal with the given data, to all the listeners.
		template<typename ... Args>
		void Fire(Args&&... args);

		template<typename Func>
		SlotId operator+=(Func&& f) { return Listen(std::forward<Func>(f)); }
		void   operator-=(SlotId id) { Unlisten(id); }
	private:
		hash_table<SlotId, Fn> _slots;
		vector<SlotId> _removeus;
		SlotId _next_id = 0;
	};
}
