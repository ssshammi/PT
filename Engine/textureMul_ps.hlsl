
#define amt 2.5

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
//	float4 textureColor, t1, t2;
//float gamma = 3.0f, exposure = 1;
//t1 = shaderTexture.Sample(SampleType, input.tex);
//t2 = shaderTexture2.Sample(SampleType, input.tex);
//// Sample the pixel color from the texture using the sampler at this texture coordinate location.
////float f1 = t1.x + t1.y + t1.z;
//float brightness = dot(t2.xyz, float3(0.2126f, 0.7152f, 0.0722f));
//
//t2 = lerp(float4(0.0f,0.0f,0.0f,0.0f),t2, brightness>0.4f);
//textureColor = (t1+t2);
//float4 result = float4(1.0f,1.0f,1.0f,1.0f) - exp(-textureColor * exposure);
//result = pow(result, 1.0f / gamma);
//
//return result;
float2 dir = float2(0.5f,0.5f) - input.tex;
float dist = sqrt(dir.x*dir.x + dir.y*dir.y);
float4 color = shaderTexture.Sample(SampleType, input.tex);
float4 sum = color;

	float px[8];
	px[0] = -0.065f;
	px[1] = -0.04f;
	px[2] = -0.02f;
	px[3] = -0.01f;
	px[4] = 0.01f;
	px[5] = 0.02f;
	px[6] = 0.04f;
	px[7] = 0.065f;

	for (int i = 0; i < 8; i++) {
		float2 uv = input.tex + dir*px[i];
		uv.x = lerp(0,uv.x,uv.x<0);
		//uv.y = clamp(uv.y, 0, 1);
		sum += shaderTexture.Sample(SampleType, uv);
	}

	sum *= 1.0f / 9.0f;
	float t =saturate(dist * amt);

	return lerp(color,sum,t);

}
