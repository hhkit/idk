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

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;



layout(set = 0, binding = 0)uniform MVP
{
	mat4 transform;
} object_transform;
layout(set = 0,binding = 1)uniform MVP_IVT
{
	mat4 transform;
} normal_transform;

layout(location = 2) out VS_OUT
{
  vec3 position;
  vec2 uv;
  vec3 normal;
  vec3 tangent;
} vs_out;

void main()
{
	vs_out.position = vec3(object_transform.transform * vec4(position, 1.0));
	vs_out.normal   = vec3(normal_transform.transform * vec4(normal, 1.0));
	vs_out.uv       = uv;
    gl_Position     = vec4(vs_out.position, 1.0);
}