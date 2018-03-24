#version 330 core

in vec2 TexCoord0;
out vec4 FragColor;

uniform sampler2D gSampler;

void main()
{
	vec4 color = texture(gSampler, TexCoord0.st);
	float brightness = (color.y * 0.3126) + (color.x * 0.4152) + (color.z * 0.2722);
	//can also set up an if brightness < 0.8, FragColor = black
	if(brightness > 0.3)
    {
    	//color.a = brightness;
		FragColor = color;
    }
	else
		FragColor = vec4(0.0, 0, 0, 1);
}
