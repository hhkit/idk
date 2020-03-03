#pragma once
#pragma once
#include <core/Component.h>
#include <network/network.h>
namespace idk
{
	class ElectronAnimatorView
		: public Component<ElectronAnimatorView>
	{
	public:
		NetworkID GetNetworkID() const;
		Handle<ElectronView> GetView() const;
		void Start();
	};
}