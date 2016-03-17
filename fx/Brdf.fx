#define PI 3.1415926535898f
#include "material.fx"

float LinearToSrgbBranchingChannel(float lin) 
{
	if(lin < 0.00313067) return lin * 12.92;
	return pow(lin, (1.0/2.4)) * 1.055 - 0.055;
}

float3 LinearToSrgbBranching(half3 lin) 
{
	return float3(
		LinearToSrgbBranchingChannel(lin.r),
		LinearToSrgbBranchingChannel(lin.g),
		LinearToSrgbBranchingChannel(lin.b));
}

half3 sRGBToLinear( half3 Color ) 
{
	Color = max(6.10352e-5, Color); // minimum positive non-denormal (fixes black problem on DX11 AMD and NV)
	return Color > 0.04045 ? pow( Color * (1.0 / 1.055) + 0.0521327, 2.4 ) : Color * (1.0 / 12.92);
}

float3 LambertDiffuse(float3 cdiffuse)
{
	return cdiffuse / PI;
}

float Fresnel1(float metal, float3 halfv, float3 lightdir)
{
	float fvalue = 1 - saturate(dot(halfv, lightdir));
	float fvalue2 = fvalue * fvalue;
	float fvalue5 = fvalue2 * fvalue2 * fvalue;
	float fret = metal + (1 - metal) * fvalue5;
	return fret;
}

float3 Fresnel(float3 specular, float3 normal, float3 lightdir)
{
	float fvalue = 1 - saturate(dot(normal, lightdir));
	float fvalue2 = fvalue * fvalue;
	float fvalue5 = fvalue2 * fvalue2 * fvalue;
	float3 fret = specular + (1 - specular) * fvalue5;
	//float3 fret = specular * Fresnel1(metal, halfv, lightdir);
	return fret;
}

float3 FresnelSpherical(float3 specular, float3 normal, float3 lightdir)
{
	float fvalue = saturate(dot(normal, lightdir));
	float ftimes = (-5.55473 * fvalue - 6.98316) * fvalue;
	float fvalue1 = pow(2.f, ftimes);
	float3 fret = specular + (1 - specular) * fvalue1;
	return fret;
}

float NDFBliin(float3 normal, float3 halfv, float roughness)
{
	float a = roughness  ;
	float nh = saturate(dot(normal, halfv));
	float nha = pow(nh, a);
	float g = (a + 2) * nha;
	float ret = g / (2 * PI);
	return ret;
}

float NDFGGX(float3 normal, float3 halfv, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float nh = saturate(dot(normal, halfv));
	float nh2 = nh * nh;
	float g = (nh2 * (a2 - 1) + 1);
	float g2 = g * g;
	float ret = a2 / (PI * g2);
	return ret;
}

float Geo1GGX(float3 normal, float3 v, float k)
{
	float nv = saturate(dot(normal, v));
	float ret = nv / (nv * (1 - k) + k);
	return ret;
}

float GeoGGX(float3 normal, float3 lightdir, float3 view, float roughness)
{
	float k = (roughness + 1)* (roughness + 1) / 8.f;
	float g1 = Geo1GGX(normal, lightdir, k);
	float g2 = Geo1GGX(normal, view, k);
	float ret = g1 * g2;
	return ret;
}

float GeoImplicit(float3 normal, float3 lightdir, float3 view, float roughness)
{
	float nl = saturate(dot(normal, lightdir));
	float nv = saturate(dot(normal, view));
	float ret = nl * nv;
	return ret;
}

float3 ImportanceSampleGGX(float2 Xi, float Roughness, float3 N)
{
	float a = Roughness * Roughness;

	float Phi = 2 * PI * Xi.x;
	float CosTheta = sqrt(( 1 - Xi.y) / (1 + (a * a - 1) * Xi.y) );
	float SinTheta = sqrt( 1 - CosTheta * CosTheta);

	float3 H;
	H.x = SinTheta * cos(Phi);
	H.y = SinTheta * sin(Phi);
	H.z = CosTheta;

	float3 UpVector = abs(N.z) < 0.999 ? float3(0, 0, 1): float3(1, 0 , 0);
	float3 TangentX = normalize( cross(UpVector, N));
	float3 TangentY = cross(N, TangentX);

	return TangentX * H.x + TangentY * H.y + N * H.z;
}

float ReverseBits32(uint i)
{
	const float inverseBits10 = 1.f / 0x3ff;
	uint uReverse = 0;
	for (int k = 0; k < 10; k++)
	{
		uReverse <<= 1;
		uReverse |= (i & 1);
		i >>= 1;
	}
	return uReverse * inverseBits10;
}

float2 Hammersley(uint i, uint NumSamples)
{
	float2 Xi;
	Xi.x = (float)i / NumSamples;
	Xi.y = 	ReverseBits32(i);
	return Xi;
}

float3 SpecularIBL(float3 SpecularColor, float Roughness, float3 N, float3 V)
{
	float3 SpecularLighting = 0;
	const uint NumSamples = 1024;
	for (uint i = 0; i < NumSamples; i++)
	{
		float2 Xi = Hammersley(i, NumSamples);

		float3 H = ImportanceSampleGGX(Xi, Roughness, N);
		float3 L = 2 * dot(V, H) * H - V;

		float NoV = saturate(dot(N, V));
		float NoL = saturate(dot(N, L));
		float NoH = saturate(dot(N, H));
		float VoH = saturate(dot(V, H));
		if (NoL > 0)
		{
			float3 SampleColor = EnvMap.SampleLevel(EnvMapSampler, L, 0).rgb;
			float G = GeoGGX(N, L, V, Roughness);
			float Fc = pow((1 - VoH), 5);
			float3 F = (1 - Fc) * SpecularColor + Fc;
			SpecularLighting += SampleColor * F * G * VoH / (NoH * NoV);
		}
	}

	return SpecularLighting / NumSamples;
}

float3 PBRColor(float3 normal , float3 view, float3 lightdir, float3 cdiffuse, float3 lightcolor,
				float3 specular, float roughness, float metal)
{
	float3 color = float3(0.f,0.f, 0.f);
	float  diffuseFactor = dot(lightdir, normal);
	if (diffuseFactor > 0.f)
	{
		float3 halfv = normalize(lightdir + view);
		//float F1 = Fresnel1(metal, halfv, lightdir);

		float3 incomecolor = PI * lightcolor * saturate(diffuseFactor);
		color += LambertDiffuse(cdiffuse) * incomecolor /** (1 - F1)*/;
		float nl = saturate(diffuseFactor);
		float nv = saturate(dot(normal, view));
		float3 F = FresnelSpherical(specular, halfv, lightdir);
	
		//float3 specularcolor = specular * NDFGGX(normal, halfv, roughness) * GeoGGX(normal, lightdir, view, roughness) * F1
		//						/ (4 * nl * ne);
	
		float3 specularcolor =  NDFGGX(normal, halfv, roughness) * GeoGGX(normal, lightdir, view, roughness) * F
							/ (4 * nl * nv);

		specularcolor = specularcolor * incomecolor;
		color += specularcolor;
	}
	
	//float3 specularcolor = specular * specularfactor;
	float3 environmentSpe = SpecularIBL(specular, roughness, normal, view);

	
	color += environmentSpe;
	//color = lightdir;
	return color;
}