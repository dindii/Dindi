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

vec3 aces_tonemap(vec3 color){	
	mat3 m1 = mat3(
        0.59719, 0.07600, 0.02840,
        0.35458, 0.90834, 0.13383,
        0.04823, 0.01566, 0.83777
	);
	mat3 m2 = mat3(
        1.60475, -0.10208, -0.00327,
        -0.53108,  1.10813, -0.07276,
        -0.07367, -0.00605,  1.07602
	);
	vec3 v = m1 * color;    
	vec3 a = v * (v + 0.0245786) - 0.000090537;
	vec3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
        
	return m2 * (a / b); 
}

void main()
{
	float gamma = 2.2;
	float exposure = 1.0f;

	vec4 hdrColor = texture(u_HDRBuffer, v_TexCoord);
	vec4 bloomColor = texture(u_BloomTexture, v_TexCoord);

	//with
	//vec4 intermediateResult = hdrColor + (bloomColor * 0.0f);
	vec4 intermediateResult = hdrColor + (bloomColor * 1.0f);
	
	vec4 result = vec4(aces_tonemap(intermediateResult.xyz * exposure), 1.0f);

	result = pow(result, vec4(1.0f / gamma));
	FragColor = vec4(result);
}