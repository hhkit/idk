#include "pch.h"
#include "IGE_NetworkSettings.h"
#include <network/NetworkSystem.h>
namespace idk
{
	IGE_NetworkSettings::IGE_NetworkSettings()
		: IGE_IWindow{ "Network Settigns##IGE_NetworkSettings", false, ImVec2{ 150,100 }, ImVec2{ 150,100 } }
	{
	}
	void IGE_NetworkSettings::Update()
	{
		ImGui::DragFloat("Latency (s)", &network_time, 0.005f, 0.f, 1.f);
		ImGui::SliderFloat("Packet Loss", &packet_loss, 0.f, 100.f);
		
		if (ImGui::Button("Set Network Simulation"))
		{
			Core::GetSystem<NetworkSystem>().SetLatency(seconds{ network_time });
			Core::GetSystem<NetworkSystem>().SetPacketLoss(packet_loss);
		}
	}

	void IGE_NetworkSettings::BeginWindow()
	{
	}

	void IGE_NetworkSettings::EndWindow_V()
	{
	}
}