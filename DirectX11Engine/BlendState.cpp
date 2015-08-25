#include "State.h"

BlendState::BlendState()
:	_blendType(BlendType::AlphaBlend),
	_d3dState(NULL)
{
}

BlendState::BlendState(BlendType blendType)
:	_blendType(blendType),
	_d3dState(NULL)
{
}

BlendState::~BlendState()
{
	this->Release();
}

void BlendState::Release()
{
	if (_d3dState) _d3dState->Release();
	_d3dState = NULL;
}

void BlendState::SetBlendType(BlendType blendType)
{
	this->Release();
	_blendType = blendType;
}