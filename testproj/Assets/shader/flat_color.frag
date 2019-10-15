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

layout(binding = 0, set = 1) uniform Color{vec3 color;}colorz;
layout(location = 2) in VS_OUT
{
  vec3 position;
  vec3 normal;
} fs_in;

layout(location = 0) out vec4 FragColor;

void main()
{
	FragColor = vec4(fs_in.position, 1.0);
} 