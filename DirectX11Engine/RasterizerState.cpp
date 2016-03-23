#include "State.h"

namespace Engine
{
	RasterizerState::RasterizerState()
		: _fillMode(FillMode::Solid),
		_cullMode(CullMode::Back),
		_fronterCounterClockwise(false),
		_d3dState(NULL)
	{
	}

	RasterizerState::RasterizerState(FillMode fillMode)
		: _fillMode(fillMode),
		_cullMode(CullMode::Back),
		_fronterCounterClockwise(false),
		_d3dState(NULL)
	{
	}
	RasterizerState::RasterizerState(CullMode cullMode)
		: _fillMode(FillMode::Solid),
		_cullMode(cullMode),
		_fronterCounterClockwise(false),
		_d3dState(NULL)
	{
	}
	RasterizerState::RasterizerState(bool fronterCounterClockwise)
		: _fillMode(FillMode::Solid),
		_cullMode(CullMode::Back),
		_fronterCounterClockwise(fronterCounterClockwise),
		_d3dState(NULL)
	{
	}
	RasterizerState::RasterizerState(FillMode fillMode, CullMode cullMode)
		: _fillMode(fillMode),
		_cullMode(cullMode),
		_fronterCounterClockwise(false),
		_d3dState(NULL)
	{
	}
	RasterizerState::RasterizerState(FillMode fillMode, CullMode cullMode, bool fronterCounterClockwise)
		: _fillMode(fillMode),
		_cullMode(cullMode),
		_fronterCounterClockwise(fronterCounterClockwise),
		_d3dState(NULL)
	{
	}

	RasterizerState::~RasterizerState()
	{
		this->Release();
	}

	void RasterizerState::Release()
	{
		if (_d3dState) _d3dState->Release();
		_d3dState = NULL;
	}

	void RasterizerState::SetFillMode(FillMode fillMode)
	{
		this->Release();
		_fillMode = fillMode;
	}

	void RasterizerState::SetCullMode(CullMode cullMode)
	{
		this->Release();
		_cullMode = cullMode;
	}

	void RasterizerState::SetFronterCounterClockwise(bool value)
	{
		this->Release();
		_fronterCounterClockwise = value;
	}
}