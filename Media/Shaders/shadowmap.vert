#version 330
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;
uniform mat4 depthBiasMVP;
layout(location=0) in vec3 position;
layout(location=3) in vec2 inTexCoords;
out vec2 uvCoords;
out vec3 ecPosition3;
out vec4 ShadowCoords;
// we calculate the normal at fragment shader
void main()
{
	vec4 ecPosition = modelViewMatrix * vec4(position, 1);
	ecPosition3 = ecPosition.xyz/ ecPosition.w;
	gl_Position = projectionMatrix * modelViewMatrix * vec4(position,1);
	ShadowCoords = projectionMatrix * modelViewMatrix * vec4(position, 1);
	uvCoords = inTexCoords;
}
