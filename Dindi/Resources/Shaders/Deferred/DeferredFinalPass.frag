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


in vec2 v_QuadTexCoord;

uniform sampler2D u_GBuffer_Position;
uniform sampler2D u_GBuffer_Albedo;
uniform sampler2D u_GBuffer_Depth;
uniform sampler2D u_GBuffer_Normal;

uniform sampler2D u_ShadowMap[DND_CSM_LEVELS];
uniform sampler3D u_RandomAngles;
uniform mat4 u_SingleLightTransform[DND_CSM_LEVELS];
uniform float u_CSMDistances[DND_CSM_LEVELS];

out vec4 OutputColor;

vec3 ResolvePointLights(vec3 fragNormal, vec3 fragmentPos, vec3 fragmentAmbient, vec3 fragColor, float specularMultiplier)
{
		vec3 lightContribution = vec3(0.0f, 0.0f, 0.0f);

		for(int i = 0; i < numLights; i++)
		{
			vec3 lightPosition = c_Lights[i].m_Position.xyz;
			vec3 lightColor = c_Lights[i].m_Color.rgb;

			vec3 lightDir = normalize(lightPosition -  fragmentPos);
			float diff = max(dot(lightDir, fragNormal), 0.0);

			//Tweak light strength per light for bloom 
			vec3 diffuse = diff * fragColor * lightColor;

			// specular
			vec3 viewDir = normalize(c_CameraPos.xyz - fragmentPos);
			vec3 reflectDir = reflect(-lightDir, fragNormal);
			vec3 halfwayDir = normalize(lightDir + viewDir);
			float spec = pow(max(dot(fragNormal, halfwayDir), 0.0), 32.0f);

			//Tweak specular for bloom
			vec3 specular = lightColor * spec * specularMultiplier * 1.0f;

			//calculate attenuation - #TODO: Attenuation tweak values for uniforms
			float distance = length(lightPosition - fragmentPos);
			float attenuation = 2.0f / (distance * (distance / 2));
			
			diffuse *= attenuation * 0.05f;
			fragmentAmbient *= attenuation;
			specular *= attenuation * 0.5f;

			lightContribution += (diffuse + fragmentAmbient + specular);
		}
		
		return lightContribution;
}

vec3 ResolveDirectionalLight(vec3 fragNormal, vec3 fragmentPos, vec3 fragColor, float specularMultiplier)
{
		vec3 lightContribution = vec3(0.0f, 0.0f, 0.0f);

		vec3 lightDir = normalize(c_DirLightPos.xyz);
		float diff = max(dot(lightDir, fragNormal), 0.0);

		//#TODO - Create a Uniform for directional light color.
		vec3 dirLightColor = vec3(1.2f, 0.75f, 0.25f) * 2.4;
		vec3 dirLightAmbientColor = dirLightColor *  vec3(0.3f, 0.47f, 1.0f) * 0.2;

		//Tweak light strength per light for bloom 
		vec3 diffuse = diff * fragColor * dirLightColor;

		// specular
		vec3 viewDir = normalize(c_CameraPos.xyz - fragmentPos);
		vec3 reflectDir = reflect(-lightDir, fragNormal);
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(fragNormal, halfwayDir), 0.0), 32.0f);

		//Tweak specular for bloom
		vec3 specular = dirLightColor * spec * specularMultiplier * 1.0f;

		lightContribution += (diffuse + dirLightAmbientColor + specular);
		
		return lightContribution;
}

