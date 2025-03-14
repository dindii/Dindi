#version 330 core

// This shader performs upsampling on a texture,
// as taken from Call Of Duty method, presented at ACM Siggraph 2014.

// Remember to add bilinear minification filter for this texture!
// Remember to use a floating-point texture format (for HDR)!
// Remember to use edge clamping for this texture!
uniform sampler2D u_SrcCopy;


in vec2 v_TexCoord;
out vec4 outColor;

void main()
{
	outColor = texture(u_SrcCopy, v_TexCoord);
}