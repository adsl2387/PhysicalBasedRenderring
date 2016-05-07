#pragma once
#include "d3dUtil.h"
struct IViewport
{

};

struct D3D11Viewport:public IViewport
{
	D3D11_VIEWPORT Viewport;
};