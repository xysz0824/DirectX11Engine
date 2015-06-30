#ifndef _STATE_H
#define _STATE_H
#include <d3d11.h>

enum class StencilFunction
{
	Never = 1,
	Less,
	Equal,
	LessEqual,
	Greater,
	NotEqual,
	GreaterEqual,
	Always
};

enum class StencilOperation
{
	Keep = 1,
	Zero,
	Replace,
	IncreaseSaturate,
	DecreaseSaturate,
	Invert,
	Increase,
	Decrease
};

enum class FillMode
{
	Wireframe = 2,
	Solid
};

enum class CullMode
{
	None = 1,
	Front,
	Back
};

enum class BlendType
{
	AlphaBlend = 1,
	Additive,
	Subtraction,
	Multiply,
	Disable,
	None
};

class DepthStencilState
{
public:
	DepthStencilState();
	DepthStencilState(bool depthEnable, bool stencilEnable);
	DepthStencilState(StencilFunction stencilFunc, StencilOperation stencilFailOp,
		StencilOperation stencilDepthFailOp, StencilOperation stencilPassOp);
	DepthStencilState(bool depthEnable, bool stencilEnable, StencilFunction stencilFunc, StencilOperation stencilFailOp,
		StencilOperation stencilDepthFailOp, StencilOperation stencilPassOp);
	~DepthStencilState();
	void Release();
	void SetDepthEnable(bool depthEnable);
	void SetStencilEnable(bool stencilEnable);
	void SetStencilFunction(StencilFunction stencilFunc);
	void SetStencilFailOp(StencilOperation stencilFailOp);
	void SetStencilDepthFailOp(StencilOperation stencilDepthFailOp);
	void SetStencilPassOp(StencilOperation stencilPassOp);
	bool GetDepthEnable() { return _depthEnable; }
	bool GetStencilEnalbe() { return _stencilEnable; }
	StencilFunction GetStencilFunction() { return _stencilFunc; }
	StencilOperation GetStencilFailOp() { return _stencilFailOp; }
	StencilOperation GetStencilDepthFailOp() { return _stencilDepthFailOp; }
	StencilOperation GetStencilPassOp() { return _stencilPassOp; }
	ID3D11DepthStencilState* GetState() { return _d3dState; }
private:
	bool											_depthEnable;
	bool											_stencilEnable;
	StencilFunction							_stencilFunc;
	StencilOperation						_stencilFailOp;
	StencilOperation						_stencilDepthFailOp;
	StencilOperation						_stencilPassOp;
	ID3D11DepthStencilState*			_d3dState;
};

class RasterizerState
{
public:
	RasterizerState();
	RasterizerState(FillMode fillMode);
	RasterizerState(CullMode cullMode);
	RasterizerState(bool fronterCounterClockwise);
	RasterizerState(FillMode fillMode, CullMode cullMode);
	RasterizerState(FillMode fillMode, CullMode cullMode, bool fronterCounterClockwise);
	~RasterizerState();
	void Release();
	void SetFillMode(FillMode fillMode);
	void SetCullMode(CullMode cullMode);
	void SetFronterCounterClockwise(bool value);
	FillMode GetFillMode() { return _fillMode; }
	CullMode GetCullMode() { return _cullMode; }
	bool GetFronterCounterClockwise() { return _fronterCounterClockwise; }
	ID3D11RasterizerState* GetState() { return _d3dState; }
private:
	FillMode								_fillMode;
	CullMode								_cullMode;
	bool										_fronterCounterClockwise;	
	ID3D11RasterizerState*			_d3dState;
};

class BlendState
{
public:
	BlendState();
	BlendState(BlendType blendType);
	~BlendState();
	void Release();
	void SetBlendType(BlendType blendType);
	BlendType GetBlendType() { return _blendType; }
	ID3D11BlendState* GetState() { return _d3dState; }
private:
	BlendType						_blendType;
	ID3D11BlendState*		_d3dState;
};
#endif