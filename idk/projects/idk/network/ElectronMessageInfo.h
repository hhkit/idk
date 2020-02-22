#pragma once
#include <network/network.h>
#include <core/Handle.h>

namespace idk
{
	struct ElectronMessageInfo
	{
		int fromID; // host
		Handle<ElectronView> view_handle;
		SeqNo frameStamp;
	};
}