#version 330

uniform mat4 depthMVP;

layout(location=0) in vec3 position;
layout(location=1) in vec2 inTexCoords;

//out vec2 uvCoords;

void main()
{
    gl_Position = depthMVP * vec4(position,1);
	//uvCoords = inTexCoords;
}
