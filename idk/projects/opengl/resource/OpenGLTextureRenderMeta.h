#pragma once
#include <gfx/TextureRenderMeta.h>
#include <gfx/TextureInternalFormat.h>
namespace idk::ogl
{
	namespace detail
	{
		class ogl_GraphicsFormat
			: public GraphicsFormat
		{
			public:
			static unsigned ToInternal(const TextureInternalFormat& fmt) noexcept;
			static unsigned ToUVMode(const UVMode& uv) noexcept;
			static unsigned ToColor(const ColorFormat& c) noexcept;
			static int ToInputChannels(const InputChannels& ic) noexcept;
			static unsigned ToFilter(const FilterMode& f) noexcept;
		};
	}
}