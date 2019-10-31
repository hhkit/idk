/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: font.glsl
Purpose: Default font vertex shader
Language: GLSL
Platform: OpenGL, Windows
Project: weixiang.c gam 300
Author: Chong Wei Xiang
Creation date: 5/28/2019
End Header --------------------------------------------------------*/
#version 450 

layout (location = 0) in vec4 position;

U_LAYOUT(0, 0) uniform BLOCK(CameraBlock)
{
	mat4 perspective_transform;
} PerCamera;

U_LAYOUT(4, 0) uniform BLOCK(ObjectMat4Block)
{
	mat4 object_transform;
} ObjectMat4s;

U_LAYOUT(8, 0) uniform BLOCK(FontBlock)
{
	vec4 color;
} PerFont;

layout(location = 1) out VS_OUT
{
  vec3 position;
  vec2 uv;
  vec4 color;
} vs_out;

layout(location = 0) out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
	vs_out.position = vec3(ObjectMat4s.object_transform * vec4(position.xyz, 1.0));
	vs_out.uv       = position.zw;
	vs_out.color    = PerFont.color;
    gl_Position     = PerCamera.perspective_transform * vec4(vs_out.position, 1.0);
}