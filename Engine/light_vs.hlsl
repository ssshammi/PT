
/////////////
// DEFINES //
/////////////
#define NUM_LIGHTS 4

cbuffer MatrixBuffer {
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;

};

cbuffer CameraBuffer {
	float3 cameraPosition;
	float padding;
};

cbuffer LightPositionBuffer {
	float4 lightPosition[NUM_LIGHTS];
};

struct VertexInputType {
	float4 position: POSITION;
	float2 tex: TEXCOORD0;
	float3 normal: NORMAL;
};

struct PixelInputType {
	float4 position: SV_POSITION;
	float2 tex: TEXCOORD0;
	float3 normal: NORMAL;
	float3 viewDirection : TEXCOORD1;
	float4 lightPos[NUM_LIGHTS] : TEXCOORD2;
};

//VERTEX SHADER
PixelInputType LightVertexShader(VertexInputType input) {
	PixelInputType output;
	float4 worldPosition;

	input.position.w = 1.0f;

	output.position = mul(input.position,worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;

	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	// Calculate the position of the vertex in the world.
	worldPosition = mul(input.position,worldMatrix);

	output.viewDirection = normalize(cameraPosition.xyz-worldPosition.xyz);



	for (int i = 0; i < NUM_LIGHTS; i++) {
		output.lightPos[i].a = 1.0f;
		output.lightPos[i].xyz = lightPosition[i].xyz - worldPosition.xyz;
		//output.lightPos[i] = normalize(output.lightPos[i]);		//commented as i want the distance of the point light from the player
	}

	return output;
}