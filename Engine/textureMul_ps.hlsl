
#define amt 1.8

/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture : register(t0);
Texture2D shaderTexture2 : register(t1);
SamplerState SampleType;
//The PixelInputType for the texture pixel shader is also modified using texture coordinates instead of the color values.


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};
//The pixel shader has been modified so that it now uses the HLSL sample function.The sample function uses the sampler state we defined above and the texture coordinates for this pixel.It uses these two variables to determine and return the pixel value for this UV location on the polygon face.

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
//Bloom (Trying something)
	float3 textureColor, t1, t2;

	t1 = shaderTexture.Sample(SampleType, input.tex).rgb;
	t2 = shaderTexture2.Sample(SampleType, input.tex).rgb;

	
	textureColor = lerp(t2+t1,t1,t1.r == 0 && t1.g == 0 && t1.b == 0);
	
	return float4(textureColor,1.0f);


}
