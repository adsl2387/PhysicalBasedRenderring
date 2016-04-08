#pragma once
#include "PBObject.h"
class IBuffer : public PBObject
{
public:
	IBuffer(void);
	~IBuffer(void);

	virtual void* GetBuffer()= 0;

	virtual UINT GetStride() = 0;

	virtual UINT GetOffset()= 0;

	virtual UINT GetElementCount() = 0;
};

