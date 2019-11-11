#include "SomeHackyThing.h"


#include <filesystem>

// project management and setup
#include <proj/ProjectManager.h>
#include <common/TagManager.h>
#include <PauseConfigurations.h>
#include <vkn/VulkanWin32GraphicsSystem.h>
#include <opengl/system/OpenGLGraphicsSystem.h>

#include <scene/SceneManager.h>

// resource importing
#include <res/EasyFactory.h>
#include <loading/AssimpImporter.h>
#include <loading/GraphFactory.h>
#include <opengl/resource/OpenGLCubeMapLoader.h>
#include <opengl/resource/OpenGLTexture.h>
#include <opengl/resource/OpenGLTextureLoader.h>
#include <opengl/resource/OpenGLFontAtlasLoader.h>
#include <opengl/resource/OpenGLMesh.h>
// editor setup
#include <gfx/RenderTarget.h>

// gfx
#include <glad/glad.h>
#include <opengl/program/Program.h>
#include <opengl/resource/FrameBuffer.h>
// util
#include <util/ioutils.h>

namespace idk
{
	namespace fs = std::filesystem;

	void SomeHackyThing::Init()
	{
		// project load
		{
			auto& proj_manager = Core::GetSystem<ProjectManager>();
			const auto recent_proj = []() -> string
			{
				fs::path recent_path = Core::GetSystem<FileSystem>().GetAppDataDir();
				recent_path /= "idk";
				recent_path /= ".recent";
				if (!fs::exists(recent_path))
					return "";
				std::ifstream recent_file{ recent_path };
				fs::path proj = stringify(recent_file);
				if (!fs::exists(proj))
					return "";
				return proj.string();
			}();

			if (recent_proj.empty())
			{
				const DialogOptions dialog{ "IDK Project", ProjectManager::ext };
				auto proj = Core::GetSystem<Application>().OpenFileDialog(dialog);
				while (!proj)
					proj = Core::GetSystem<Application>().OpenFileDialog(dialog);
				proj_manager.LoadProject(*proj);
			}
			else
				proj_manager.LoadProject(recent_proj);
		}


		Core::GetResourceManager().RegisterLoader<OpenGLCubeMapLoader>(".cbm");
		Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".png");
		Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".tga");
		Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".jpg");
		Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".jpeg");
		Core::GetResourceManager().RegisterLoader<OpenGLTextureLoader>(".dds");
		Core::GetResourceManager().RegisterLoader<OpenGLFontAtlasLoader>(".ttf");

		Core::GetResourceManager().RegisterLoader<AssimpImporter>(".fbx");
		Core::GetResourceManager().RegisterLoader<AssimpImporter>(".obj");
		Core::GetResourceManager().RegisterLoader<AssimpImporter>(".md5mesh");
		Core::GetResourceManager().RegisterLoader<GraphLoader>(shadergraph::Graph::ext);
		Core::GetResourceManager().RegisterFactory<GraphFactory>();

		auto& fs = Core::GetSystem<FileSystem>();
		fs.Mount(string{ fs.GetExeDir() } +"/editor_data", "/editor_data", false);
		if (shadergraph::NodeTemplate::GetTable().empty())
			shadergraph::NodeTemplate::LoadTable("/editor_data/nodes");

	}

	void SomeHackyThing::LateInit()
	{
		auto vert_stream = Core::GetSystem<FileSystem>().Open("/engine_data/shaders/fsq.vert", FS_PERMISSIONS::READ);
		auto frag_stream = Core::GetSystem<FileSystem>().Open("/engine_data/shaders/fsq.frag", FS_PERMISSIONS::READ);
		auto vert_shader = ogl::Shader{ GL_VERTEX_SHADER, stringify(vert_stream) };
		auto frag_shader = ogl::Shader{ GL_FRAGMENT_SHADER, stringify(frag_stream) };

		program_id = glCreateProgram();
		glAttachShader(program_id, vert_shader.ID());
		glAttachShader(program_id, frag_shader.ID());
		glLinkProgram(program_id);


		for (auto& elem : Core::GetSystem<FileSystem>().GetEntries("/assets", FS_FILTERS::FILE | FS_FILTERS::RECURSE_DIRS))
		{
			if (elem.GetExtension() != ".meta")
				Core::GetResourceManager().Load(elem, false);
		}
	}

	void SomeHackyThing::Shutdown()
	{
	}

	void SomeHackyThing::EditorUpdate()
	{
	}

	void SomeHackyThing::EditorDraw()
	{
		glUseProgram(program_id);
		
		glActiveTexture(GL_TEXTURE0);
		RscHandle<ogl::OpenGLRenderTarget>{}->GetColorBuffer().as<ogl::OpenGLTexture>().BindToUnit(0);
		glProgramUniform1i(program_id, glGetUniformLocation(program_id, "flashmebaby"), 0);

		auto mesh = RscHandle<ogl::OpenGLMesh>{ Mesh::defaults[MeshType::FSQ] };
		mesh->Bind(renderer_attributes
			{ {
				{vtx::Attrib::Position, 0},
				{vtx::Attrib::UV, 1},
			}});
		mesh->Draw();
	}
}