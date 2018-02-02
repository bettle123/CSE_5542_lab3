
#version 330
// fragment shader for the depth do nothing.

out vec4 FragColor;


void main( )
{
	//float distance = 0.5*gl_FragCoord.z +0.5;
	float distance =gl_FragCoord.z;
	FragColor = vec4(distance, distance, distance, 1);
} 






