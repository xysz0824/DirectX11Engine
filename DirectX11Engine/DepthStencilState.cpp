#include "State.h"

DepthStencilState::DepthStencilState()
:	_depthEnable(true),
	_stencilEnable(true),
	_stencilFunc(StencilFunction::Always),
	_stencilFailOp(StencilOperation::Keep),
	_stencilDepthFailOp(StencilOperation::Keep),
	_stencilPassOp(StencilOperation::Keep)
{
}

DepthStencilState::DepthStencilState(bool depthEnable, bool stencilEnable)
:	_depthEnable(depthEnable),
	_stencilEnable(stencilEnable),
	_stencilFunc(StencilFunction::Always),
	_stencilFailOp(StencilOperation::Keep),
	_stencilDepthFailOp(StencilOperation::Keep),
	_stencilPassOp(StencilOperation::Keep)
{
}

DepthStencilState::DepthStencilState(StencilFunction stencilFunc, StencilOperation stencilFailOp,
	StencilOperation stencilDepthFailOp, StencilOperation stencilPassOp)
:	_depthEnable(true),
	_stencilEnable(true),
	_stencilFunc(stencilFunc),
	_stencilFailOp(stencilFailOp),
	_stencilDepthFailOp(stencilDepthFailOp),
	_stencilPassOp(stencilPassOp)
{
}

DepthStencilState::DepthStencilState(bool depthEnable, bool stencilEnable, 
	StencilFunction stencilFunc, StencilOperation stencilFailOp, 
	StencilOperation stencilDepthFailOp, StencilOperation stencilPassOp)
:	 _depthEnable(depthEnable),
	_stencilEnable(stencilEnable),
	_stencilFunc(stencilFunc),
	_stencilFailOp(stencilFailOp),
	_stencilDepthFailOp(stencilDepthFailOp),
	_stencilPassOp(stencilPassOp)
{
}

DepthStencilState::~DepthStencilState()
{
	Release();
}

void DepthStencilState::Release()
{
	if (_d3dState) _d3dState->Release();
	_d3dState = NULL;
}

void DepthStencilState::SetDepthEnable(bool depthEnable)
{
	Release();
	_depthEnable = depthEnable;
}

void DepthStencilState::SetStencilEnable(bool stencilEnable)
{
	Release();
	_stencilEnable = stencilEnable;
}

void DepthStencilState::SetStencilFunction(StencilFunction stencilFunc)
{
	Release();
	_stencilFunc = stencilFunc;
}

void DepthStencilState::SetStencilFailOp(StencilOperation stencilFailOp)
{
	Release();
	_stencilFailOp = stencilFailOp;
}
void DepthStencilState::SetStencilDepthFailOp(StencilOperation stencilDepthFailOp)
{
	Release();
	_stencilDepthFailOp = stencilDepthFailOp;
}

void DepthStencilState::SetStencilPassOp(StencilOperation stencilPassOp)
{
	Release();
	_stencilPassOp = stencilPassOp;
}