
cbuffer CONSTANT_BUFFER : register(b0) {

	matrix matrixWorld;
	matrix matrixWorldInvTranspose;
	matrix matrixView;
	matrix matrixProjection;
	float4 cameraPos;

};

struct VS_IN
{
	float3 pos : POSITION;
	float4 color : COLOR;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	input.pos.z = input.pos.z - 50;
	input.pos.x = input.pos.x - 40;

	output.pos = mul(float4(input.pos, 1.0f), matrixWorld);
	output.pos = mul(output.pos, matrixView);
	output.pos = mul(output.pos, matrixProjection);

	output.color = input.color * 3;

	return output;
}