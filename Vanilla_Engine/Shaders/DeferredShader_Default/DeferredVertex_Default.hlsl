
cbuffer CONSTANT_BUFFER : register(b0) {

	matrix matrixWorld;
	matrix matrixWorldInvTranspose;
	matrix matrixView;
	matrix matrixProjection;
	float4 cameraPos;

};

cbuffer TRANSFORM_BUFFER: register (b1) {

	matrix matrixW;
	matrix entityInvTranspose;
}

struct VS_IN
{
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex: TEXCOORD;
	float3 WPos : WPOSITION;
	float3 ViewPos : CAMERAPOS;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	// Careful not to drop the translation here
	float3 worldPosition = mul(float4(input.Pos, 1.0f), matrixW).xyz;
	output.WPos = worldPosition;

	output.Pos = mul(float4(input.Pos.xyz, 1.0f), matrixW);
	output.Pos = mul(output.Pos, matrixView);
	output.Pos = mul(output.Pos, matrixProjection);

	output.Normal = mul(input.Normal.xyz, (float3x3)entityInvTranspose);

	output.Tex = input.Tex;

	output.ViewPos = cameraPos.xyz - worldPosition.xyz;

	return output;
}