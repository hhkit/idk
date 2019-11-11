/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: flat_color.frag
Purpose: Flat color fragment shader
Language: GLSL
Platform: OpenGL, Windows
Project: ivan.ho_CS300_2
Author: Ho Han Kit Ivan, 230001418, ivan.ho
Creation date: 5/28/2019
End Header --------------------------------------------------------*/
#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

U_LAYOUT(0, 0) uniform BLOCK(CameraBlock)
{
	mat4 perspective_transform;
} PerCamera;

U_LAYOUT(4, 0) uniform BLOCK(ObjectMat4Block)
{
	mat4 object_transform;
	mat4 normal_transform;
} ObjectMat4s;

layout(location = 1) out VS_OUT
{
  vec3 position;
  vec3 normal;	
} vs_out;

layout(location = 0) out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
	vs_out.position = vec3(ObjectMat4s.object_transform * vec4(position, 1.0));
	vs_out.normal   = vec3(ObjectMat4s.normal_transform * vec4(normal, 1.0));
    gl_Position     = PerCamera.perspective_transform * vec4(vs_out.position, 1.0);
}