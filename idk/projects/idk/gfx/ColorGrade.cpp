#include "stdafx.h"
#include "ColorGrade.h"

namespace idk
{
//#pragma optimize("",off)
	template<size_t stride>
	SimpleTexData GenerateDefaultColorGradeTexData(byte default_fill)
	{
		static_assert(stride >= 3, "Stride must be at least 3!");
		SimpleTexData tex_data{};
		constexpr auto square_width = 16u;
		const auto& dim = tex_data.dimensions = uvec2{ square_width * square_width,square_width };
		auto& rgba = tex_data.data;
		rgba.resize(dim.x * dim.y * stride,default_fill);

		for (auto y = square_width; y-- > 0;)
		{
			for (auto x = square_width; x-- > 0;)
			{
				for (auto z = square_width; z-- > 0;)
				{
					const auto offset = (x + z * square_width + y * dim.x) * stride;
					auto derp = x / 15.0f * 255;
					auto u = static_cast<uint8_t>(derp);
					rgba[offset + 0] = static_cast<byte>(u);
					u = static_cast<uint8_t>(y / 15.0f * 255);
					rgba[offset + 1] = static_cast<byte>(u);
					u = static_cast<uint8_t>(z / 15.0f * 255);
					rgba[offset + 2] = static_cast<byte>(u);
				}
			}
		}
		return tex_data;
	}

	Guid GetDefaultColorGradeGuid()
	{
		return Guid{ 0xFADAu,0xFADAu,0xFADAu ,0xFADAu };
	}

	SimpleTexData GenerateRgbDefaultColorGradeTexData(byte default_fill)
	{
		return GenerateDefaultColorGradeTexData<3>(default_fill);
	}

	SimpleTexData GenerateRgbaDefaultColorGradeTexData(byte default_fill)
	{
		return GenerateDefaultColorGradeTexData<4>(default_fill);
	}


}