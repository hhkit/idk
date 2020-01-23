#pragma once
#include <idk.h>
#include <core/Handle.h>
#include <network/network.h>
#include <event/Signal.h>

namespace idk
{
	class ElectronView;

	class IDManager
	{
	public:
		IDManager();
		~IDManager();

		NetworkID CreateNewIDFor(Handle<ElectronView> go);
		bool      EmplaceID(NetworkID id, Handle<ElectronView> ev);

		Handle<ElectronView> GetViewFromId(NetworkID id) const;
	private:
		NetworkID running_count{0};

		hash_table<NetworkID, Handle<ElectronView>> ids;
		vector<Handle<ElectronView>> new_electron_views;
	};
}