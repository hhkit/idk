#pragma once
#include <res/Resource.h>

namespace idk
{
	enum class ShaderBuildResult
	{
		Ok,
		Err_InvalidGLSL,
		Err_NonexistentFactory
	};

	enum class ShaderStage
	{
		Vertex,
		TessControl,
		TessEval,
		Geometry,
		Fragment,
		Compute,
	};

	class ShaderProgram
		: public Resource<ShaderProgram>
	{
	public:
		// graph will call this function
		ShaderBuildResult BuildShader(ShaderStage stage, string_view glsl_code);
		virtual ~ShaderProgram() = default;
	};
}