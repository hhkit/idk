//#include "stdafx.h"
//
//#include <gfx/CascadedSet.h>
//
//namespace idk {
//	CascadedSet::CascadedSet(CameraData camData)
//	{
//		camData;
//		/*cascadeSet.resize(cascade_count);
//		float n_plane = camData.near_plane, f_plane = camData.far_plane;
//
//		float diff = f_plane - n_plane;
//		float first_end = n_plane + 0.25f * diff;
//		float second_end = n_plane + 0.60f * diff;
//
//		float cascadeiter[4] = {n_plane,first_end,second_end,f_plane};
//
//		unsigned i = 0;
//		for (auto& elem : cascadeSet)
//		{
//			elem.SetCascade(camData, cascadeiter[i++],cascadeiter[i]);
//			
//			if (elem.NeedLightMap())
//				elem.InitShadowMap();
//		}
//
//		needSetup = false;*/
//	}
//	void CascadedSet::Setup(CameraData camData)
//	{
//		camData;
//		/*if (needSetup)
//		{
//			float n_plane = camData.near_plane, f_plane = camData.far_plane;
//
//			float diff = f_plane - n_plane;
//			float first_end = n_plane + 0.25f * diff;
//			float second_end = n_plane + 0.60f * diff;
//
//			float cascadeiter[4] = { n_plane,first_end,second_end,f_plane };
//
//			unsigned i = 0;
//			for (auto& elem : cascadeSet)
//			{
//				elem.SetCascade(camData, cascadeiter[i++], cascadeiter[i]);
//
//				if (elem.NeedLightMap())
//					elem.InitShadowMap();
//
//				needSetup = false;
//			}
//		}*/
//	}
//}