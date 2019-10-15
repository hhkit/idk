#version 450

layout (location = 0) in vec3 position;

layout (location = 1) out VS_OUT
{
  vec3 position;
} vs_out;

layout (location = 0)out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    vs_out.position = position;

    gl_Position = vec4(position, 1);
}