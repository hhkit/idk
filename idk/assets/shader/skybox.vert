/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: skybox.vert
Purpose: Default vertex shader
Language: GLSL
Platform: OpenGL, Windows
Project: gam300
Author: Chong Wei Xiang, weixiang.c
Creation date: -
End Header --------------------------------------------------------*/
#version 450 
//#ifndef OGL
//#define U_LAYOUT(SET, BIND) layout(set = SET, binding = BIND) 
//#define BLOCK(X) X
//#endif

layout (location = 0) in vec3 position;

U_LAYOUT(0, 0) uniform BLOCK(CameraBlock)
{
	mat4 pv_transform;
} PerCamera;

layout(location = 2) out VS_OUT
{
  vec3 uv;	
} vs_out;

layout(location = 0) out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
	vs_out.uv       = position;
    gl_Position     = PerCamera.pv_transform * vec4(position, 1.0);
}