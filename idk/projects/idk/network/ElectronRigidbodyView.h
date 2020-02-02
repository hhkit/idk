#pragma once
#include <core/Component.h>
#include <network/network.h>
namespace idk
{
	class ElectronRigidbodyView
		: public Component<ElectronRigidbodyView>
	{
	public:
		bool sync_velocity{true};

		NetworkID GetNetworkID() const;
		Handle<ElectronView> GetView() const;
		void Start();
	};
}