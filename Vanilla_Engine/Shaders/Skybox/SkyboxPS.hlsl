
TextureCube gCubeMap : register(t0);
SamplerState samTriLinearSam : register(s0);
							  
struct PS_IN
{
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
};

float4 PS_main(PS_IN input) : SV_TARGET
{

	float4 color;

	color = gCubeMap.Sample(samTriLinearSam, input.PosL);

	return color * 4;
}
