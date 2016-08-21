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

float Pow5(float value)
{
	float value2 = value * value;
	return value2 * value2 * value;
}

float Square(float value)
{
	return value * value;
}

float3 F_None( float3 SpecularColor )
{
	return SpecularColor;
}

float3 F_Schlick( float3 SpecularColor, float VoH )
{
	float Fc = Pow5( 1 - VoH );					// 1 sub, 3 mul
	//return Fc + (1 - Fc) * SpecularColor;		// 1 add, 3 mad
	
	// Anything less than 2% is physically impossible and is instead considered to be shadowing
	return saturate( 50.0 * SpecularColor.g ) * Fc + (1 - Fc) * SpecularColor;
	
}

float3 F_Fresnel( float3 SpecularColor, float VoH )
{
	float3 SpecularColorSqrt = sqrt( clamp( float3(0, 0, 0), float3(0.99, 0.99, 0.99), SpecularColor ) );
	float3 n = ( 1 + SpecularColorSqrt ) / ( 1 - SpecularColorSqrt );
	float3 g = sqrt( n*n + VoH*VoH - 1 );
	return 0.5 * Square( (g - VoH) / (g + VoH) ) * ( 1 + Square( ((g+VoH)*VoH - 1) / ((g-VoH)*VoH + 1) ) );
}

float Vis_Implicit()
{
	return 0.25;
}

float Vis_SmithJointApprox( float Roughness, float NoV, float NoL )
{
	float a = Square( Roughness );
	float Vis_SmithV = NoL * ( NoV * ( 1 - a ) + a );
	float Vis_SmithL = NoV * ( NoL * ( 1 - a ) + a );
	return 0.5 * rcp( Vis_SmithV + Vis_SmithL );
}

float D_GGX( float Roughness, float NoH )
{
	float a = Roughness * Roughness;
	float a2 = a * a;
	float d = ( NoH * a2 - NoH ) * NoH + 1;	// 2 mad
	return a2 / ( PI*d*d );					// 4 mul, 1 rcp
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
				float3 specular, float roughness)
{
	float3 color = float3(0.f,0.f, 0.f);
	float  diffuseFactor = dot(lightdir, normal);
	if (diffuseFactor > 0.f)
	{
		float3 halfv = normalize(lightdir + view);

		float3 incomecolor = PI * lightcolor * saturate(diffuseFactor);
		//color += LambertDiffuse(cdiffuse) * incomecolor /** (1 - F1)*/;


		float NoL = saturate( dot(normal, lightdir) );
		float NoV = saturate( abs( dot(normal, view) ) + 1e-5 );
		float NoH = saturate( dot(normal, halfv) );
		float VoH = saturate( dot(view, halfv) );
	
		// Generalized microfacet specular
		float D = D_GGX( roughness, NoH ) ;
		float Vis = Vis_SmithJointApprox( roughness, NoV, NoL );
		//float Vis = Vis_Implicit();
		float3 F = F_Schlick(specular, VoH );


	
		float3 specularcolor = (D * Vis) * F;

		color = incomecolor * (LambertDiffuse(cdiffuse) + specularcolor);
		color = Vis;
	}
	
	//float3 specularcolor = specular * specularfactor;
	//float3 environmentSpe = SpecularIBL(specular, roughness, normal, view);

	
	//color += environmentSpe;
	//color = lightdir;
	return color;
}

float4 ImportanceSampleGGX( float2 E, float Roughness )
{
	float m = Roughness * Roughness;
	float m2 = m * m;

	float Phi = 2 * PI * E.x;
	float CosTheta = sqrt( (1 - E.y) / ( 1 + (m2 - 1) * E.y ) );
	float SinTheta = sqrt( 1 - CosTheta * CosTheta );

	float3 H;
	H.x = SinTheta * cos( Phi );
	H.y = SinTheta * sin( Phi );
	H.z = CosTheta;
	
	float d = ( CosTheta * m2 - CosTheta ) * CosTheta + 1;
	float D = m2 / ( PI*d*d );
	float PDF = D * CosTheta;

	return float4( H, PDF );
}

float2 IntegrateBRDF( float Roughness, float NoV )
{
	float3 V;
	V.x = sqrt( 1.0f - NoV * NoV );	// sin
	V.y = 0;
	V.z = NoV;						// cos

	float A = 0;
	float B = 0;
	float C = 0;

	const uint NumSamples = 1024;
	for( uint i = 0; i < NumSamples; i++ )
	{
		float2 E = Hammersley( i, NumSamples );

		{
			float3 H = ImportanceSampleGGX( E, Roughness ).xyz;
			float3 L = 2 * dot( V, H ) * H - V;

			float NoL = saturate( L.z );
			float NoH = saturate( H.z );
			float VoH = saturate( dot( V, H ) );

			if( NoL > 0 )
			{
				float Vis = Vis_SmithJointApprox( Roughness, NoV, NoL );

				float a = Square( Roughness );
				float a2 = a*a;
				float Vis_SmithV = NoL * sqrt( NoV * (NoV - NoV * a2) + a2 );
				float Vis_SmithL = NoV * sqrt( NoL * (NoL - NoL * a2) + a2 );
				//float Vis = 0.5 * rcp( Vis_SmithV + Vis_SmithL );

				// Incident light = NoL
				// pdf = D * NoH / (4 * VoH)
				// NoL * Vis / pdf
				float NoL_Vis_PDF = NoL * Vis * (4 * VoH / NoH);

				float Fc = pow( 1 - VoH, 5 );
				A += (1 - Fc) * NoL_Vis_PDF;
				B += Fc * NoL_Vis_PDF;
			}
		}
	}

	return float2( A, B) / NumSamples;
}