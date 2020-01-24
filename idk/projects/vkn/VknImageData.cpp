#include "pch.h"
#include "VknImageData.h"

namespace idk {
	vkn::VknImageData::VknImageData(vk::UniqueImage img)
		:image{std::move(img)},
		format{},
		imageView{nullptr},
		size{}
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
		imageView{ std::move(rhs.imageView) },
		size{std::move(rhs.size)}
	{
	}

	vkn::VknImageData& idk::vkn::VknImageData::operator=(VknImageData&& rhs)
	{
		std::swap(image, rhs.image);
		std::swap(format, rhs.format);
		std::swap(imageView, rhs.imageView);
		std::swap(size, rhs.size);

		return *this;
	}

	/*vkn::VknImageData::VknImageData(const VknImageData& rhs)
		:image{ rhs.image.get() },
		format{ std::move(rhs.format) },
		imageView{ rhs.imageView.get() }
	{
	}*/

};