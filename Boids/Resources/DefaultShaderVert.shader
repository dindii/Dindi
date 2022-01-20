#version 330 core

layout(location = 0) in vec3 a_Coord;

uniform mat4 u_ViewProjection;

void main()
{
	gl_Position = u_ViewProjection * /* u_Transform*/ vec4(a_Coord.x, a_Coord.y, a_Coord.z, 1.0f);
}