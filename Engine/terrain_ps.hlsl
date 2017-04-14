////////////////////////////////////////////////////////////////////////////////
// Filename: terrain.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// DEFINES //
/////////////
#define NUM_LIGHTS 4

/////////////
// GLOBALS //
/////////////
Texture2D grassTexture : register(t0);
Texture2D slopeTexture : register(t1);
Texture2D rockTexture : register(t2);
SamplerState SampleType;

cbuffer LightBuffer :register(b0)
{
	float4 ambientColor;
	float4 diffuseColor;
    float3 lightDirection;
	float padding;
};

cbuffer PointLightColorBuffer : register(b1)
{
	float4 pointDiffuseColor[NUM_LIGHTS];
	float4 PointLightRadius[NUM_LIGHTS];
};

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 walkable : COLOR0;
	float4 lightPos[NUM_LIGHTS] : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 TerrainPixelShader(PixelInputType input) : SV_TARGET
{
	float4 grassColor;
	float4 slopeColor;
	float4 rockColor;
	float4 textureColor;
	float slope;
	float blendAmount;
	float3 lightDir;
	float lightIntensity;
	float4 color;
	float4 pointColor[NUM_LIGHTS];

	grassColor = grassTexture.Sample(SampleType,input.tex);
	slopeColor = slopeTexture.Sample(SampleType,input.tex);
	rockColor = rockTexture.Sample(SampleType,input.tex);

	slope = 1-input.normal.y;


	 // Determine which texture to use based on height.
    if(slope < 0.2)
    {
        blendAmount = slope / 0.2f;
        textureColor = lerp(grassColor, slopeColor, blendAmount);
    }
	
    if((slope < 0.7) && (slope >= 0.2f))
    {
        blendAmount = (slope - 0.2f) * (1.0f / (0.7f - 0.2f));
        textureColor = lerp(slopeColor, rockColor, blendAmount);
    }

    if(slope >= 0.7) 
    {
        textureColor = rockColor;
    }

	// Set the default output color to the ambient light value for all pixels.
    color = ambientColor;

	// Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.normal, lightDir));

	if(lightIntensity > 0.0f)
    {
        // Determine the final diffuse color based on the diffuse color and the amount of light intensity.
        color += (diffuseColor * lightIntensity);
    }

	float4 sumOfPointLights = float4(0.0f,0.0f,0.0f,0.0f);
	//Point lights and their colors

	for (int i = 0; i < NUM_LIGHTS; i++) {

		float radius = PointLightRadius[i].x;
		float falloff = PointLightRadius[i].y;
		//getting the distance and setting intensity if too far
		float dist = length(input.lightPos[i]);
		dist = (radius - dist) / (radius-falloff);
		dist = saturate(dist);

		float4 normalizedPos = normalize(input.lightPos[i]);
		float lightIntensity = saturate(dot(input.normal, normalizedPos));

		pointColor[i] = pointDiffuseColor[i] * lightIntensity *dist;
		sumOfPointLights += pointColor[i];

	}

	//color.x = input.walkable.x;
	//see if this works or else try *
	color = color + saturate(sumOfPointLights);

	


    // Saturate the final light color.
    color = saturate(color);
	
	color = color * textureColor;
	

    return color;
}