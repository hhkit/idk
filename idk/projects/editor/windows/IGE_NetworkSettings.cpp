#include "pch.h"
#include "IGE_NetworkSettings.h"
#include <network/NetworkSystem.h>
#include <network/Client.h>
#include <network/Server.h>
#include <network/ConnectionManager.h>

namespace idk
{
	IGE_NetworkSettings::IGE_NetworkSettings()
		: IGE_IWindow{ "Network Settigns##IGE_NetworkSettings", false, ImVec2{ 150,100 }, ImVec2{ 150,100 } }
	{
	}
	void IGE_NetworkSettings::Update()
	{
		auto& netsys = Core::GetSystem<NetworkSystem>();
		ImGui::DragFloat("Latency (s)", &network_time, 0.005f, 0.f, 1.f);
		ImGui::SliderFloat("Packet Loss", &packet_loss, 0.f, 100.f);

		if (ImGui::Button("Set Network Simulation"))
		{
			Core::GetSystem<NetworkSystem>().SetLatency(seconds{ network_time });
			Core::GetSystem<NetworkSystem>().SetPacketLoss(packet_loss);
		}

		ImGui::Separator();
		if (auto conn_man = Core::GetSystem<NetworkSystem>().GetConnectionTo(idk::Host::SERVER))
		{
			ImGui::Text("To Server");
			ImGui::Text("RTT: %f", conn_man->GetRTT());
			ImGui::Separator();
		}

		for (int i = 0; i < static_cast<int>(idk::Host::CLIENT_MAX); +i)
		{
			if (auto conn_man = Core::GetSystem<NetworkSystem>().GetConnectionTo((idk::Host)i)) 
			{
				ImGui::Text("To Client %i", i);
				ImGui::Text("RTT: %f", conn_man->GetRTT());
				ImGui::Separator();
					
			}
		}
	}

	void IGE_NetworkSettings::BeginWindow()
	{
	}

	void IGE_NetworkSettings::EndWindow_V()
	{
	}
}