/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: skybox.frag
Purpose: Default vertex shader
Language: GLSL
Platform: OpenGL, Windows
Project: gam300
Author: Chong Wei Xiang, weixiang.c
Creation date: -
End Header --------------------------------------------------------*/
#version 450

//uniform struct Color {vec3 color;} ColorBlk;

layout(location = 2) in VS_OUT
{
  vec3 uv;	
} fs_in;

S_LAYOUT(3,1) uniform samplerCube sb;

layout(location = 0) out vec4 FragColor;

void main()
{
	vec3 color = texture(sb, fs_in.uv).rgb;
	//color = color / (color + vec3(1.0));
	//FragColor = vec4(pow(color, vec3(1.0/2.2)), 1); 
	FragColor = vec4(color, 1);
} 