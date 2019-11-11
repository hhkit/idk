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

layout(location = 2) in VS_OUT
{
  vec2 uv;	
} vs_out;

uniform sampler2D flashmebaby;

out vec4 color;

void main()
{
   color = texture(flashmebaby, uv).rgb;
}