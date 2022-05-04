#version 330 core

layout(location = 0) in vec3 a_Coord;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform float u_Time;

out float v_Time;

out vec2 v_TexCoord;

void main()
{
	gl_Position = u_ViewProjection * /* u_Transform*/ vec4(a_Coord.x, a_Coord.y, a_Coord.z, 1.0f);
	v_Time = u_Time;
	v_TexCoord = a_TexCoord;
}