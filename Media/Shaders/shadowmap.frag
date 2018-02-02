
#version 330 core
struct BasicMaterial
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

const int maxLights = 2;
uniform vec4 lightPositions[maxLights];
uniform vec4 lightColors[maxLights];
uniform vec3 lightAmbient = vec3(0.1, 0.5, 0.0);
uniform BasicMaterial frontMaterial;

uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat3 normalMatrix;

uniform sampler2D normaltexture;
uniform sampler2D shadowMap;

in vec2 uvCoords;
in vec3 ecPosition3;
in vec4 ShadowCoords;
out vec4 fragColor;


void PointLight(in int i, in vec3 eye, in vec3 ecPosition3, in vec3 normal,
	inout vec4 diffuse, inout vec4 specular)
{
	// Compute vector from surface to light position
	vec4 lightPosInCamera = modelViewMatrix * lightPositions[i];
	vec3 lightDir = normalize(vec3(lightPosInCamera) - ecPosition3);
	// Compute distance between surface and light position
	float lightDistance = length(lightDir);
	lightDir = lightDir / lightDistance;
	// Compute attenuation
	float attenuation = 1.0;
	vec3 viewDir = normalize(-ecPosition3);
	float nDotL = dot(normal, lightDir);
	vec3 reflectionDir = normalize(2*nDotL*normal - lightDir);
	nDotL = max(0.0, nDotL);
	float vDotR = max(0.0, dot(viewDir, reflectionDir));
	float specularAmount = 0;
	if (nDotL > 0.0)
		specularAmount = pow(vDotR, frontMaterial.shininess);
	diffuse += lightColors[i] * nDotL * attenuation;
	specular += lightColors[i] * specularAmount * attenuation;
}


void DirectionalLight(in int i, in vec3 normal, in vec3 ecPosition3,
	inout vec4 diffuse, inout vec4 specular)
{
	vec3 lightDir = normalize(normalMatrix * normalize(lightPositions[i].xyz));
	vec3 viewDir = normalize(-ecPosition3);
	float nDotL = dot(normal, lightDir);
	//vec3 reflectionDir = normalize(2*nDotL*normal - lightDir);
	vec3 reflectionDir = normalize(viewDir + lightDir);
	nDotL = max(0.0, nDotL);
	//float vDotR = max(0.0, dot(viewDir, reflectionDir));
	float nDotH = max(0.0, dot(normal, reflectionDir));
	float specularAmount = 0;
	if (nDotL > 0.0)
		specularAmount = pow(nDotH, frontMaterial.shininess);
	diffuse += lightColors[i] * nDotL;
	specular += lightColors[i] * specularAmount;
	specular += lightColors[i] * specularAmount;
}

void main( )
{

	
	vec4 ambient = vec4(lightAmbient, 1.0);
	vec4 diffuse = vec4(0.2, 0.2, 0.2, 0.0);
	vec4 specular = vec4(0.2, 0.2, 0.2, 0.0);

	vec3 eye = vec3(0, 0, 1);
	
	vec4 normal_4 = texture2D( normaltexture, uvCoords );
	vec3 normal = normal_4.xyz;
	
	for (int i = 0; i <maxLights; i++)
	{
		if (lightPositions[i].w <= 0.1)
			DirectionalLight(i, normal, ecPosition3, diffuse, specular);
		else
			PointLight(i, eye, ecPosition3, normal, diffuse, specular);
	}
	/*
	float visibility = 1.0;
	if ( textureProj( shadowMap, ShadowCoords.xy ).z < ShadowCoords.z){
		visibility = 0.5;
	}
	*/
	
	/*
	float visibility =1.0f;
	if ( texture( shadowMap, ShadowCoords.xy ).z < ShadowCoords.z){
		visibility = 0.5f;
	}
	*/
	// for the color texture to calculate the depth
	vec4 shadowColor = texture( shadowMap, ShadowCoords.xy/ShadowCoords.w);
	float visibility = 1.0f;
	float ggg = ShadowCoords.z/ShadowCoords.w;
	float fff = (shadowColor.r+ shadowColor.g+ shadowColor.b)/3.0f;
	float bias = 0.005;
	if (fff< ggg){
		visibility = 0.2;
	}
	fragColor = ambient* frontMaterial.ambient
		+ visibility*shadowColor * diffuse * frontMaterial.diffuse
		+ visibility *shadowColor* specular* frontMaterial.specular;
	//fragColor =shadowColor;
	//fragColor = visibility*shadowColor;
	//fragColor = shadowColor;
	//fragColor = texture2D( normaltexture, uvCoords );
	//fragColor = texture( shadowMap, uvCoords);

} 









