#pragma once
namespace idk
{

	enum VertexShaders
	{
		VDebug,
		VNormalMesh,
		VNormalMeshShadow,
		VNormalMeshPicker,
		VSkinnedMesh,
		VSkinnedMeshShadow,
		VSkinnedMeshPicker,
		VParticle,
		VSkyBox,
		VPBRConvolute,
		VFsq,
		VFont,
		VUi,
		VMax
	};
	enum FragmentShaders
	{
		FDebug,
		FSkyBox,
		FShadow,
		FPicking,
		FPBRConvolute,
		FBrdf,
		FFont,
		FDeferredPost,
		FDeferredPostSpecular,
		FDeferredPostAmbient,
		FMax
	};
	enum GeometryShaders
	{
		GSinglePassCube,
		GShadowCNM,
		GShadowCSM,
		GMax
	};
}