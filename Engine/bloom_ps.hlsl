/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
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
float4 BloomPixelShader(PixelInputType input) : SV_TARGET
{
	float4 t1;
	float gamma = 3.0f, exposure = 1;
	t1 = shaderTexture.Sample(SampleType, input.tex);
	float brightness = dot(t1.xyz, float3(0.2126f, 0.7152f, 0.0722f));

	t1 = lerp(float4(0.0f, 0.0f, 0.0f, 0.0f), t1, brightness > 0.4f);
	
	/*textureColor = (t1 + t2);
	float4 result = float4(1.0f, 1.0f, 1.0f, 1.0f) - exp(-textureColor * exposure);
	result = pow(result, 1.0f / gamma);*/

	return t1;

}