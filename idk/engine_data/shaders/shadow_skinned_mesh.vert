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
#define SHADOW_CASCADES 2

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in ivec4 bone_ids;
layout (location = 5) in vec4 bone_weights;

U_LAYOUT(4, 0) uniform BLOCK(ObjectMat4Block)
{
	mat4 object_transform;
	mat4 normal_transform;
} ObjectMat4s;

//U_LAYOUT(10,0) uniform BLOCK(CameraBlock)
//{
//	mat4 perspective_transform[SHADOW_CASCADES];
//} PerCamera;

U_LAYOUT(4, 1) uniform BLOCK(BoneMat4Block)
{
	mat4 bone_transform[MAX_BONES];
} BoneMat4s;

layout (location = 0) out gl_PerVertex
{
    vec4 gl_Position;
};

layout (location = 1) out VS_OUT
{
  vec3 position;
  vec2 uv;
  vec3 normal;
  vec3 tangent;
  vec4 color;
  float clip_pos;
} vs_out;


void main()
{
  mat4 b_transform =  BoneMat4s.bone_transform[bone_ids[0]] * bone_weights[0];
  
  b_transform      += BoneMat4s.bone_transform[bone_ids[1]] * bone_weights[1];
  b_transform      += BoneMat4s.bone_transform[bone_ids[2]] * bone_weights[2];
  b_transform      += BoneMat4s.bone_transform[bone_ids[3]] * bone_weights[3];
	
	b_transform /= (bone_weights[0] + bone_weights[1] + bone_weights[2] + bone_weights[3]);
	mat4 resultant = ObjectMat4s.object_transform *
                     b_transform;  
	vs_out.position = vec3(resultant * vec4(position, 1.0));
	
	mat4 nml_transform = transpose(inverse(resultant));
	vs_out.normal   = vec3( nml_transform * vec4(normal, 0.0));
	vs_out.tangent  = vec3( nml_transform * vec4(tangent, 0.0));
	vs_out.uv       = uv;
	vs_out.color    = vec4(1);
	vs_out.clip_pos = gl_Position.z;
	gl_Position     = vec4(vs_out.position, 1.0);
 
	// vs_out.uv = gl_Position.xy;
}