#pragma once
#include <objbase.h>
#include <idk.h>
#include <string_view>
#include <xhash>

#pragma warning(disable:6328)

namespace idk
{
	using u64 = std::uint_fast64_t;

	struct Guid
	{
		unsigned long  Data1;
		unsigned short Data2;
		unsigned short Data3;
		unsigned char  Data4[8];

		bool operator==(const Guid& other) const
		{
			return std::memcmp(this, &other, sizeof(GUID)) == 0;
		}
		explicit operator string() const
		{
			char guid_cstr[39];
			snprintf(guid_cstr, sizeof(guid_cstr),
				"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
				Data1, Data2, Data3,
				Data4[0], Data4[1], Data4[2], Data4[3],
				Data4[4], Data4[5], Data4[6], Data4[7]);
			return string{ guid_cstr };
		}

		// is valid?
		explicit operator bool() const
		{
			return *reinterpret_cast<const GUID*>(this) != GUID_NULL;
		}

		explicit Guid(const std::string_view& str)
			: Guid{}
		{
			sscanf_s(str.data(),
				"%8x-%4hx-%4hx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
				&Data1, &Data2, &Data3,
				&Data4[0], &Data4[1], &Data4[2], &Data4[3],
				&Data4[4], &Data4[5], &Data4[6], &Data4[7]);
		}

		Guid()
			: Data1{ 0 }, Data2{ 0 }, Data3{ 0 }, Data4{ 0,0,0,0,0,0,0,0 }
		{}

		static inline Guid Make()
		{
			Guid guid;
			auto res = CoCreateGuid((GUID*)& guid); (res);
			return guid;
		}

	private:
		struct Natvis
		{
			struct x4lo { unsigned __int8 v : 4;    unsigned __int8 _ : 4; };
			struct x4hi { unsigned __int8 _ : 4;    unsigned __int8 v : 4; };
			struct x8 { unsigned __int8 _; };
			struct x32 { __int32 _; };
		};
	};

}

// Specialize std::hash
namespace std
{
	template<> struct hash<idk::Guid>
	{
		size_t operator()(const idk::Guid& guid) const noexcept
		{
			const idk::u64* p = reinterpret_cast<const idk::u64*>(&guid);
			std::hash<idk::u64> hash;
			return hash(p[0]) ^ hash(p[1]);
		}
	};
}