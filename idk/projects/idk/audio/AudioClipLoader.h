//////////////////////////////////////////////////////////////////////////////////
//@file		AudioClipLoader.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		18 AUG 2019
//@brief	A factory class that creates AudioClip resources.

//////////////////////////////////////////////////////////////////////////////////


#include <res/ResourceFactory.h>
#include <res/EasyFactory.h>
#include <audio/AudioClip.h>
namespace idk
{
	class AudioClipFactory
		: public EasyFactory<AudioClip>
	{
	public:
		unique_ptr<AudioClip> Create(PathHandle h) override;
	};
}