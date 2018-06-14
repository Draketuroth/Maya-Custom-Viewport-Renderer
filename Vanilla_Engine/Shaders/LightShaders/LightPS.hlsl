
SamplerState PointSampler : register(s0);

#define MAX_LIGHTS 10

struct Light {

	float4 Position;
	float4 Color;
};

cbuffer LIGHT_BUFFER : register(b0) {

	float4 nrOfLights;
	Light Pointlights[MAX_LIGHTS];
};

cbuffer CONSTANT_BUFFER : register(b1) {

	matrix matrixWorld;
	matrix matrixWorldInvTranspose;
	matrix matrixView;
	matrix matrixProjection;
	float4 cameraPos;
};

Texture2D gAlbedoSpecTexture : register(t0);
Texture2D gNormalTexture : register(t1);
Texture2D gPositionTexture : register(t2);

struct PS_IN {

	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};

float4 PS_main(PS_IN input) : SV_TARGET
{

	// Retrieve data from G-buffer
	float3 FragPos = gPositionTexture.Sample(PointSampler, input.tex).xyz;
	float3 Normal = gNormalTexture.Sample(PointSampler, input.tex).xyz;
	float3 Albedo = gAlbedoSpecTexture.Sample(PointSampler, input.tex).xyz;

	// Cosine power is stored on the w component of the albedo texture
	float CosinePower = gAlbedoSpecTexture.Sample(PointSampler, input.tex).w;
	
	// Initialize the lightning
	float3 lightning = Albedo * 0.1;

	// Get the direction to the viewer
	float3 viewDir = normalize(cameraPos.xyz - FragPos).xyz;

	for (int i = 0; i < nrOfLights.x; i++) {

		// Calculate the distance between the light and the fragment position
		float lightDistance = length(Pointlights[i].Position.xyz - FragPos);

		// If the distance is smaller than the light radius, this light should contribute to the final color
		if (lightDistance < Pointlights[i].Position.w) {

			// Light intensity is stored on the w component of the light color
			float intensity = Pointlights[i].Color.w;

			// Diffuse
			float3 lightDir = normalize(Pointlights[i].Position.xyz - FragPos);
			Normal = normalize(Normal);

			float3 diffuse = max(dot(Normal, lightDir), 0.0f) * Albedo * (Pointlights[i].Color.xyz * intensity);

			// Specular (Phong)
			// The view vector represents the viewer position in world coordinates
			// The reflection vector represent the reflection of the light direction vector with the the normal n
			
			float3 view = normalize(cameraPos).xyz;
			float3 reflection = reflect(-lightDir.xyz, Normal);
			float3 specular = pow(saturate(dot(reflection, view)), CosinePower);

			// Specular (Blinn-Phong)
			/*float3 HalfwayNormal = normalize(lightDir + viewDir);
			float SpecularHightLightAmount = pow(saturate(dot(Normal, HalfwayNormal)), CosinePower);
			float3 specular = (Pointlights[i].Color.xyz * intensity) * SpecularHightLightAmount;*/

			// Attenuation (Damping the light)
			float distance = length(lightDistance);
			float attenuation = max(0, 1.0f - (distance / Pointlights[i].Position.w));
			diffuse *= attenuation;
			specular *= attenuation;

			lightning += (diffuse + specular);

		}

	}

	return float4(lightning, 1.0f);

}