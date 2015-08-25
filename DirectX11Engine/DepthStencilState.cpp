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
	this->Release();
}

void DepthStencilState::Release()
{
	if (_d3dState) _d3dState->Release();
	_d3dState = NULL;
}

void DepthStencilState::SetDepthEnable(bool depthEnable)
{
	this->Release();
	_depthEnable = depthEnable;
}

void DepthStencilState::SetStencilEnable(bool stencilEnable)
{
	this->Release();
	_stencilEnable = stencilEnable;
}

void DepthStencilState::SetStencilFunction(StencilFunction stencilFunc)
{
	this->Release();
	_stencilFunc = stencilFunc;
}

void DepthStencilState::SetStencilFailOp(StencilOperation stencilFailOp)
{
	this->Release();
	_stencilFailOp = stencilFailOp;
}
void DepthStencilState::SetStencilDepthFailOp(StencilOperation stencilDepthFailOp)
{
	this->Release();
	_stencilDepthFailOp = stencilDepthFailOp;
}

void DepthStencilState::SetStencilPassOp(StencilOperation stencilPassOp)
{
	this->Release();
	_stencilPassOp = stencilPassOp;
}