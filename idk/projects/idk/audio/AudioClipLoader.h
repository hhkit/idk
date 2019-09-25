//////////////////////////////////////////////////////////////////////////////////
//@file		AudioClipLoader.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		18 AUG 2019
//@brief	A factory class that creates AudioClip resources.

//////////////////////////////////////////////////////////////////////////////////


#include <res/FileLoader.h>
#include <audio/AudioClip.h>
namespace idk
{
	class AudioClipLoader
		: public IFileLoader
	{

	public:
		ResourceBundle LoadFile(PathHandle filepath) override;
		ResourceBundle LoadFile(PathHandle filepath, const MetaBundle& metabundle) override;
	};
}