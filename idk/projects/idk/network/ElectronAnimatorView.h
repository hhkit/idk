#pragma once
#include <core/Component.h>
#include <network/network.h>
namespace idk
{
	class ElectronAnimatorView
		: public Component<ElectronAnimatorView>
	{
	public:
		vector<string> int_params;
		vector<string> float_params;
		vector<string> bool_params;
		vector<string> trigger_params;

		NetworkID GetNetworkID() const;
		Handle<ElectronView> GetView() const;
		void Start(Handle<ElectronView>);

	private:
		Handle<ElectronView> electron_view;
	};
}