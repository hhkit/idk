/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: flat_color.frag
Purpose: Flat color fragment shader
Language: GLSL
Platform: OpenGL, Windows
Project: ivan.ho_CS300_2
Author: Ho Han Kit Ivan, 230001418, ivan.ho
Creation date: 5/28/2019
End Header --------------------------------------------------------*/
uniform vec3 color;

out vec4 FragColor;

void main()
{
	FragColor = vec4(1.0);
} 