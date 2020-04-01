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

layout(location = 0) out vec4 FragColor;

S_LAYOUT(3,0) uniform sampler2D tex;

const float smoothing = 1.0 /16.0;

void main()
{
	float dist = texture(tex,vs_out.uv).r;
	float alpha = smoothstep(0.3f, 0.8f, dist);
	
	//FragColor = vec4(1, 1, 1, texture(tex, vs_out.uv).r) * vs_out.color;
		
	FragColor = vec4(vs_out.color.rgb,alpha * vs_out.color.a);
	
	if(FragColor.a< 0.001176)
		discard;
		
	//FragColor = vec4(vs_out.color.rgb,0.f);
} 