#version 330 core

in vec2 TexCoord0;
in vec3 Color0;
in float Alpha0;
out vec4 FragColor;

uniform sampler2D gSampler;

void main()
{
	vec4 color = texture2D(gSampler, TexCoord0.st);

	// Modify color here
	color.b /= 2.0;
	color.rg /= 8.0;

	FragColor = color;
}
