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

layout(location=1)in uint id;

layout(location=0)out uint out_color;

void main()
{
	out_color = id;
} 