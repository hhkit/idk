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

layout(location = 1) in VS_OUT
{
  vec3 position;
} ;
layout(location=0)in vec4 gl_FragCoord;

layout(location = 0) out vec4 FragColor;

void main()
{
	FragColor = vec4(abs(gl_FragCoord.zzz/gl_FragCoord.w), 1.0);
} 