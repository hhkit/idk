#pragma once
#include <idk.h>
#include <core/ISystem.h>
#include <scene/Scene.h>

namespace idk
{
	class Scene;

	class ProjectManager
		: public ISystem
	{
	public:
		static constexpr string_view project_path = "/game/project.idk";

		

	private:
		void Init() override;
		void LateInit() override;
		void Shutdown() override;
		void SaveProject();


	};
}