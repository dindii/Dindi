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

in vec2 v_TexCoords;

uniform sampler2D u_GBuffer_Position;
uniform sampler2D u_GBuffer_Albedo;
uniform sampler2D u_GBuffer_Depth;
uniform sampler2D u_GBuffer_Normal;

uniform sampler2D u_SSAONoise;
uniform vec3 u_Samples[64];
uniform mat4 u_Projection;

out float FragColor;

//tile the noise tex accordingly
const vec2 noiseScale = vec2(2560.0f / 4.0f, 1440 / 4.0f); // screen = 2560x1440

void main()
{
	ivec2 coords = ivec2(v_TexCoords.x * 2560.0f, v_TexCoords.y * 1440.0f);
	vec4 worldFragPos =  vec4(texelFetch(u_GBuffer_Position, coords, 0).xyz, 1.0f);
	vec3 fragPos = (c_View * vec4(worldFragPos.xyz, 1.0f)).xyz;


	
//	float debugVal = fragPos.x * 0.02f;
//	float debugVal = length(fragPos) * 0.010f;
//	FragColor = debugVal;

vec3 normal =  mat3(c_View) * normalize(texture(u_GBuffer_Normal, v_TexCoords)).rgb;
vec3 randomVec = normalize(texture(u_SSAONoise, v_TexCoords * noiseScale).xyz);

//define orthogonal basis 
vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal)); 
vec3 bitangent = cross(normal, tangent);
mat3 TBN = mat3(tangent, bitangent, normal);

float occlusion = 0.0f;

for(int i = 0; i < 64; i++)
{
	vec3 samplePos = (TBN) * u_Samples[i]; //#TODO: Find out why this turns from view to clip space..
	samplePos = fragPos + samplePos * 1.2f;

	//we have to do some transformations on this so we can find the corresponding coords on the texture
	vec4 offset = vec4(samplePos, 1.0f);
	offset = (u_Projection) * offset;
	offset.xyz /= offset.w;
	offset.xyz = offset.xyz * 0.5f + 0.5f;

	float sampleDepth = (c_View * vec4(texture(u_GBuffer_Position, offset.xy).xyz, 1.0f)).z;

	float rangeCheck = smoothstep(0.0f, 1.0f, 1.2f / abs(fragPos.z - sampleDepth));

	occlusion += (sampleDepth >= samplePos.z + 0.025 ? 1.0f : 0.0f) * rangeCheck;
}

occlusion = (occlusion / 64.0f);
FragColor = occlusion;
}
