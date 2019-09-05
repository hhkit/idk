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
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform mat4 perspective_transform;

uniform mat4 object_transform;
uniform mat4 normal_transform;

out VS_OUT
{
  vec3 position;
  vec2 uv;
  vec3 normal;
  vec3 tangent;
  vec3 view_pos; 		
} vs_out;

void main()
{
	vs_out.position = vec3(object_transform * vec4(position, 1.0));
	vs_out.normal   = vec3(normal_transform * vec4(normal, 1.0));
	vs_out.uv       = uv;
    gl_Position     = perspective_transform * vec4(vs_out.position, 1.0);
	vs_out.uv = gl_Position.xy;
}