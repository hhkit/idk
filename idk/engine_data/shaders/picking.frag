/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: picking.frag
Purpose: Default picking shader
Language: GLSL
Platform: OpenGL, Windows
Project: gam300
Author: Chong Wei Xiang, weixiang.c
Creation date: -
End Header --------------------------------------------------------*/
#version 450

layout(location=1)flat in uint id;

U_LAYOUT(5, 0) uniform BLOCK(NearFarBlock)
{
	float near,far;
} PerCamera;
layout(location=0)out uint out_color;

float LinearizeDepth(float depth,float near, float far) 
{
    return  (((near * far) / (far + depth * (near- far )))-near) /(far-near);	
}

void main()
{
	out_color = id;
	//Linearized so that picking of distant objects is more accurate. Tradeoff: Accuracy of super near objects.
	gl_FragDepth = LinearizeDepth(gl_FragCoord.z,PerCamera.near,PerCamera.far);
} 