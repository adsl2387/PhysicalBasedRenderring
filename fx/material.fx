struct PBRMaterial
{
	float4 diffuse;
	float specular;
	float roughness;
	float metal;
	float pad;
};

struct PBRDirectionalLight
{
	float4 Diffuse;
	float3 Direction;
	float pad;
};

struct PBRPointLight
{ 
	float4 Diffuse;

	float3 Position;
	float Range;

	float3 Att;
	float pad;
};

struct PBRSpotLight
{
	float4 Diffuse;

	float3 Position;
	float Range;

	float3 Direction;
	float Spot;

	float3 Att;
	float pad;
};

TextureCube EnvMap;

SamplerState EnvMapSampler
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;
	AddressU = WRAP;
	AddressV = WRAP;
};