#include <res/ResourceFactory.h>
#include <audio/AudioClip.h>
namespace idk
{
	class AudioClipFactory
		: public ResourceFactory <AudioClip> {

	public:
		virtual unique_ptr<AudioClip> Create() override;
		virtual unique_ptr<AudioClip> Create(string_view filepath) override;
		virtual unique_ptr<AudioClip> Create(string_view filepath, const ResourceMeta&) override;

	};
}