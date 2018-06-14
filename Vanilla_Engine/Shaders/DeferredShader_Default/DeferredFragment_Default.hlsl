
SamplerState texSampler: register(s0);
Texture2D shaderTexture : register(t0);

cbuffer MATERIAL_BUFFER: register(b0) {

	float4 diffuse;
}

struct PS_IN
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex: TEXCOORD;
	float3 WPos : WPOSITION;
	float3 ViewPos : CAMERAPOS;
};

struct PS_OUT {

	float4 Color : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 Position : SV_TARGET2;
};


PS_OUT PS_main(PS_IN input)
{
	PS_OUT output;

	// Sample the color from the texture and multiply it with the material color
	// Store it for output to the render target
	output.Color = lerp(shaderTexture.Sample(texSampler, input.Tex), float4(diffuse.xyz, 1.0f), 0.5f);

	// Get the material cosine power
	output.Color.w = diffuse.w;

	// Store the normal output to the render target
	output.Normal = float4(input.Normal, 1.0f);

	// Store the position output to the render target
	output.Position = float4(input.WPos, 1.0f);

	return output;
};