#version 420 core

#define DND_MAX_LIGHTS 5
#define DND_CSM_LEVELS 3

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

out vec2 v_QuadTexCoord;

void main()
{
	v_QuadTexCoord = a_QuadTexCoord;
	gl_Position = vec4(a_QuadCoord, 1.0);
}