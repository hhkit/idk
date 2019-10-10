#pragma once
#include <mono/jit/jit.h>

namespace idk::mono
{
	class ManagedThunk
	{
	public:
		ManagedThunk(MonoMethod* method);

		template<typename ... Args>
		MonoObject* Invoke(Args&& ...);
	private:
		void* thunk;
	};
}