#version 420 core

uniform vec3 u_Color;

out vec4 finalColor;

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
	finalColor = vec4(u_Color.x, u_Color.y, u_Color.z, 1.0f);
}