#version 450 

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

U_LAYOUT(4, 0) uniform BLOCK(ObjectMat4Block)
{
	mat4 object_transform;
} ObjectMat4s;

U_LAYOUT(8, 0) uniform BLOCK(UIBlock)
{
	vec4 color;
} PerUI;

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
	vs_out.position = vec3(ObjectMat4s.object_transform * vec4(position.xy, 0, 1.0));
	vs_out.uv       = uv;
	vs_out.color    = PerFont.color;
    gl_Position     = PerCamera.perspective_transform * vec4(vs_out.position, 1.0);
}