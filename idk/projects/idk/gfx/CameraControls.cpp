#include "stdafx.h"

//Graphics
#include "GraphicsSystem.h"

//Camera
#include "CameraControls.h"
#include "Camera.h"

//IDK
#include "IDK.h"
#include "Core/GameObject.h"

//Common
#include <common/Transform.h>

namespace idk {
	void CameraControls::FocusOnObj(Handle<GameObject> gameObjToFocus)
	{
		Handle<Camera> currentCam = Core::GetSystem<GraphicsSystem>().CurrentCamera();
		
		auto tfm = gameObjToFocus->GetComponent<Transform>();

		currentCam->SetTarget(tfm->GlobalPosition());

		currentCam->Focus();
	}
	void CameraControls::RotateCamera()
	{
		//arcball camera implemntation


	}
};
