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
		static constexpr string_view project_path = "/assets/project.idk";

		void SaveProject();

	private:
		void Init() override;
		void LateInit() override;
		void Shutdown() override;


	};
}