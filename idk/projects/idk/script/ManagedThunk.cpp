#include "stdafx.h"
#include "ManagedThunk.h"

namespace idk::mono
{
	ManagedThunk::ManagedThunk(MonoMethod* method)
		: thunk{ mono_method_get_unmanaged_thunk(method) }
	{
	}

	void yolo()
	{
		ManagedThunk m{ nullptr };
		Handle<GameObject> go;
		m.Invoke(go, 5, 4);
	}
}
