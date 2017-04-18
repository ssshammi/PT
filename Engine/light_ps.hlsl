#define NUM_LIGHTS 10

Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer {
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float specularPower;
	float4 specularColor;
};

cbuffer PointLightColorBuffer
{
	float4 pointDiffuseColor[NUM_LIGHTS];
	float4 PointLightRadius[NUM_LIGHTS];
};

struct PixelInputType {
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
	float4 lightPos[NUM_LIGHTS] : TEXCOORD2;
};

//PixelShader

float4 LightPixelShader(PixelInputType input):SV_TARGET
{
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;
	float3 reflection;
	float4 specular;
	float4 pointColor[NUM_LIGHTS];
	

	textureColor = shaderTexture.Sample(SampleType,input.tex);

	color = ambientColor;
	lightDir = -lightDirection;
	lightIntensity = saturate(dot(input.normal,lightDir));
	//color = saturate(diffuseColor * lightIntensity);

	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Calculate the reflection vector based on the light intensity, normal vector, and light direction.
	if (lightIntensity > 0.0f) {

		color = color + (diffuseColor*lightIntensity);

		// Saturate the ambient and diffuse color.
		color = saturate(color);


		reflection =  normalize(2 * lightIntensity * input.normal - lightDir);

		// Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
		specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower)*specularColor;

	}

	float4 sumOfPointLights = float4(0.0f, 0.0f, 0.0f, 0.0f);
	//Point lights and their colors
	for (int i = 0; i < NUM_LIGHTS; i++) {

		float radius = PointLightRadius[i].x;
		float falloff = PointLightRadius[i].y;
		//getting the distance and setting intensity if too far
		float dist = length(input.lightPos[i]);
		dist = (radius - dist) / (radius - falloff);
		dist = saturate(dist);

		float4 normalizedPos = normalize(input.lightPos[i]);
		float lightIntensity = saturate(dot(input.normal, normalizedPos));

		pointColor[i] = pointDiffuseColor[i] * lightIntensity*dist;
		sumOfPointLights += pointColor[i];
	}

	//adding all point lights
	color = color + saturate(sumOfPointLights);

	color = saturate(color);
	color = color*textureColor;

	//color = color* sumOfAllothercolors 
	/*float grayscale = dot(color.rgb, float3(0.3, 0.59, 0.11));

	color.r = grayscale;
	color.g = grayscale;
	color.b = grayscale;*/

	color = saturate(color + specular);


	return color;

}