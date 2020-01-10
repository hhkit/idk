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
#define MAX_BONES 100

layout (location = 0) in vec3 position;
layout (location = 1) in uint id;
layout (location = 2) in vec4 bone_weights;
layout (location = 3) in ivec4 bone_ids;
layout (location = 4) in mat4 object_transform;

U_LAYOUT(0, 0) uniform BLOCK(CameraBlock)
{
	mat4 perspective_transform;
} PerCamera;

U_LAYOUT(4, 1) uniform BLOCK(BoneMat4Block)
{
	mat4 bone_transform[MAX_BONES];
} BoneMat4s;

layout (location = 0) out gl_PerVertex
{
    vec4 gl_Position;
};

layout (location = 1) out uint out_id;

void main()
{
  mat4 b_transform = BoneMat4s.bone_transform[bone_ids[0]] * bone_weights[0];
  
  b_transform      += BoneMat4s.bone_transform[bone_ids[1]] * bone_weights[1];
  b_transform      += BoneMat4s.bone_transform[bone_ids[2]] * bone_weights[2];
  b_transform      += BoneMat4s.bone_transform[bone_ids[3]] * bone_weights[3];
	
	b_transform /= (bone_weights[0] + bone_weights[1] + bone_weights[2] + bone_weights[3]);
	mat4 resultant = object_transform *
                     b_transform;  
	vec3 out_position = vec3(resultant * vec4(position, 1.0));
	
	gl_Position     = PerCamera.perspective_transform * vec4(out_position, 1.0);
	out_id = id;
 
}