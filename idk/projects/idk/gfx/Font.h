#pragma once
#include <idk.h>
#include <core/Component.h>
#include <util/enum.h>
#include <gfx/FontAtlas.h>
#include <gfx/FontData.h>

namespace idk {
	class Font
		: public Component<Font>
	{
	public:

		real lineSpacing = 0.1f;
		//ASCII STARTOFFSET
		real tracking = 1.f;
		real spacing = 10.f;
		real padding = 1.f;
		color color = {1.f,1.f,1.f,1.f};
		int  fontSize = 48;
		RscHandle<FontAtlas> textureAtlas = FontAtlas::defaults[FontDefault::SourceSansPro];

		string text;

    private:
		virtual void* ID() const { return 0; };
	};
}