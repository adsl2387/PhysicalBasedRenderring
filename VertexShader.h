#pragma once
#include "ShaderFactory.h"
#include "ITexture.h"
class VertexShader
{
	friend class ShaderFactory;
public:

	VertexShader(void);
	~VertexShader(void);

	void* GetShader() const;

	void SetShader(void* shader);
};


