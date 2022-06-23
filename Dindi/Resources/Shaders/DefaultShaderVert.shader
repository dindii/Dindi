#version 420 core

layout(location = 0) in vec3 a_Coord;
layout(location = 2) in vec2 a_TexCoord;

#define DND_MAX_LIGHTS 1000

struct PointLight
{
	vec4 m_Position;
	vec4 m_Color;
};

layout(std140, binding = 1) uniform ConstantData
{
	float c_Time;
	float _p1;
	float _p2;
	unsigned int numLights;
	mat4 c_ViewProjection;
	PointLight c_Lights[DND_MAX_LIGHTS];
};


out vec2 v_TexCoord;

void main()
{
	gl_Position = c_ViewProjection * /* u_Transform*/ vec4(a_Coord.x, a_Coord.y, a_Coord.z, 1.0f);
	v_TexCoord = a_TexCoord;
}