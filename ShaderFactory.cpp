#include "StdAfx.h"
#include "ShaderFactory.h"
#include "d3dUtil.h"

ShaderFactory GShaderFactory;

ShaderFactory::ShaderFactory(void)
{
}


ShaderFactory::~ShaderFactory(void)
{
}

ShaderFactory* ShaderFactory::GetSingleton()
{
	return &GShaderFactory;
}

Shader* ShaderFactory::CreateShaderFromFile(const char* filename)
{

}