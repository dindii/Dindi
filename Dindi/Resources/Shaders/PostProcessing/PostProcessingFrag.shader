#version 420 core

in vec2 v_TexCoord;
uniform sampler2D u_HDRBuffer;
uniform sampler2D u_BloomTexture;
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

	vec4 hdrColor = texture(u_HDRBuffer, v_TexCoord);
	vec4 bloomColor = texture(u_BloomTexture, v_TexCoord);

	//with
	vec4 intermediateResult = hdrColor + (bloomColor * 0.8f);
	

	vec4 result = vec4(1.0f) - exp(-intermediateResult * exposure);
	result = pow(result, vec4(1.0f / gamma));
	FragColor = vec4(result);
}