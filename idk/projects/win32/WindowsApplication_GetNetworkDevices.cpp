#include "pch.h"
#include "idk.h"
#include "WindowsApplication.h"
#include "WindowsSocket.h"
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
namespace idk
{
	string adapter_to_short_name(PIP_ADAPTER_ADDRESSES aa)
	{
		char buf[BUFSIZ];
		memset(buf, 0, BUFSIZ);
		WideCharToMultiByte(CP_ACP, 0, aa->FriendlyName, (int) wcslen(aa->FriendlyName), buf, BUFSIZ, NULL, NULL);
		return buf;
	}

	string adapter_to_device_name(PIP_ADAPTER_ADDRESSES aa)
	{
		char buf[BUFSIZ];
		memset(buf, 0, BUFSIZ);
		WideCharToMultiByte(CP_ACP, 0, aa->Description, (int) wcslen(aa->Description), buf, BUFSIZ, NULL, NULL);
		return buf;
	}

	std::optional<Address> adapter_to_address(PIP_ADAPTER_UNICAST_ADDRESS ua)
	{
		char buf[BUFSIZ];

		int family = ua->Address.lpSockaddr->sa_family;
		memset(buf, 0, BUFSIZ);
		getnameinfo(ua->Address.lpSockaddr, ua->Address.iSockaddrLength, buf, sizeof(buf), NULL, 0, NI_NUMERICHOST);
		if (family == AF_INET) // IPV4
		{
			in_addr addr;
			if (InetPtonA(family, buf, &addr) != 1)
				return {};
			return Address{ addr.S_un.S_un_b.s_b1, addr.S_un.S_un_b.s_b2,addr.S_un.S_un_b.s_b3 ,addr.S_un.S_un_b.s_b4 };
		}
		if (family == AF_INET6) // IPV6
		{
			in6_addr addr;
			if (InetPtonA(family, buf, &addr) != 1)
				return {};

		}
		return {};
	}

	vector<Device> Windows::GetNetworkDevices()
	{
		vector<Device> devices;

		PIP_ADAPTER_INFO pAdapterInfo;
		PIP_ADAPTER_INFO pAdapter = NULL;
		DWORD dwRetVal = 0;
		[[maybe_unused]] UINT i;

		ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
		pAdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));

		if (pAdapterInfo == NULL)
			printf("Error allocating memory need to call GetAdaptersInfo");

		if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
			free(pAdapterInfo);
			pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
		}

		if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
			pAdapter = pAdapterInfo;
			while (pAdapter)
			{
				Device d;
				d.name = pAdapter->AdapterName;
				d.description = pAdapter->Description;

				{ // find IP addresses
					Address addr;
					for (auto ip = &pAdapter->IpAddressList; ip; ip = ip->Next)
					{
						if (string_view(ip->IpAddress.String) == "0.0.0.0")
							continue;

						if (inet_pton(AF_INET, ip->IpAddress.String, &addr) == 1)
						{
							d.ip_addresses.emplace_back(addr);
						}
					}
					
					if (d.ip_addresses.size())
						devices.emplace_back(d);
				}
				pAdapter = pAdapter->Next;
			}
		}

		if (pAdapterInfo)
			free(pAdapterInfo);
		return devices;
	}

	unique_ptr<Socket> win::Windows::CreateSocket()
	{
		return std::make_unique<WindowsSocket>();
	}

}