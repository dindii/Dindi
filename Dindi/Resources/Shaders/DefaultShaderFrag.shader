#version 420 core

#define DND_MAX_LIGHTS 5
#define DND_CSM_LEVELS 3

struct PointLight
{
	vec4 m_Position;
	vec4 m_Color;
};

in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal;

out vec4 outColor;
in vec4 v_FragPosLightSpace[DND_CSM_LEVELS];
in vec4 v_FragPosViewSpace;

//#TEMPORARY Until we have PBR (=
uniform sampler2D u_Diffuse;
uniform sampler2D u_Specular;
uniform sampler2D u_Normal; //#TODO: Expand to properly accept normal textures other than only attribute normals (usually we will only get those as textures).

uniform sampler2D u_ShadowMap[DND_CSM_LEVELS];
uniform sampler3D u_RandomAngles;
uniform float u_CSMDistances[DND_CSM_LEVELS];

in PointLight v_Lights[DND_MAX_LIGHTS];

uniform int u_LightIndex;

in vec3 v_TangentLightDirPos;
in vec3 v_TangentViewPos;
in vec3 v_TangentFragPos;
in mat3 v_TBN;


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


float gamma = 2.2;

//float gShadowMapRandomRadius = 2.5f;
float gShadowMapRandomRadius = 2.0f;

float gShadowMapOffsetTextureSize = 16;
float gShadowMapOffsetFilterSize = 8;


#define NUM_SAMPLES 25  //PCSS sample parameter in step 3
#define BLOCKER_SEARCH_NUM_SAMPLES 25//PCSS sample parameter in step 1


/*******-------------------- PCSS functions --------------------******/

float u_LightSize = 80.0f;


float FilterSizeShadowCalculation(vec4 argfragPosLightSpace, int layer)
{
	vec4 fragPosLightSpace = v_FragPosLightSpace[layer];
	vec3 lightDir = c_DirLightPos.xyz;

	// Handling Perspective Issues
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(u_ShadowMap[layer], projCoords.xy).r;

	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;

	/***--------STEP 1: Blocker search: find the average blocker depth---------***/

	float sampleStride = u_LightSize / 2.5;
	float dBlocker = 0.0;
	float sampleSize = 1.0 / textureSize(u_ShadowMap[layer], 0).x * sampleStride;
	
	int blockerNumSample = BLOCKER_SEARCH_NUM_SAMPLES;

	int count = 0;

	ivec3 OffsetCoord;
	vec2 f = mod(gl_FragCoord.xy, vec2(gShadowMapOffsetFilterSize));
	OffsetCoord.yz = ivec2(f);

	for (int i = 0; i < blockerNumSample; ++i)
	{
		OffsetCoord.x = i;
		vec2 sampleCoord = texelFetch(u_RandomAngles, OffsetCoord, 0).xy * sampleSize + projCoords.xy;
		float closestDepth = texture(u_ShadowMap[layer], sampleCoord).r;
		//Only compute average depth of blocker! not the average of the whole filter's area!
		if (closestDepth < currentDepth) {
			dBlocker += closestDepth;
			count++;
		}
	}

	dBlocker /= count;

	float lightWidth = u_LightSize / 2.5;
	float wPenumbra = (currentDepth - dBlocker) * lightWidth / dBlocker;

	return wPenumbra;
}

