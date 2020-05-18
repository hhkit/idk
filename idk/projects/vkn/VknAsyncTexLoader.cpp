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
//static hash_set<RscHandle<VknTexture>> handles;
void AsyncTexLoader::Load(AsyncTexLoadInfo&& load_info, VknTexture& tex, RscHandle<VknTexture> tex_handle)
{
	//tex.MarkLoaded(false);
	auto data_ptr = std::make_unique<VknTextureData>();
	//if (!handles.emplace(tex_handle).second)
	//	return;
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
		//size_t i = 0;
		//while (i < _results.size())
		//{
		//	auto& [future, handle, ctx, data] = _results.at(i);
		for(auto& [future, handle, ctx, data]:  _results)
		{
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
		//	++i;
			//}
		}
		//static vector<OpData> tmp;
		//for (auto& derp : _results)
		//{
		//	tmp.emplace_back(std::move(derp));
		//}
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
		_process_queue.clear();
		//handles.clear();
	}
}

size_t AsyncTexLoader::num_pending() const noexcept
{
	return _queued.size() + _process_queue.size();
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
	auto tail = _process_queue.size();
	_process_queue.resize(tail + _queued.size());

	std::move(_queued.begin(), _queued.end(), _process_queue.begin() + tail);
	_queued.clear();
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
	if (ptr->_process_queue.empty())
		return;
	dbg::stopwatch timer;
	timer.start();
	do {
		auto& curr = ptr->_process_queue.back();
		auto fut = ptr->_loader.LoadTextureAsync(*curr.data, ptr->_allocator, ptr->_load_fences, ptr->_cmd_buffers, curr.info.to, curr.info.tci, curr.info.iti);

		if (ptr->_process_queue.size() > 1)
		{
			auto& curr2 = ptr->_process_queue.at(ptr->_process_queue.size()-2);
			auto fut2 = ptr->_loader.LoadTextureAsync(*curr2.data, ptr->_allocator, ptr->_load_fences, ptr->_cmd_buffers, curr2.info.to, curr2.info.tci, curr2.info.iti);
			while (!fut2.ready() && Core::IsRunning()) std::this_thread::yield();
			//fut2.get();
			ptr->_results.emplace_back(std::move(curr2));
			//handles.erase(curr2.handle);
		}
		//fut.get();
		while (!fut.ready() && Core::IsRunning()) std::this_thread::yield();
		//handles.erase(curr.handle);
		ptr->_results.emplace_back(std::move(curr));
		ptr->_process_queue.pop_back();
		if (ptr->_process_queue.size())
			ptr->_process_queue.pop_back();
		timer.stop();
	} while (timer.time() < ptr->time_slice && ptr->_process_queue.size());
}

}