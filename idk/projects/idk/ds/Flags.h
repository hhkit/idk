#pragma once
#include <cstdint>
namespace idk
{
	//Copied from vulkan.hpp
	template <typename BitType, typename MaskType = uint32_t>
	class Flags
	{
	public:
		constexpr Flags();

		Flags(BitType bit);

		Flags(Flags<BitType> const& rhs);

		explicit Flags(MaskType flags);

		Flags<BitType>& operator=(Flags<BitType> const& rhs)
		{
			m_mask = rhs.m_mask;
			return *this;
		}

		Flags<BitType>& operator|=(Flags<BitType> const& rhs)
		{
			m_mask |= rhs.m_mask;
			return *this;
		}

		Flags<BitType>& operator&=(Flags<BitType> const& rhs)
		{
			m_mask &= rhs.m_mask;
			return *this;
		}

		Flags<BitType>& operator^=(Flags<BitType> const& rhs)
		{
			m_mask ^= rhs.m_mask;
			return *this;
		}

		Flags<BitType> operator|(Flags<BitType> const& rhs) const
		{
			Flags<BitType> result(*this);
			result |= rhs;
			return result;
		}

		Flags<BitType> operator&(Flags<BitType> const& rhs) const
		{
			Flags<BitType> result(*this);
			result &= rhs;
			return result;
		}

		Flags<BitType> operator^(Flags<BitType> const& rhs) const
		{
			Flags<BitType> result(*this);
			result ^= rhs;
			return result;
		}

		bool operator!() const
		{
			return !m_mask;
		}

		Flags<BitType> operator~() const
		{
			Flags<BitType> result(*this);
			result.m_mask ^= FlagTraits<BitType>::allFlags;
			return result;
		}

		bool operator==(Flags<BitType> const& rhs) const
		{
			return m_mask == rhs.m_mask;
		}

		bool operator!=(Flags<BitType> const& rhs) const
		{
			return m_mask != rhs.m_mask;
		}

		explicit operator bool() const
		{
			return !!m_mask;
		}

		explicit operator MaskType() const
		{
			return m_mask;
		}

	private:
		MaskType  m_mask;
	};
	template<typename BitType, typename MaskType>
	inline constexpr Flags<BitType, MaskType>::Flags()
		: m_mask(0)
	{
	}
	template<typename BitType, typename MaskType>
	Flags<BitType, MaskType>::Flags(BitType bit)
		: m_mask(static_cast<MaskType>(bit))
	{
	}
	template<typename BitType, typename MaskType>
	Flags<BitType, MaskType>::Flags(Flags<BitType> const& rhs)
		: m_mask(rhs.m_mask)
	{
	}
	template<typename BitType, typename MaskType>
	Flags<BitType, MaskType>::Flags(MaskType flags)
		: m_mask(flags)
	{
	}
}