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
	Release();
}

void BlendState::Release()
{
	if (_d3dState) _d3dState->Release();
	_d3dState = NULL;
}

void BlendState::SetBlendType(BlendType blendType)
{
	Release();
	_blendType = blendType;
}