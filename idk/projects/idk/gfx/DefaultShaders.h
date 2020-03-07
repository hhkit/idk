#pragma once
namespace idk
{

	enum VertexShaders
	{
		VDebug,
		VNormalMesh,
		VNormalMeshPicker,
		VSkinnedMesh,
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
		FDeferredCombine,
		FDeferredHDR,
		FMax
	};
	enum GeometryShaders
	{
		GSinglePassCube,
		GMax
	};
}