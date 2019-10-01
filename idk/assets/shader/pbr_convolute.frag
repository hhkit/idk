/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: skybox.frag
Purpose: Default vertex shader
Language: GLSL
Platform: OpenGL, Windows
Project: gam300
Author: Chong Wei Xiang, weixiang.c
Creation date: -
End Header --------------------------------------------------------*/
#version 450

S_LAYOUT(6,0) uniform samplerCube environment_probe;

layout (location = 3) in VS_OUT
{
  vec3 position;
} vs_out;

S_LAYOUT(6,1) uniform samplerCube sb;

out vec4 out_color;

const float PI = 3.1415926535;
const float HALF_PI = PI / 2;
const float TAU = PI * 2;

void main()
{
	vec3 normal = normalize(vs_out.position);
	vec3 irradiance = vec3(0);
	
	vec3 up = vec3(0, 1, 0);
	vec3 right = cross(up, normal);
	up = cross(normal, right);
	
	const float sampleDelta = 0.025;
	float nrSamples =0.0;
	
	for (float phi = 0.0; phi < TAU; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < HALF_PI; theta += sampleDelta)
        {
			float sinthet = sin(theta);
			float costhet = cos(theta);
			float sinphi  = sin(phi);
			float cosphi  = cos(phi);
			
            vec3 tangentSample = vec3(sinthet * cosphi,  sinthet * sinphi, costhet);
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal; 

            irradiance += texture(environment_probe, sampleVec).rgb * costhet * sinthet;
            nrSamples++;
        }
    }
	
	out_color = vec4(irradiance * PI / nrSamples, 1);
	//if (gl_Layer == 0) out_color = vec4(  1,0.5,0.5,1);
	//if (gl_Layer == 1) out_color = vec4(  0,0.5,0.5,1);
	//if (gl_Layer == 2) out_color = vec4(0.5,  1,0.5,1);
	//if (gl_Layer == 3) out_color = vec4(0.5,  0,0.5,1);
	//if (gl_Layer == 4) out_color = vec4(0.5,0.5,  1,1);
	//if (gl_Layer == 5) out_color = vec4(0.5,0.5,  0,1);
	//out_color = vec4(normal / 2 + vec3(0.5), 1);
} 