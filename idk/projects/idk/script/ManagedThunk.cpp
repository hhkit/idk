#include "stdafx.h"
#include "ManagedThunk.h"

namespace idk::mono
{
	ManagedThunk::ManagedThunk(MonoMethod* method)
		: thunk{  }
	{
		auto res = mono_method_get_unmanaged_thunk(method);
		if (!res)
			throw;
		thunk = res;
	}
}
