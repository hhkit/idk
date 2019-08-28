#include "pch.h"
#include "idkTexture.h"

idk::vkn::idkTexture::~idkTexture()
{
	//vknData.reset();
	//mem.reset();
}

idk::vkn::idkTexture::idkTexture(idkTexture&& rhs)
	:size{std::move(rhs.size)},
	sizeOnDevice{std::move(rhs.sizeOnDevice)},
	rawData{ std::move(rhs.rawData) },
	path{std::move(rhs.path)},
	vknData{std::move(rhs.vknData)},
	format{std::move(rhs.format)},
	mem{std::move(rhs.mem)},
	imageView{std::move(rhs.imageView)},
	sampler{std::move(rhs.sampler)}
{}

/*
idk::vkn::idkTexture::idkTexture(const idkTexture& rhs)
	:size{rhs.size},
	sizeOnDevice{rhs.sizeOnDevice},
	rawData{ std::move(rhs.rawData)},
	path{ rhs.path },
	vknData{ std::move(*rhs.vknData) },
	format{ rhs.format },
	mem{ std::move(*rhs.mem) },
	imageView{std::move(*rhs.imageView)},
	sampler{std::move(*rhs.sampler)}
{}
*/
