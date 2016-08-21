#include "Brdf.fx"
#include "LightHelper.fx"

void ComputePBRDirectionalLight(float3 diffuse, float3 specular, float roughness, PBRDirectionalLight L, 
                             float3 normal, float3 toEye,
						     out float3 color)
{
	// Initialize outputs.
	color = float3(0.0f, 0.0f, 0.0f);
	

	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = normalize(-L.Direction);
	
	//float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	//[flatten]
	//if( diffuseFactor > 0.0f )
	//{
		color.xyz = PBRColor(normal, toEye, lightVec, diffuse, L.Diffuse.xyz, specular, roughness);
		//color.xyz = diffuse.xyz * diffuseFactor;
		//color.xyz = roughness;
	//}

}

//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a point light.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputePBRPointLight(float4 diffuse, float4 specular, float roughness, float metal,PBRPointLight L, float3 pos, float3 normal, float3 toEye,
				   out float4 color)
{
	// Initialize outputs.
	//ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	//spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The vector from the surface to the light.
	float3 lightVec = L.Position - pos;
		
	// The distance from surface to light.
	float d = length(lightVec);
	
	// Range test.
	if( d > L.Range )
		return;
		
	// Normalize the light vector.
	lightVec /= d; 

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	//[flatten]
	//if( diffuseFactor > 0.0f )
	{
		float3 v         = reflect(-lightVec, normal);
		color.xyz = PBRColor(normal, toEye, lightVec, diffuse.xyz, L.Diffuse.xyz, specular.xyz, roughness);
	}

	// Attenuate
	float att = 1.0f / dot(L.Att, float3(1.0f, d, d*d));

	color *= att;

}

//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a spotlight.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputePBRSpotLight(float4 diffuse, float4 specular, float roughness, float metal, PBRSpotLight L, float3 pos, float3 normal, float3 toEye,
				  out float4 color)
{
	// Initialize outputs.
	color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The vector from the surface to the light.
	float3 lightVec = L.Position - pos;
		
	// The distance from surface to light.
	float d = length(lightVec);
	
	// Range test.
	if( d > L.Range )
		return;
		
	// Normalize the light vector.
	lightVec /= d; 

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	//[flatten]
	//if( diffuseFactor > 0.0f )
	{
		PBRColor(normal, toEye, lightVec, diffuse.xyz, L.Diffuse.xyz, specular.xyz, roughness);
	}
	
	// Scale by spotlight factor and attenuate.
	float spot = pow(max(dot(-lightVec, L.Direction), 0.0f), L.Spot);

	// Scale by spotlight factor and attenuate.
	float att = spot / dot(L.Att, float3(1.0f, d, d*d));


	color *= att;

}

cbuffer cbPerFrame
{
	PBRDirectionalLight gDirLights[3];
	float3 gEyePosW;

	float  gFogStart;
	float  gFogRange;
	float4 gFogColor;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
	PBRMaterial gMaterial;
}; 

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;
Texture2D gPrefilterMap;

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerState samPrefilter
{
	Filter = MIN_MAG_MIP_POINT;

	AddressU = WRAP;
	AddressV = WRAP;
};

struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float3 TangentL: TANGENT;
	float2 Tex     : TEXCOORD;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float2 Tex     : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to world space space.
	vout.PosW    = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);
	vout.TangentW = mul(vin.TangentL, (float3x3)gWorld);
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	return vout;
}
 
