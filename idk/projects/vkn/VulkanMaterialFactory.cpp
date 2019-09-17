#include "pch.h"
#include <vkn/VulkanMaterial.h>
#include "VulkanMaterialFactory.h"
static const auto default_material_code = R"(
	color = vec4(1.0,0.0,0.2,1.0);
)";
namespace idk::vkn
{

	unique_ptr<Material> VulkanMaterialFactory::GenerateDefaultResource()
	{
		auto mat = std::make_unique<VulkanMaterial>();
		auto meta = mat->GetMeta();
		meta.compiled_shader_guid = Guid{ "10b6e1eb-694f-4509-b636-f22fca6f6b02" };
		mat->SetMeta(meta);
		mat->BuildShader({}, "", default_material_code);
		return mat;
	}
	unique_ptr<Material> VulkanMaterialFactory::Create()
	{
		auto mat = std::make_unique<VulkanMaterial>();
		return mat;
	}
	unique_ptr<Material> VulkanMaterialFactory::Create(PathHandle filepath)
	{
		UNREFERENCED_PARAMETER(filepath);
		return Create();
	}
}