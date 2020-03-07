#pragma once

#include <vkn/RenderBindings.h>

namespace idk::vkn::bindings
{
	class TransparentBlendBindings : public RenderBindings
	{
	public:
		static inline const std::array<uint32_t, 1> default_attachments{ 0 };
		span<const uint32_t> attachments{ default_attachments };
		void Bind(RenderInterface& context);
	};
}