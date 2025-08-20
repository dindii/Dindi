#version 420 core

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
	mat4 c_View;
	vec4 c_CameraPos;
	vec4 c_DirLightPos;
	PointLight c_Lights[DND_MAX_LIGHTS];
};

layout(location = 0) in vec3 a_QuadCoord;
layout(location = 2) in vec2 a_QuadTexCoord;


out vec2 v_TexCoords;

void main()
{
	gl_Position = vec4(a_QuadCoord, 1.0);
	v_TexCoords = a_QuadTexCoord;
}