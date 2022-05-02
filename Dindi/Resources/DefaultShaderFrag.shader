#version 330 core

in float v_Time;
out vec4 outColor;

void main()
{
	outColor = vec4(sin(v_Time), 0.0f, cos(v_Time), 1.0f);
}