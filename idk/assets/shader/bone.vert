#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

U_LAYOUT(0, 0) uniform BLOCK(CameraBlock)
{
	mat4 perspective_transform;
} PerCamera;

U_LAYOUT(4, 0) uniform BLOCK(ObjectMat4Block)
{
	mat4 object_transform;
	mat4 normal_transform;
} ObjectMat4s;

out VS_OUT
{
  vec3 position;
  vec2 uv;
  vec3 normal;
  vec3 tangent;
  vec3 view_pos; 		
} vs_out;

void main()
{
	vs_out.position = vec3(ObjectMat4s.object_transform * vec4(position, 1.0));
	vs_out.normal   = vec3(ObjectMat4s.normal_transform * vec4(normal, 1.0));
	vs_out.uv       = uv;
    gl_Position     = PerCamera.perspective_transform * vec4(vs_out.position, 1.0);
	vs_out.uv = gl_Position.xy;
}