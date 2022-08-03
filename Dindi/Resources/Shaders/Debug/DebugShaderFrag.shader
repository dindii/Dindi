#version 420 core

uniform vec3 u_Color;

out vec4 finalColor;

void main()
{
	finalColor = vec4(u_Color.x, u_Color.y, u_Color.z, 1.0f);
}