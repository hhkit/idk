/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: default_vertex.glsl
Purpose: Default vertex shader
Language: GLSL
Platform: OpenGL, Windows
Project: ivan.ho_CS300_1
Author: Ho Han Kit Ivan, 230001418, ivan.ho
Creation date: 5/28/2019
End Header --------------------------------------------------------*/
#version 450 
//#ifndef OGL
//#define U_LAYOUT(SET, BIND) layout(set = SET, binding = BIND) 
//#define BLOCK(X) X
//#endif

layout (location = 0) in vec3 position;
layout (location = 1) in uint id;
layout (location = 4) in mat4 object_transform;
U_LAYOUT(0, 0) uniform BLOCK(CameraBlock)
{
	mat4 perspective_transform;
} PerCamera;
layout(location = 0) out gl_PerVertex
{
    vec4 gl_Position;
};
layout(location = 1) out uint out_id;

void main()
{
	vec3 pos = (object_transform * vec4(position, 1.0)).xyz;
    gl_Position     = PerCamera.perspective_transform * vec4(pos, 1.0);
	out_id = id;
}