float CalculateShadowFilterSize(vec4 fragPosLightSpace, vec3 lightDir, float lightSize, int blockerSearchSamples, float shadowmapOffsetTextureSize, float shadowmapOffsetFilterSize)
{
	// Handling Perspective Issues
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(u_ShadowMap[0], projCoords.xy).r;

	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;

	/***--------STEP 1: Blocker search: find the average blocker depth---------***/

	float sampleStride = lightSize / 2.5;
	float dBlocker = 0.0;
	float sampleSize = 1.0 / textureSize(u_ShadowMap[0], 0).x * sampleStride;
	
	int blockerNumSample = blockerSearchSamples;

	int count = 0;

	ivec3 OffsetCoord;
	vec2 f = mod(gl_FragCoord.xy, vec2(shadowmapOffsetFilterSize));
	OffsetCoord.yz = ivec2(f);

	for (int i = 0; i < blockerNumSample; ++i)
	{
		OffsetCoord.x = i;
		vec2 sampleCoord = texelFetch(u_RandomAngles, OffsetCoord, 0).xy * sampleSize + projCoords.xy;
		float closestDepth = texture(u_ShadowMap[0], sampleCoord).r;
		//Only compute average depth of blocker! not the average of the whole filter's area!
		if (closestDepth < currentDepth) {
			dBlocker += closestDepth;
			count++;
		}
	}

	dBlocker /= count;

	float lightWidth = lightSize / 2.5;
	float wPenumbra = (currentDepth - dBlocker) * lightWidth / dBlocker;

	return wPenumbra;
}

float CalculateDirectionalShadow(vec4 fragPosLightSpace, int layer, float shadowFilterSize, float randomRadius, float offsetFilterSize, vec3 normal)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5f + 0.5f;
	vec3 ShadowCoords = projCoords;

//	float radius = clamp(FilterSizeShadowCalculation(fragPosLightSpace, layer), 1.0f, 3.0f);
	//float radius = clamp(FilterSizeShadowCalculation(fragPosLightSpace, layer), gShadowMapRandomRadius, gShadowMapRandomRadius * 4);
	float radius = clamp(shadowFilterSize, randomRadius, randomRadius * 4);
	radius /= (layer + 1);

	vec4 sc = vec4(ShadowCoords, 1.0f);

	vec3 LightDirection = c_DirLightPos.xyz;

	ivec3 OffsetCoord;
	vec2 f = mod(gl_FragCoord.xy, vec2(offsetFilterSize));
	OffsetCoord.yz = ivec2(f);
	float Sum = 0.0;
	int SamplesDiv2 = int(offsetFilterSize * offsetFilterSize / 2.0);


	float TexelWidth = 1.0 / textureSize(u_ShadowMap[layer], 0).x;
	//#BUG? SHOULD IT BE .X?
	float TexelHeight = 1.0 / textureSize(u_ShadowMap[layer], 0).x;

	vec2 TexelSize = vec2(TexelWidth, TexelHeight);

	float DiffuseFactor = dot(normal, -LightDirection);
	//float bias = mix(0.001, 0.0005f, DiffuseFactor);
	float bias = 0.0005;
	float Depth = 0.0;


	for (int i = 0; i < 4; i++)
	{
		OffsetCoord.x = i;
		vec4 Offsets = texelFetch(u_RandomAngles, OffsetCoord, 0) * radius;
		sc.xy = ShadowCoords.xy + Offsets.rg * TexelSize;
		Depth = texture(u_ShadowMap[layer], sc.xy).x;
		if (Depth + bias < ShadowCoords.z) {
			Sum += 0.0;
		}
		else {
			Sum += 1.0;
		}

		sc.xy = ShadowCoords.xy + Offsets.ba * TexelSize;
		Depth = texture(u_ShadowMap[layer], sc.xy).x;
		if (Depth + bias < ShadowCoords.z) {
			Sum += 0.0;
		}
		else {
			Sum += 1.0;
		}
	}

	float Shadow = Sum / 8.0;

	if (Shadow != 0.0 && Shadow != 1.0)
	{
		for (int i = 4; i < SamplesDiv2; i++) {
			OffsetCoord.x = i;
			vec4 Offsets = texelFetch(u_RandomAngles, OffsetCoord, 0) * radius;
			sc.xy = ShadowCoords.xy + Offsets.rg * TexelSize;
			Depth = texture(u_ShadowMap[layer], sc.xy).x;
			if (Depth + bias < ShadowCoords.z) {
				Sum += 0.0;
			}
			else {
				Sum += 1.0;
			}

			sc.xy = ShadowCoords.xy + Offsets.ba * TexelSize;
			Depth = texture(u_ShadowMap[layer], sc.xy).x;
			if (Depth + bias < ShadowCoords.z) {
				Sum += 0.0;
			}
			else {
				Sum += 1.0;
			}
		}

		Shadow = Sum / float(SamplesDiv2 * 2.0f);
	}

	return Shadow;
}

