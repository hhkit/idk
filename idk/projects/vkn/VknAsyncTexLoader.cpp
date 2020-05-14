#include "pch.h"
#include "VknAsyncTexLoader.h"

namespace idk::vkn
{

	//struct AsyncTexResult
	//{
	//public:
	//	UniqueImage                 _image     {};
	//	UniqueImageView             _image_view{};
	//	vk::UniqueSampler           _sampler   {};
	//	uvec2						_size      {};
	//	vk::Format					_format    {};
	//	vk::ImageAspectFlagBits		_aspects   {};
	//	uint32_t					_layers    {};
	//	vk::ImageSubresourceRange	_range     {};
	//	uint32_t					 mipmap_level = 1;
	//};
void AsyncTexLoader::Load(AsyncTexLoadInfo&& load_info, VknTexture& tex, RscHandle<VknTexture> tex_handle)
{
	tex.MarkLoaded(false);
	auto data_ptr = std::make_unique<VknTextureData>();
	auto fut = _loader.LoadTextureAsync(*data_ptr, _allocator, _load_fences, _cmd_buffers, load_info.to, load_info.tci, load_info.iti);
	_results.emplace_back(
		OpData{
	//	.get(); 
			std::move(fut),
		tex_handle,
		std::move(load_info),
		std::move(data_ptr)
		}
	);
}

void AsyncTexLoader::UpdateTextures()
{
	size_t i = 0;
	while (i<_results.size())
	{
		auto& [future, handle,ctx, data] = _results.at(i);
		if (future.ready())
		{
			future.get();
			if (handle)
			{
				data->ApplyOnTexture(*handle);
				handle->MarkLoaded(true);
			}
			//UpdateHandle(handle, future.get());
			i = erase_result(i);
		}
		else
		{
			++i;
		}
	}
}

//void UpdateHandle(RscHandle<VknTexture> tex, AsyncTexResult&& result)
//{
//	//Only update if the handle is still valid.
//	if (tex)
//	{
//		auto& t = *tex;
//		t.image_    = std::move(result._image);
//		t.imageView = std::move(result._image_view);
//		t.sampler   = std::move(result._sampler);
//		t.range     = result._range;
//		t.size      = result._size;
//		t.format    = result._format;
//		t.img_aspect= result._aspects;  
//		t.Layers(result._layers);  
//		t.MarkLoaded(true);
//	}
//}

size_t AsyncTexLoader::erase_result(size_t i)
{
	size_t back = _results.size() - 1;
	if (i != back)
	{
		std::swap(_results.at(i), _results.at(back));
	}
	_results.resize(back);
	return i;
}

}