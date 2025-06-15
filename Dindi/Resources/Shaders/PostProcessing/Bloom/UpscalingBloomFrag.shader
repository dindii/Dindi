#version 330 core

// This shader performs upsampling on a texture,
// as taken from Call Of Duty method, presented at ACM Siggraph 2014.

// Remember to add bilinear minification filter for this texture!
// Remember to use a floating-point texture format (for HDR)!
// Remember to use edge clamping for this texture!
uniform sampler2D u_SrcTexture;
uniform sampler2D u_DownsampleSrcTexture;
uniform float u_FilterRadius;
in vec2 v_TexCoord;
vec4 upsample;
out vec4 outColor;


void main()
{
	// The filter kernel is applied with a radius, specified in texture
	// coordinates, so that the radius will vary across mip resolutions.
	float x = u_FilterRadius;
	float y = u_FilterRadius;

	// Take 9 samples around current texel:
	// a - b - c
	// d - e - f
	// g - h - i
	// === ('e' is the current texel) ===
	vec4 a = texture(u_SrcTexture, vec2(v_TexCoord.x - x, v_TexCoord.y + y));
	vec4 b = texture(u_SrcTexture, vec2(v_TexCoord.x, v_TexCoord.y + y));
	vec4 c = texture(u_SrcTexture, vec2(v_TexCoord.x + x, v_TexCoord.y + y));

	vec4 d = texture(u_SrcTexture, vec2(v_TexCoord.x - x, v_TexCoord.y));
	vec4 e = texture(u_SrcTexture, vec2(v_TexCoord.x, v_TexCoord.y));
	vec4 f = texture(u_SrcTexture, vec2(v_TexCoord.x + x, v_TexCoord.y));

	vec4 g = texture(u_SrcTexture, vec2(v_TexCoord.x - x, v_TexCoord.y - y));
	vec4 h = texture(u_SrcTexture, vec2(v_TexCoord.x, v_TexCoord.y - y));
	vec4 i = texture(u_SrcTexture, vec2(v_TexCoord.x + x, v_TexCoord.y - y));

	// Apply weighted distribution, by using a 3x3 tent filter:
	//  1    | 1 2 1 |
	// -- * | 2 4 2 |
	// 16   | 1 2 1 |
	upsample = e * 4.0;
	upsample += (b + d + f + h)*2.0;
	upsample += (a + c + g + i);
	upsample *= 1.0 / 16.0;

	upsample += texture(u_DownsampleSrcTexture, vec2(v_TexCoord.x, v_TexCoord.y));

	outColor = vec4(upsample.xyz, 1.0f);
}