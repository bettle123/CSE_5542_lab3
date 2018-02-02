#version 330
uniform sampler2D diffusetexture;
uniform sampler2D shadowMap;
in vec4 baseColor;
in vec2 uvCoords;
out vec4 fragColor;
in vec4 ShadowCoords;



void main( )
{
	/*
	float visibility =1.0f;
	
	if ( texture( shadowMap, ShadowCoords.xy ).z < ShadowCoords.z){
		visibility = 0.5f;
	}
	vec4 color = visibility * texture2D(diffusetexture, uvCoords );
	fragColor = color *baseColor;
	//fragColor = texture2D( shadowMap, uvCoords);
	*/
	
	// for the color texture to calculate the depth
	vec4 shadowColor = texture( shadowMap, ShadowCoords.xy/ShadowCoords.w);
	float visibility = 1.0f;
	float ggg = ShadowCoords.z/ShadowCoords.w;
	float fff = (shadowColor.r+ shadowColor.g+ shadowColor.b)/3.0f;
	float bias = 0.005;
	if ( fff < ggg- bias){
		visibility = 0.2;
	}
	
	vec4 color = texture(diffusetexture, uvCoords );
	//fragColor = visibility*color;
	//fragColor = vec4(fff,fff,fff,1.0);
	fragColor = visibility * texture(shadowMap, uvCoords);
	//fragColor = color*fragColor;
	
} 



