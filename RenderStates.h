//***************************************************************************************
// RenderStates.h by Frank Luna (C) 2011 All Rights Reserved.
//   
// Defines render state objects.  
//***************************************************************************************

#ifndef RENDERSTATES_H
#define RENDERSTATES_H

#include "d3dUtil.h"

enum RSType
{
	RSType_RasterizereState,
	RSType_BlendState,
	RSType_DepthStencilState,
	RSType_Num,
};

class RenderState
{
public:
	RenderState(){};

	RSType	RenderStateType;

	union RenderStateData
	{
		ID3D11DepthStencilState* DepthStencilState;
		ID3D11RasterizerState* RasterizerState;
		ID3D11BlendState* BlendState;
	}StateData;

	float BlendFactor[4];

	UINT SimpleMask;
};

class RenderStates
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static ID3D11RasterizerState* WireframeRS;
	static ID3D11RasterizerState* NoCullRS;
	 
	static ID3D11BlendState* AlphaToCoverageBS;
	static ID3D11BlendState* TransparentBS

	static ID3D11DepthStencilState* LessEqualDSS;
}; 

#endif // RENDERSTATES_H