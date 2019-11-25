/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: default_vertex.glsl
Purpose: Default vertex shader
Language: GLSL
Platform: OpenGL, Windows
Project: ivan.ho_CS300_1
Author: Ho Han Kit Ivan, 230001418, ivan.ho
Creation date: 5/28/2019
End Header --------------------------------------------------------*/
#version 450 
//#ifndef OGL
//#define U_LAYOUT(SET, BIND) layout(set = SET, binding = BIND) 
//#define BLOCK(X) X
//#endif

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in mat4 object_transform;
layout (location = 8) in mat4 normal_transform;

U_LAYOUT(0, 0) uniform BLOCK(CameraBlock)
{
	mat4 perspective_transform;
} PerCamera;

//U_LAYOUT(4, 0) uniform BLOCK(ObjectMat4Block)
//{
//	mat4 object_transform;
//	mat4 normal_transform;
//} ObjectMat4s;

layout(location = 1) out VS_OUT
{
  vec3 position;
  vec2 uv;
  vec3 normal;
  vec3 tangent;
  vec4 color;
} vs_out;

layout(location = 0) out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
	vs_out.position = vec3(object_transform * vec4(position, 1.0));
	vs_out.normal   = vec3(normal_transform * vec4(normal, 0.0));
	vs_out.tangent  = vec3(normal_transform * vec4(tangent, 0.0));
	vs_out.uv       = uv;
	vs_out.color    = vec4(1);
    gl_Position     = PerCamera.perspective_transform * vec4(vs_out.position, 1.0);
}