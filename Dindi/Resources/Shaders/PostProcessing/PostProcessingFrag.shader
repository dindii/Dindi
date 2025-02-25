#version 420 core

in vec2 v_TexCoord;
uniform sampler2D u_HDRBuffer;
out vec4 FragColor;

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
	float gamma = 2.2;
	float exposure = 1.0f;

	vec3 hdrColor = texture(u_HDRBuffer, v_TexCoord).rgb;
	

	vec3 result = vec3(1.0f) - exp(-hdrColor * exposure);

	result = pow(result, vec3(1.0f / gamma));
	FragColor = vec4(result, 1.0f);
}