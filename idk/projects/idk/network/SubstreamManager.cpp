#include "stdafx.h"
#include "SubstreamManager.h"

namespace idk
{
	void BaseSubstreamManager::SetConnectionManager(ConnectionManager* man)
	{ 
		connection_manager = man;
	}
	void BaseSubstreamManager::NetworkFrameStart()
	{ 
		for (auto& elem : frame_start_functions) 
			elem();
	} 
	void BaseSubstreamManager::NetworkFrameEnd()
	{ 
		for (auto& elem : frame_end_functions)
			elem();
	}
}
