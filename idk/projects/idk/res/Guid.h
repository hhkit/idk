#pragma once
#include <compare>
#include <objbase.h>
#undef max
#undef min
#undef GetObject
#include <idk.h>
#include <util/hash_combine.h>

#pragma warning(disable:6328)

namespace idk
{
	struct Guid
	{
		unsigned int   Data1;
		unsigned short Data2;
		unsigned short Data3;
		unsigned char  Data4[8];

		constexpr Guid() noexcept;
		explicit  Guid(const std::string_view& str) noexcept;
		constexpr Guid(unsigned int a, unsigned short b, unsigned short c, unsigned long long d);
		auto operator<=>(const Guid&) const noexcept = default;
		//bool operator==(const Guid& other) const noexcept;

		explicit operator string() const;
		// is valid?
		explicit operator bool() const noexcept;
		static inline Guid Make();


	private:
		struct Natvis
		{
			struct x4lo { unsigned __int8 v : 4;    unsigned __int8 _ : 4; };
			struct x4hi { unsigned __int8 _ : 4;    unsigned __int8 v : 4; };
			struct x8 { unsigned __int8 _; };
			struct x32 { __int32 _; };
		};
	};

	// exposing default guid constructor
	constexpr Guid::Guid() noexcept
		: Data1{ 0 }, Data2{ 0 }, Data3{ 0 }, Data4{ 0,0,0,0,0,0,0,0 }
	{}
}

// Specialize std::hash
namespace std
{
	template<> struct hash<idk::Guid>
	{
		size_t operator()(const idk::Guid& guid) const noexcept
		{
			const size_t* p = reinterpret_cast<const size_t*>(&guid);
			size_t seed = 0;
			idk::hash_combine(seed, p[0]);
			idk::hash_combine(seed, p[1]);
			return seed;
		}
	};
}