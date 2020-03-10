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
#define SHADOW_TRANSFORMS 6

layout(location = 5) in VS_OUT
{
  vec3 fragPos;
} fp_in;

U_LAYOUT(11,0) uniform BLOCK(CameraBlock)
{
	float far_plane;
	vec3 light_pos;
	mat4 perspective_transform[SHADOW_TRANSFORMS];
} PerCamera;


void main()
{
	float dist = length(fp_in.fragPos.xyz - PerCamera.light_pos);
	
	dist = dist / PerCamera.far_plane;
	
	gl_FragDepth = dist;
} 