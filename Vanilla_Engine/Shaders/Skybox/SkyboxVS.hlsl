
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
	float3 PosL : POSITION;
};

struct VS_OUT
{
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	
	output.PosH = mul(float4(input.PosL, 1.0f), matrixW).xyzw;
	output.PosH = mul(output.PosH, matrixView);
	output.PosH = mul(output.PosH, matrixProjection);

	// Use the local vertex position as cubemap lookup vector
	output.PosL = input.PosL;

	return output;
}

