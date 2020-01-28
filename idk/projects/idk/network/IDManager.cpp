#include "stdafx.h"
#include "IDManager.h"
#include <core/GameState.h>
#include <network/ElectronView.h>

namespace idk
{
	IDManager::IDManager()
	{
	}

	IDManager::~IDManager()
	{
	}

	NetworkID IDManager::CreateNewIDFor(Handle<ElectronView> go)
	{
		do
		{
			++running_count;
		} while (ids.find(running_count) != ids.end());
		ids[running_count] = go;
		go->network_id = running_count;
		return running_count;
	}

	bool IDManager::EmplaceID(NetworkID id, Handle<ElectronView> ev)
	{
		if (id == 0)
			return false;

		auto itr = ids.find(id);
		if (itr != ids.end())
			return false;

		ids[id] = ev;
		ev->network_id = id;
		return true;
	}
	Handle<ElectronView> IDManager::GetViewFromId(NetworkID id) const
	{
		auto itr = ids.find(id);
		
		return itr != ids.end() ? itr->second : Handle<ElectronView>{};
	}
}
