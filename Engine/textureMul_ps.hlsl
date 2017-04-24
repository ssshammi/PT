
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
	float4 textureColor, t1, t2;

	t1 = shaderTexture.Sample(SampleType, input.tex);
	t2 = shaderTexture2.Sample(SampleType, input.tex);

	float gamma = 3.0f, exposure = 1.0;
	
	textureColor = (t1+t2);/*
	float4 result = float4(1.0f,1.0f,1.0f,1.0f) - exp(-textureColor * exposure);
	result = pow(abs(result), 1.0f / gamma);*/

	return textureColor;

/*
float2 dir = float2(0.5f,0.5f) - input.tex;
float dist = sqrt(dir.x*dir.x + dir.y*dir.y);
float4 color = shaderTexture.Sample(SampleType, input.tex);
float4 sum = color;

	float px[10];
	px[0] = 0.10f;
	px[1] = 0.09f;
	px[2] = 0.08f;
	px[3] = 0.07f;
	px[4] = 0.06f;
	px[5] = 0.05f;
	px[6] = 0.04f;
	px[7] = 0.03f;
	px[8] = 0.02f;
	px[9] = 0.01f;

	for (int i = 0; i < 10; i++) {
		sum += shaderTexture.Sample(SampleType, input.tex + dir*px[i]);
	}

	sum *= 1.0f / 11.0f;
	float t =clamp(dist * amt,0,1);

	return lerp(color,sum,t);
*/
}