int ResolveShadowCascadeLayer(float thresholdMultiplier, float fragmentDistance)
{
	int Bayer4[4 * 4] = {
		0, 8, 2, 10,
		12, 4, 14, 6,
		3, 11, 1, 9,
		15, 7, 13, 5
	};

	int ModX = int(gl_FragCoord.x) % 4;
	int ModY = int(gl_FragCoord.y) % 4;

	float close = u_CSMDistances[0];
	float mid = u_CSMDistances[1];

	float BayerCoords = float(Bayer4[ModX * 4 + ModY]);
	BayerCoords *= (1.0f / (4.0f * 4.0f));
	BayerCoords -= 0.5f;
	float threshold = thresholdMultiplier * BayerCoords;

	int finalLayer = 2;

	if (abs(fragmentDistance) < close + threshold)
	{
		//outColor.rgb *= vec3(1.0f, 0.1f, 0.1f);
		finalLayer = 0;
	}
	else if (abs(fragmentDistance) < mid + threshold)
	{
		//outColor.rgb *= vec3(0.1f, 1.1f, 0.1f);
		finalLayer = 1;
	}

	return finalLayer;
}


void main()
{
		//fetch gbuffer
		vec3 normal = texture(u_GBuffer_Normal, v_QuadTexCoord).rgb;
		vec4 position = texture(u_GBuffer_Position, v_QuadTexCoord);
		vec4 colorSpecular= texture(u_GBuffer_Albedo, v_QuadTexCoord);

		vec3 fragPos = position.rgb;
		float viewSpaceFragmentDistance = position.a;

		vec3 color = colorSpecular.rgb;
		float specularTexVal = colorSpecular.a;
		vec3 ambient = color * 0.5f;
		
		//----------------------------------------------------------------------------------    Shadow calc
		vec4 FragPosLightSpace[DND_CSM_LEVELS];
		for(int i = 0; i < DND_CSM_LEVELS; i++)
		{
			FragPosLightSpace[i] = (u_SingleLightTransform[i] * vec4(fragPos.x, fragPos.y, fragPos.z, 1.0f));
		}

		float lightSize = 80.0f;
		int blockerSearchSamples = 25;
		float shadowMapOffsetTextureSize = 16;
		float shadowMapOffsetFilterSize = 8;
		float shadowMapRandomRadius = 2.0f;


		int shadowCascadeLayer = ResolveShadowCascadeLayer(3.0f, viewSpaceFragmentDistance);
		shadowCascadeLayer = shadowCascadeLayer;
		float filterSize = CalculateShadowFilterSize(FragPosLightSpace[shadowCascadeLayer], c_DirLightPos.xyz, lightSize, blockerSearchSamples, shadowMapOffsetTextureSize, shadowMapOffsetFilterSize);
		float shadowValue = CalculateDirectionalShadow(FragPosLightSpace[shadowCascadeLayer], shadowCascadeLayer, filterSize,  shadowMapRandomRadius, shadowMapOffsetFilterSize, normal);
		//----------------------------------------------------------------------------------    Shadow calc

		//----------------------------------------------------------------------------------    Light calc 
		vec3 totalLightContribution = vec3(0.0f, 0.0f, 0.0f);
		
		totalLightContribution += ResolvePointLights(normal, fragPos, ambient, color, specularTexVal);
		totalLightContribution += ResolveDirectionalLight(normal, fragPos, color, specularTexVal) * shadowValue;
		//----------------------------------------------------------------------------------    Light calc 

		OutputColor = vec4(totalLightContribution.rgb, 1.0f);
}