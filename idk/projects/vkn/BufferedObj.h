#pragma once
#include <idk.h>
#include <vkn/VulkanResourceManager.h>
namespace idk::vkn
{

	class IBufferedObj
	{
	public:
		virtual bool HasUpdate()const = 0;
		//Mark the start of the cycle, switch the current into old and back into current
		//only if there isn't something else in _old, otherwise nothing changes
		virtual void UpdateCurrent(size_t curr_index) = 0;
	};

	template<typename BufferedData>
	class BufferedObj :IBufferedObj
	{
	public:
		bool HasCurrent()const { return s_cast<bool>(_current); }
		BufferedData& Current() { return *_current; }
		const BufferedData& Current()const { return *_current; }
		void WriteToBack(unique_ptr<BufferedData>&& new_data){
			
			((!_current)?_current:_back) = std::move(new_data);
		}
		bool HasUpdate()const{return s_cast<bool>(_back);}
		//Mark the start of the cycle, switch the current into old and back into current
		//only if there isn't something else in _old, otherwise nothing changes
		void UpdateCurrent(size_t curr_index);
	protected:

		ManagedRsc<unique_ptr<BufferedData>> _current;
		ManagedRsc<unique_ptr<BufferedData>> _back;
		ManagedRsc<unique_ptr<BufferedData>> _old;
		size_t _cycle;
	};

}
