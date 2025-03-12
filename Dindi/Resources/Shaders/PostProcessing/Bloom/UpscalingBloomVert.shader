#version 420 core

layout(location = 0) in vec3 a_Coord;
layout(location = 2) in vec2 a_TexCoord;

out vec2 v_TexCoord;

#define DND_MAX_LIGHTS 5

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
	mat4 c_View;
	vec4 c_CameraPos;
	vec4 c_DirLightPos;
	PointLight c_Lights[DND_MAX_LIGHTS];
};

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = vec4(a_Coord, 1.0);
}