#version 450 

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

U_LAYOUT(4, 0) uniform BLOCK(ObjectMat4Block)
{
	mat4 object_transform;
} ObjectMat4s;

layout(location = 1) out VS_OUT
{
  vec3 position;
  vec2 uv;
} vs_out;

layout(location = 0) out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
	vs_out.position = vec3(ObjectMat4s.object_transform * vec4(position, 0, 1.0));
	vs_out.uv       = uv;
    gl_Position     = vec4(vs_out.position.xy, 0, 1.0);
}