float ShadowCalculation(vec4 fragPosLightSpace, int layer)
{
	vec3 projCoords = v_FragPosLightSpace[layer].xyz / v_FragPosLightSpace[layer].w;
	projCoords = projCoords * 0.5f + 0.5f;
	vec3 ShadowCoords = projCoords;

//	float radius = clamp(FilterSizeShadowCalculation(fragPosLightSpace, layer), 1.0f, 3.0f);
	//float radius = clamp(FilterSizeShadowCalculation(fragPosLightSpace, layer), gShadowMapRandomRadius, gShadowMapRandomRadius * 4);
	float radius = clamp(FilterSizeShadowCalculation(fragPosLightSpace, layer), gShadowMapRandomRadius, gShadowMapRandomRadius * 4);
	radius /= (layer + 1);

	vec4 sc = vec4(ShadowCoords, 1.0f);

	vec3 LightDirection = c_DirLightPos.xyz;
	vec3 Normal = v_Normal;

	ivec3 OffsetCoord;
	vec2 f = mod(gl_FragCoord.xy, vec2(gShadowMapOffsetFilterSize));
	OffsetCoord.yz = ivec2(f);
	float Sum = 0.0;
	int SamplesDiv2 = int(gShadowMapOffsetFilterSize * gShadowMapOffsetFilterSize / 2.0);


	float TexelWidth = 1.0 / textureSize(u_ShadowMap[layer], 0).x;
	//#BUG? SHOULD IT BE .X?
	float TexelHeight = 1.0 / textureSize(u_ShadowMap[layer], 0).x;

	vec2 TexelSize = vec2(TexelWidth, TexelHeight);

	float DiffuseFactor = dot(Normal, -LightDirection);
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


struct PackLight
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

PackLight CalculatePointLight(vec3 argLightColor, vec3 argLightPos, int idx)
{
	if (textureSize(u_Normal, 0).x > 1)
	{
		//	VERSION WITH (working)
		vec3 normal = texture(u_Normal, v_TexCoord).rgb;
		// transform normal vector to range [-1,1]
		normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
	
		// get diffuse color
		vec3 color = texture(u_Diffuse, v_TexCoord).rgb * argLightColor;
		// ambient
		vec3 ambient = color;
		// diffuse
		vec3 lightDir = normalize(argLightPos - v_TangentFragPos);
		float diff = max(dot(lightDir, normal), 0.0);
		vec3 diffuse = diff * color;// *4;
		// specular
		vec3 viewDir = normalize(v_TangentViewPos - v_TangentFragPos);
		vec3 reflectDir = reflect(-lightDir, normal);
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0f);
		 
		vec3 specular;
	
		if (textureSize(u_Specular, 0).x > 1)
		{
			specular = vec3(1.0) * spec * texture2D(u_Specular, v_TexCoord).rgb;
		}
		else
		{
			specular = vec3(1.0) * spec * argLightColor;
		}

		return PackLight(ambient, diffuse, specular);
	}
	else
	{
		//VERSION WITHOUT (working)
		vec3 normal = normalize(v_Normal);
	
		// get diffuse color
		vec3 color = texture(u_Diffuse, v_TexCoord).rgb * argLightColor;
		// ambient
		//vec3 ambient = 0.5 * color;
		vec3 ambient = color;
		// diffuseZ
		//vec3 lightDir = normalize(transpose(v_TBN) * argLightPos - transpose(v_TBN) * v_TangentFragPos);
		vec3 lightDir = normalize(c_Lights[idx].m_Position.xyz -  v_FragPos.xyz);
		float diff = max(dot(lightDir, normal), 0.0);
		
		vec3 diffuse = diff * color;// *4;
		// specular
		vec3 viewDir = normalize(c_CameraPos.xyz - v_FragPos.xyz);
		vec3 reflectDir = reflect(-lightDir, normal);
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0f);

		vec3 specular = vec3(0.0f, 0.0f, 0.0f);

		if (textureSize(u_Specular, 0).x > 1)
		{
			specular = vec3(1.0) * spec * texture2D(u_Specular, v_TexCoord).rgb;
		}
		else
		{
			//#TODO adjust the value of specular for each material
			specular = spec * argLightColor;
			
		}

		return PackLight(ambient, diffuse, specular);
	}
}

PackLight CalculateDirectionalLight(vec3 argLightColor)
{
	if (textureSize(u_Normal, 0).x > 1)
	{
		//	VERSION WITH (working)
		vec3 normal = texture(u_Normal, v_TexCoord).rgb;
		// transform normal vector to range [-1,1]
		normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

		// get diffuse color
		vec3 color = texture(u_Diffuse, v_TexCoord).rgb * argLightColor;
		// ambient
		vec3 ambient = color;
		// diffuse
		vec3 lightDir = normalize(v_TBN * (c_DirLightPos.xyz));
		float diff = max(dot(lightDir, normal), 0.0);
		vec3 diffuse = diff * color;// *4;
		// specular
		vec3 viewDir = normalize(v_TangentViewPos - v_TangentFragPos);
		vec3 reflectDir = reflect(-lightDir, normal);
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0f);

		vec3 specular;

		if (textureSize(u_Specular, 0).x > 1)
		{
			specular = vec3(1.0) * spec * texture2D(u_Specular, v_TexCoord).rgb;
		}
		else
		{
			//#TODO adjust the value of specular for each material
			specular = vec3(1.0) * spec * argLightColor;
		}

		return PackLight(ambient, diffuse, specular);
	}
	else
	{
		//VERSION WITHOUT (working)
		vec3 normal = normalize(v_Normal);

		// get diffuse color
		vec3 color = texture(u_Diffuse, v_TexCoord).rgb * argLightColor;
		// ambient
		//vec3 ambient = 0.5 * color;
		vec3 ambient = color;
		// diffuseZ
		//vec3 lightDir = normalize(transpose(v_TBN) * argLightPos - transpose(v_TBN) * v_TangentFragPos);
		vec3 lightDir = normalize(c_DirLightPos.xyz);
		float diff = max(dot(lightDir, normal), 0.0);

		vec3 diffuse = diff * color;// *4;
		// specular
		vec3 viewDir = normalize(c_CameraPos.xyz - v_FragPos.xyz);
		vec3 reflectDir = reflect(-lightDir, normal);
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0f);

		vec3 specular = vec3(0.0f, 0.0f, 0.0f);

		if (textureSize(u_Specular, 0).x > 1)
		{
			specular = vec3(1.0) * spec * texture2D(u_Specular, v_TexCoord).rgb;
		}
		else
		{
			//#TODO adjust the value of specular for each material
			specular = spec * argLightColor;
		}

		 //ambient = normal;
		 //diffuse = normal;

		return PackLight(ambient, diffuse, specular);
	}
}

