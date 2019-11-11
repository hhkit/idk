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

layout (location = 3) in VS_OUT
{
  vec3 position;
} vs_out;

S_LAYOUT(6,1) uniform uint obj_index;

layout(location=0)out vec4 out_color;

void main()
{
	out_color = vec4(float(obj_index),0,0,float(gl_PrimitiveID + 1));
} 