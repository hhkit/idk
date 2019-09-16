#version 450

layout(binding = 0, set = 1) uniform Color{vec3 color;}colorz;
layout(location = 2) in VS_OUT
{
  vec3 position;
  vec3 cubeMapuv;
  vec3 normal;
} fs_in;

uniform samplerCube skyBox;

layout(location = 0) out vec4 FragColor;

void main()
{
	FragColor = texture(skyBox, cubeMapuv);
} 