int Bayer4[4 * 4] = {
	0, 8, 2, 10,
	12, 4, 14, 6,
	3, 11, 1, 9,
	15, 7, 13, 5
};

void main()
{
//	remember to add support to non-normal textures by setting a uniform bool..

	//test changign stuff of lights (vlights) on vertex shader
	vec3 temporaryResult = vec3(0.0f, 0.0f, 0.0f);

	//I will not waste too much time setting uniforms for those tweaks or even setting properties in the light class because we are not going to
	//use blinn-phong for much time. So, that's why it is extremely hardcoded.
	for (int x = 0; x < numLights; x++)
	{
		PackLight temp = CalculatePointLight(v_Lights[x].m_Color.rgb, v_Lights[x].m_Position.xyz, x);

		vec3 fragPos = textureSize(u_Normal, 0).x > 1 ? v_TangentFragPos : v_FragPos;
		vec3 lightPos = textureSize(u_Normal, 0).x > 1 ? v_Lights[x].m_Position.xyz : c_Lights[x].m_Position.xyz;

		float distance = length(lightPos - fragPos);
		//float attenuation = 1.0f / (distance * distance);
		float attenuation = 2.0f / (distance * (distance / 2));

		//#TODO adjust the value of attenuation
		vec3 ambientWithAttenuation = temp.ambient * attenuation * 0.05f;
		vec3 diffuseWithAttenuation = temp.diffuse * attenuation * 1.0f;
		vec3 specularWithAttenuation = temp.specular * attenuation * 0.5f;

		temporaryResult += ambientWithAttenuation + diffuseWithAttenuation + specularWithAttenuation;
	}


//	//#TODO: colocar a cor do dir light no cbuffer
	PackLight dirLight = CalculateDirectionalLight(vec3(1.2f, 0.75f, 0.25f) * 2.4);

	dirLight.diffuse *= 1.0f;
	dirLight.ambient *= vec3(0.3f, 0.47f, 1.0f) * 0.2;

//	temporaryResult += dirLight.ambient + dirLight.diffuse + dirLight.specular;
//	
//
	int layer = 2;
	
	float closer = u_CSMDistances[0];
	float mid = u_CSMDistances[1];
	//
	//
	int ModX = int(gl_FragCoord.x) % 4;
	int ModY = int(gl_FragCoord.y) % 4;
	//
	//
	float BayerCoords = float(Bayer4[ModX * 4 + ModY]);
	BayerCoords *= (1.0f / (4.0f * 4.0f));
	BayerCoords -= 0.5f;
	float threshold = 3.0f * BayerCoords;


	////}
	//
	if (abs(v_FragPosViewSpace.z) < closer + threshold)
	{
		//outColor.rgb *= vec3(1.0f, 0.1f, 0.1f);
		layer = 0;
	}
	else if (abs(v_FragPosViewSpace.z) < mid + threshold)
	{
		//outColor.rgb *= vec3(0.1f, 1.1f, 0.1f);
		layer = 1;
	}
	//
	float shadow = ShadowCalculation(v_FragPosLightSpace[layer], layer);

	//get backas
	temporaryResult += vec3(dirLight.ambient + shadow * (dirLight.diffuse + dirLight.specular));
	

	outColor = vec4(temporaryResult.xyz, 1.0f);
//s

	//DEBUG TEST
	//float exposure = 1.0f;
	//outColor.rgb = vec3(1.0f) - exp(-outColor.rgb * exposure);
	////vec3 result = hdrColor / (hdrColor + vec3(1.0f));
	//
	//
	//outColor.rgb = pow(outColor.rgb, vec3(1.0f / gamma));

	//DEBUG TEST


	//outColor.rgb = pow(outColor.rgb, vec3(1.0f / gamma));
//	
//	//
//	if (abs(v_FragPosViewSpace.z) < closer + threshold)
//	{
//	//		outColor.rgb *= vec3(1.0f, 0.1f, 0.1f);
//		layer = 0;
//	}
//	else if (abs(v_FragPosViewSpace.z) < mid + threshold)
//	{
//	//		outColor.rgb *= vec3(0.1f, 1.1f, 0.1f);
//		layer = 1;
//	}
}