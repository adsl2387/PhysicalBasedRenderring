#pragma once
class PixelShader
{
public:
	PixelShader(void);

	~PixelShader(void);

	void* GetShader() const;

	void SetShader(void* shader);
};

