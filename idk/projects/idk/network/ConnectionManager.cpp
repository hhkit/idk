#include "stdafx.h"
#include "ConnectionManager.h"
#include <steam/isteamnetworkingsockets.h>

namespace idk
{
	HSteamNetConnection ConnectionManager::GetHandle() const
	{
		return handle;
	}
	seconds ConnectionManager::GetRTT() const
	{
		SteamNetworkingQuickConnectionStatus status;
		SteamNetworkingSockets()->GetQuickConnectionStatus(handle, &status);

		return duration_cast<seconds>(std::chrono::duration<float, std::milli>(status.m_nPing));
	}
}