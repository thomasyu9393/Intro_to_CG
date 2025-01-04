#version 330 core
out vec4 FragColor;

in vec2 TexCoord; 

uniform sampler2D ourTexture;
uniform vec3 rainbowColor;
uniform bool useRainbowColor;

uniform vec3 cubeColor;
uniform bool useHelicopter;

void main()
{
	// TODO: Implement Rainbow Effect

	//   1. Retrieve the color from the texture at texCoord.
	vec4 texColor = texture(ourTexture, TexCoord);

	//   2. Set FragColor to be the dot product of the color and rainbowColor
	//   Note: Ensure FragColor is appropriately set for both rainbow and normal cases.
	if (!useHelicopter) {
		FragColor = vec4(texColor.rgb * rainbowColor, texColor.a);
	} else {
		FragColor = vec4(cubeColor.rgb * rainbowColor, 1.0f);
	}
}