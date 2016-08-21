#pragma once
#include "ITexture.h"
class VertexShader
{
public:

	VertexShader(void);
	~VertexShader(void);

	void* GetShader() const;

	void SetShader(void* shader);
};


