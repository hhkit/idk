#pragma once
#include <core/Component.h>
#include <network/network.h>
#include <network/ElectronView.h>
namespace idk
{
	class ElectronRigidbodyView
		: public Component<ElectronRigidbodyView>
	{
	public:
		bool sync_velocity{true};

		void Start();
		NetworkID GetNetworkID() const;
		Handle<ElectronView> GetView() const;
	private:
		ElectronView::BaseParameter* velocity_param = nullptr;
		Handle<RigidBody> rb;
	};
}