#include "pch.h"
#include "VknImageData.h"

namespace idk {
	vkn::VknImageData::VknImageData(vk::UniqueImage img)
		:image{std::move(img)},
		format{},
		imageView{nullptr}
	{
	}
	vkn::VknImageData::~VknImageData()
	{
		image.reset();
		imageView.reset();
	}

	vkn::VknImageData::VknImageData(VknImageData&& rhs)
		:image{ std::move(rhs.image) },
		format{ std::move(rhs.format) },
		imageView{ std::move(rhs.imageView) }
	{
	}

	vkn::VknImageData& idk::vkn::VknImageData::operator=(VknImageData&& rhs)
	{
		// TODO: insert return statement here
		std::swap(image, rhs.image);
		std::swap(format, rhs.format);
		std::swap(imageView, rhs.imageView);

		return *this;
	}

	/*vkn::VknImageData::VknImageData(const VknImageData& rhs)
		:image{ rhs.image.get() },
		format{ std::move(rhs.format) },
		imageView{ rhs.imageView.get() }
	{
	}*/

};