Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer {
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float specularPower;
	float4 specularColor;
};

struct PixelInputType {
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
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
	

	textureColor = shaderTexture.Sample(SampleType,input.tex);

	color = ambientColor;
	lightDir = -lightDirection;
	lightIntensity = saturate(dot(input.normal,lightDir));
	//color = saturate(diffuseColor * lightIntensity);

	color = lerp(color,color+(diffuseColor*lightIntensity),lightIntensity>0.0f);

	// Saturate the ambient and diffuse color.
	color = saturate(color);

	// Calculate the reflection vector based on the light intensity, normal vector, and light direction.
	reflection = lerp(0.0f, normalize( 2*lightIntensity * input.normal - lightDir), lightIntensity>0.0f);

	// Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
	specular = lerp(float4(0.0f, 0.0f, 0.0f, 0.0f), pow(saturate(dot(reflection, input.viewDirection)), specularPower)*specularColor, lightIntensity>0.0f);


	color = color*textureColor;
	/*float grayscale = dot(color.rgb, float3(0.3, 0.59, 0.11));

	color.r = grayscale;
	color.g = grayscale;
	color.b = grayscale;*/

	color = saturate(color + specular);



	return color;

}