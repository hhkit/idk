#version 450

layout(location = 2) in VS_OUT
{
  vec4 color;	
} fs_in;

layout(location = 0) out vec4 FragColor;

void main()
{
	FragColor = vec4(1,1,1,1);
} 