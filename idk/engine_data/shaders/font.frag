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

layout(location = 2) in VS_OUT
{
  vec3 position;
  vec2 uv;
  vec4 color;
} vs_out;

layout(location = 0) out vec4 out_color;

S_LAYOUT(2, 5) uniform sampler2D _uTex[1];

void main()
{
	float alpha = texture(_uTex[0], vs_out.uv).r;
		
	out_color = vec4(vs_out.color.rgb, vs_out.color.a * alpha);
	
	if(out_color.a < 0.001176)
		discard;
} 