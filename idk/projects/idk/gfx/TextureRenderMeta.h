#pragma once
#include <idk.h>
#include <util/enum.h>

namespace idk{

	// for import only
	// if you need a more specific format, call the constructor directly
	ENUM(ColorFormat, char
		, Automatic
		, Alpha_8
		, RGB_24bit
		, RGBA_32bit
		, RGB_16bit
		, R_16bit
		, R_8
	);

	ENUM(UVMode, char,
		Repeat,
		MirrorRepeat,
		Clamp,
		ClampToBorder
	);

	ENUM(FilterMode, char,
		Linear,
		Nearest,
		Cubic
	);

	ENUM(DepthBufferMode, char
		, NoDepth
		, Depth16
		, Depth2432
		);

	ENUM(TextureTarget, int,
		PosX, NegX,
		PosY, NegY,
		PosZ, NegZ
	);

	class GraphicsFormat {
	public:
		//GraphicsFormat() = default;
		virtual ~GraphicsFormat() = default;
		virtual unsigned ToUVMode(const UVMode& uv) = 0;
		virtual unsigned ToColor(const ColorFormat& c) = 0;
		virtual unsigned ToFilter(const FilterMode& f) = 0;
	};


	ENUM(FontDefault, int,
		SourceSansPro
	); // 

}