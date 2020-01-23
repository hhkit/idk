#pragma once
#include <idk.h>
#include <network/ConnectionManager.h>
#include <network/ClientConnectionManager.inl>
#include <network/ServerConnectionManager.inl>
namespace idk
{
	template<typename T>
	inline T* ConnectionManager::GetServerManager(size_t index)
	{
		auto& man = server_connection_manager[index];
		return man ? man->GetManager<T>() : nullptr;
	}
	template<typename T>
	inline T* ConnectionManager::GetClientManager()
	{
		return client_connection_manager ? client_connection_manager->GetManager<T>() : nullptr;
	}
}