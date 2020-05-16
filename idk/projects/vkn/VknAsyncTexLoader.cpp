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
	//tex.MarkLoaded(false);
	auto data_ptr = std::make_unique<VknTextureData>();
	_queued.emplace_back(
		OpData{
			{},
		tex_handle,
		std::move(load_info),
		std::move(data_ptr)
		});
	//auto fut = _loader.LoadTextureAsync(*data_ptr, _allocator, _load_fences, _cmd_buffers, load_info.to, load_info.tci, load_info.iti);
	//_results.emplace_back(
	//	OpData{
	////	.get(); 
	//		std::move(fut),
	//	tex_handle,
	//	std::move(load_info),
	//	std::move(data_ptr)
	//	}
	//);
}

void AsyncTexLoader::UpdateTextures()
{
	if (ready && ready->ready())
	{
		//Done processing the last frame's stuff
		ready->get();
		ready.reset();
		size_t i = 0;
		while (i < _results.size())
		{
			auto& [future, handle, ctx, data] = _results.at(i);
			//if (future.ready())
			{
				//future.get();
				if (handle)
				{
					data->ApplyOnTexture(*handle);
					handle->MarkLoaded(true);
				}
				//UpdateHandle(handle, future.get());
			//	i = erase_result(i);
			}
			//else
			//{
			++i;
			//}
		}
		_results.clear();
	}
	else if (ready && !ready->ready())
		return;//currently processing something, don't start another one.
	ProcessFrame();
}

void AsyncTexLoader::ClearQueue()
{
	if (ready)
	{
		ready->get();
		ready.reset();
		_queued.clear();
	}
}

size_t AsyncTexLoader::num_pending() const noexcept
{
	return _queued.size();
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

void AsyncTexLoader::ProcessFrame()
{
	ready = Core::GetThreadPool().Post(
		[](ExecProxy proxy)
		{
			proxy.exec();
		},
		ExecProxy{this}
	);
}

void AsyncTexLoader::ExecProxy::exec()
{
	if (ptr->_queued.empty())
		return;
	dbg::stopwatch timer;
	timer.start();
	do {
		auto& curr = ptr->_queued.back();
		auto fut = ptr->_loader.LoadTextureAsync(*curr.data, ptr->_allocator, ptr->_load_fences, ptr->_cmd_buffers, curr.info.to, curr.info.tci, curr.info.iti);

		if (ptr->_queued.size() > 1)
		{
			auto& curr2 = ptr->_queued.at(ptr->_queued.size()-2);
			auto fut2 = ptr->_loader.LoadTextureAsync(*curr2.data, ptr->_allocator, ptr->_load_fences, ptr->_cmd_buffers, curr2.info.to, curr2.info.tci, curr2.info.iti);
			while (!fut2.ready()) std::this_thread::yield();
			ptr->_results.emplace_back(std::move(curr2));
		}
		while (!fut.ready()) std::this_thread::yield();
		ptr->_results.emplace_back(std::move(curr));
		ptr->_queued.pop_back();
		if (ptr->_queued.size())
			ptr->_queued.pop_back();
		timer.stop();
	} while (timer.time() < ptr->time_slice && ptr->_queued.size());
}

}