float4 PS(VertexOut pin, uniform int gLightCount, uniform bool gUseTexure, uniform bool gAlphaClip, uniform bool gFogEnabled) : SV_Target
{
	// Interpolating normal can unnormalize it, so normalize it.
    pin.NormalW = normalize(pin.NormalW);

	float3 NormalSample = float3(0.5f, 0.5f, 1.0f);

	float3 NormalW = normalize(NormalSampleToWorldSpace(NormalSample, pin.NormalW, pin.TangentW));


	// The toEye vector is used in lighting.
	float3 toEye = gEyePosW - pin.PosW;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye);

	// Normalize.
	toEye /= distToEye;
	
    // Default to multiplicative identity.
    float4 texColor = float4(1, 1, 1, 1);
	float4 baseColor = gMaterial.diffuse;
    if(gUseTexure)
	{
		// Sample texture.
		baseColor = gDiffuseMap.Sample( samAnisotropic, pin.Tex );

		if(gAlphaClip)
		{
			// Discard pixel if texture alpha < 0.1.  Note that we do this
			// test as soon as possible so that we can potentially exit the shader 
			// early, thereby skipping the rest of the shader code.
			clip(baseColor.a - 0.1f);
		}
		
	}
	float3 diffuse = (baseColor - baseColor * gMaterial.metal).xyz;
	float3 specularColor  = lerp( 0.08 * gMaterial.specular.xxxx, baseColor, gMaterial.metal ).xyz;

	//specularColor = float3(0.1, 0.1, 0.1);
	//
	// Lighting.
	//

	float4 color = float4(0.0f, 0.0f, 0.0f,1.0f);
	
	if( gLightCount > 0  )
	{  
		// Start with a sum of zero. 
		
		
		

		// Sum the light contribution from each light source.  
		[unroll]
		for(int i = 0; i < gLightCount; ++i)
		{
			float3 color1;
			ComputePBRDirectionalLight(diffuse, specularColor, gMaterial.roughness, gDirLights[i], NormalW, toEye, 
				color1);

			color.xyz += color1;
		}
	}

	//
	// Fogging
	//

	if( gFogEnabled )
	{
		float fogLerp = saturate( (distToEye - gFogStart) / gFogRange ); 

		// Blend the fog color and the lit color.
		color = lerp(color, gFogColor, fogLerp);
	}

	// Common to take alpha from diffuse material and texture.
	color.a = gMaterial.diffuse.a * texColor.a;

	//color.rgb = LinearToSrgbBranching(color.rgb);
	//color.rgb = sqrt(color.rgb);
	color.rgb = LinearToSrgbBranching(color.rgb);
	//color.rgb = float3(0.5, 0.5, 0.5);
    return color;
}

VertexOut LUTVS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to world space space.
	vout.PosW    = float4(vin.PosL, 1.0f);

	vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);
	vout.TangentW = mul(vin.TangentL, (float3x3)gWorld);
	// Transform to homogeneous clip space.
	vout.PosH = float4(vin.PosL, 1.0f);
	
	// Output vertex attributes for interpolation across triangle.
	vout.Tex = vin.Tex;

	return vout;
}

float4 LUTPS(VertexOut pin) : SV_Target
{
	float4 color = float4(0.f, 0.f, 0.f, 0.f);
	color.xy = IntegrateBRDF(1.0 - pin.Tex.y, pin.Tex.x);
	return color;
}

float4 LUTPSS(VertexOut pin) : SV_Target
{
	float4 color = float4(0.f, 0.f, 0.f, 0.f);
	color.xy = gPrefilterMap.Sample(samPrefilter, pin.Tex).xy;
	return color;
}

technique11 LUTS
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_5_0, LUTVS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, LUTPSS() ) );
	}
}

technique11 LUT
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_5_0, LUTVS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, LUTPS() ) );
	}
}

technique11 Light1
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(1, false, false, false) ) );
    }
}

technique11 Light2
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(2, false, false, false) ) );
    }
}

technique11 Light3
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(3, false, false, false) ) );
    }
}

technique11 Light0Tex
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(0, true, false, false) ) );
    }
}

technique11 Light1Tex
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(1, true, false, false) ) );
    }
}

technique11 Light2Tex
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(2, true, false, false) ) );
    }
}

technique11 Light3Tex
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(3, true, false, false) ) );
    }
}

technique11 Light0TexAlphaClip
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(0, true, true, false) ) );
    }
}

technique11 Light1TexAlphaClip
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(1, true, true, false) ) );
    }
}

technique11 Light2TexAlphaClip
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(2, true, true, false) ) );
    }
}

technique11 Light3TexAlphaClip
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(3, true, true, false) ) );
    }
}

technique11 Light1Fog
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(1, false, false, true) ) );
    }
}

technique11 Light2Fog
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(2, false, false, true) ) );
    }
}

technique11 Light3Fog
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(3, false, false, true) ) );
    }
}

technique11 Light0TexFog
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(0, true, false, true) ) );
    }
}

technique11 Light1TexFog
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(1, true, false, true) ) );
    }
}

technique11 Light2TexFog
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(2, true, false, true) ) );
    }
}

technique11 Light3TexFog
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(3, true, false, true) ) );
    }
}

technique11 Light0TexAlphaClipFog
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(0, true, true, true) ) );
    }
}

technique11 Light1TexAlphaClipFog
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(1, true, true, true) ) );
    }
}

technique11 Light2TexAlphaClipFog
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(2, true, true, true) ) );
    }
}

technique11 Light3TexAlphaClipFog
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(3, true, true, true) ) ); 
    }
}