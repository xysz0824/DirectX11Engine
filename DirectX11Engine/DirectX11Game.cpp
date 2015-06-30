#include <d3d11.h>
#include <d3dx11.h>
#include <memory>
#include <DxErr.h>
#include <d3dcompiler.h>
#include <Windows.h>
#include <Psapi.h>
#include "DirectX11Game.h"
#include "cpu.hpp"
#include "Effect.h"
#include "Texture2D.h"
#include "State.h"
#include "SpriteFont.h"

extern HINSTANCE HInstance;
extern HWND Hwnd;

int Game::_drawCallCount = 0;

DirectX11Game::DirectX11Game() 
:	_driverType(D3D_DRIVER_TYPE_NULL), 
	_featureLevel(D3D_FEATURE_LEVEL_11_0),
	_d3dDevice(NULL),
	_d3dContext(NULL),
	_swapChain(NULL),
	_backBufferTarget(NULL),
	_depthTexture(NULL),
	_depthStencil(NULL),
	_depthStencilState(NULL),
	_rasterizerState(NULL),
	_blendState(NULL),
	_directInput(NULL),
	_keyboardDevice(NULL),
	_profileFont(NULL)
{
}

DirectX11Game::~DirectX11Game()
{
	Shutdown();
}

bool DirectX11Game::Init()
{
	_hInstance = HInstance;
	_hwnd = Hwnd;

	//Set driver types and feature levels
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_SOFTWARE
	};
	unsigned int totalDriverTypes = ARRAYSIZE(driverTypes);
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	//Obtain size of window
	RECT dimensions;
	GetClientRect(Hwnd, &dimensions);
	_viewportWidth = dimensions.right - dimensions.left;
	_viewportHeight = dimensions.bottom - dimensions.top;

	//Set swap chain.
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = _viewportWidth;
	swapChainDesc.BufferDesc.Height = _viewportHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = Hwnd;
	swapChainDesc.Windowed = true;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	//Create device and swap chain.
	HRESULT result;
	unsigned int driver = 0;
	for (driver = 0; driver < totalDriverTypes; ++driver)
	{
		result = D3D11CreateDeviceAndSwapChain(NULL, driverTypes[driver], NULL, 0,
			featureLevels, totalDriverTypes, D3D11_SDK_VERSION, &swapChainDesc,
			&_swapChain, &_d3dDevice, &_featureLevel, &_d3dContext);
		if (SUCCEEDED(result))
		{
			_driverType = driverTypes[driver];
			break;
		}
	}
	if (FAILED(result))
	{
		DXTRACE_ERR_MSGBOX("Failed to create the Direct3D Device!", result);
		return false;
	}

	//Create render target view.
	ID3D11Texture2D* backBufferTexture;
	result = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferTexture);
	if (FAILED(result))
	{
		DXTRACE_ERR_MSGBOX("Failed to get the swap chain back buffer!", result);
		return false;
	}
	result = _d3dDevice->CreateRenderTargetView(backBufferTexture, NULL, &_backBufferTarget);
	if (backBufferTexture)
		backBufferTexture->Release();
	if (FAILED(result))
	{
		DXTRACE_ERR_MSGBOX("Failed to create the render target view!", result);
		return false;
	}

	//Create depth texture.
	D3D11_TEXTURE2D_DESC	depthTexDesc;
	ZeroMemory(&depthTexDesc, sizeof(depthTexDesc));
	depthTexDesc.Width = _viewportWidth;
	depthTexDesc.Height = _viewportHeight;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;
	result = _d3dDevice->CreateTexture2D(&depthTexDesc, NULL, &_depthTexture);
	if (FAILED(result))
	{
		DXTRACE_ERR_MSGBOX("Failed to create the depth texture!", result);
		return false;
	}

	//Create depth stencil view.
	D3D11_DEPTH_STENCIL_VIEW_DESC desvDesc;
	ZeroMemory(&desvDesc, sizeof(desvDesc));
	desvDesc.Format = depthTexDesc.Format;
	desvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	desvDesc.Texture2D.MipSlice = 0;
	result = _d3dDevice->CreateDepthStencilView(_depthTexture, &desvDesc, &_depthStencil);
	if (FAILED(result))
	{
		DXTRACE_ERR_MSGBOX("Failed to create the depth stencil target view!", result);
		return false;
	}

	//Create depth stencil state.
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	result = _d3dDevice->CreateDepthStencilState(&depthStencilDesc, &_depthStencilState);
	if (FAILED(result))
		return false;

	// Set default depth stencil state.
	_d3dContext->OMSetDepthStencilState(_depthStencilState, 1);

	//Set render target and depth stencil.
	_d3dContext->OMSetRenderTargets(1, &_backBufferTarget, _depthStencil);

	//Create rasterizer state.
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthClipEnable = true;

	result = _d3dDevice->CreateRasterizerState(&rasterizerDesc, &_rasterizerState);
	if (FAILED(result))
		return false;

	//Set default rasterizer state.
	_d3dContext->RSSetState(_rasterizerState);

	//Create blend state.
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	result = _d3dDevice->CreateBlendState(&blendDesc, &_blendState);
	if (FAILED(result))
		return false;

	//Set default blend state.
	//The blend factor generally doesn't work unless be specified.
	_d3dContext->OMSetBlendState(_blendState, blendFactor, 0xFFFFFFFF);

	//Create viewport.
	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(_viewportWidth);
	viewport.Height = static_cast<float>(_viewportHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	_d3dContext->RSSetViewports(1, &viewport);

	//Load font shader.
	bool loadResult;
	D3D11_INPUT_ELEMENT_DESC fontInput[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32_FLOAT, 1, 28, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};
	loadResult = LoadVertexShader(FONT_EFFECT_FILEPATH, FONT_VERTEX_SHADER_ENTRY, FONT_VERTEX_SHADER_VER,
		fontInput, ARRAYSIZE(fontInput), &_fontVS, &_fontInputLayout);
	if (!loadResult)
		return false;
	loadResult = LoadPixelShader(FONT_EFFECT_FILEPATH, FONT_PIXEL_SHADER_ENTRY, FONT_PIXEL_SHADER_VER, &_fontPS);
	if (!loadResult)
		return false;

	//Load solid shader.
	D3D11_INPUT_ELEMENT_DESC solidInput[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	loadResult = LoadVertexShader(SOLID_EFFECT_FILEPATH, SOLID_VERTEX_SHADER_ENTRY, SOLID_VERTEX_SHADER_VER,
		solidInput, ARRAYSIZE(solidInput), &_solidVS, &_solidInputLayout);
	if (!loadResult)
		return false;
	loadResult = LoadPixelShader(SOLID_EFFECT_FILEPATH, SOLID_PIXEL_SHADER_ENTRY, SOLID_PIXEL_SHADER_VER, &_solidPS);
	if (!loadResult)
		return false;

	//Load profile font.
	_profileFont = new SpriteFont;
	_profileFont->Load(this, "Font//Heiti.fnt", 18);

	//Create DirectInput8 and keyboard device.
	ZeroMemory(_prevKeyboardKeys, sizeof(_prevKeyboardKeys));
	result = DirectInput8Create(HInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&_directInput, NULL);
	if (FAILED(result))
	{
		DXTRACE_ERR_MSGBOX("Failed to create DirectInput!", result);
		return false;
	}
	result = _directInput->CreateDevice(GUID_SysKeyboard, &_keyboardDevice, NULL);
	if (FAILED(result))
	{
		DXTRACE_ERR_MSGBOX("Failed to create DirectInput keyboard device!", result);
		return false;
	}
	result = _keyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		DXTRACE_ERR_MSGBOX("Failed to set data format of keyboard device.", result);
		return false;
	}
	result = _keyboardDevice->SetCooperativeLevel(Hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		DXTRACE_ERR_MSGBOX("Failed to set cooperative level!", result);
		return false;
	}
	result = _keyboardDevice->Acquire();
	if (FAILED(result))
	{
		DXTRACE_ERR_MSGBOX("Failed to acquire keyboard device!", result);
		return false;
	}
	if (!LoadContent())
	{
		DXTRACE_ERR_MSGBOX("Failed to load content!", NULL);
		return false;
	}

	return true;
}

void DirectX11Game::SetDepthStencilState(DepthStencilState* depthStencilState)
{
	if (!depthStencilState)
	{
		//Set default depth stencil state.
		_d3dContext->OMSetDepthStencilState(_depthStencilState, 1);
		return;
	}
	auto state = depthStencilState->GetState();
	if (state)
	{
		_d3dContext->OMSetDepthStencilState(depthStencilState->GetState(), 1);
		return;
	}

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = depthStencilState->GetDepthEnable();
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = depthStencilState->GetStencilEnalbe();
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	//The following state is influenced by cull mode, which means if you cull back face, the back face state will lose effect. 
	depthStencilDesc.FrontFace.StencilFailOp = (D3D11_STENCIL_OP)depthStencilState->GetStencilFailOp();
	depthStencilDesc.FrontFace.StencilDepthFailOp = (D3D11_STENCIL_OP)depthStencilState->GetStencilDepthFailOp();
	depthStencilDesc.FrontFace.StencilPassOp = (D3D11_STENCIL_OP)depthStencilState->GetStencilPassOp();
	depthStencilDesc.FrontFace.StencilFunc = (D3D11_COMPARISON_FUNC)depthStencilState->GetStencilFunction();
	depthStencilDesc.BackFace.StencilFailOp = depthStencilDesc.FrontFace.StencilFailOp;
	depthStencilDesc.BackFace.StencilDepthFailOp = depthStencilDesc.FrontFace.StencilDepthFailOp;
	depthStencilDesc.BackFace.StencilPassOp = depthStencilDesc.FrontFace.StencilPassOp;
	depthStencilDesc.BackFace.StencilFunc = depthStencilDesc.FrontFace.StencilFunc;
	HRESULT result = _d3dDevice->CreateDepthStencilState(&depthStencilDesc, &state);
	if (FAILED(result))
		return;

	//Set depth stencil state.
	_d3dContext->OMSetDepthStencilState(state, 1);
}

void DirectX11Game::SetRasterizerState(RasterizerState* rasterizerState)
{
	if (!rasterizerState)
	{
		//Set default rasterizer state.
		_d3dContext->RSSetState(_rasterizerState);
		return;
	}
	auto state = rasterizerState->GetState();
	if (state)
	{
		_d3dContext->RSSetState(state);
		return;
	}

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = (D3D11_FILL_MODE)rasterizerState->GetFillMode();
	rasterizerDesc.CullMode = (D3D11_CULL_MODE)rasterizerState->GetCullMode();
	rasterizerDesc.FrontCounterClockwise = rasterizerState->GetFronterCounterClockwise();
	rasterizerDesc.DepthClipEnable = true;

	HRESULT result = _d3dDevice->CreateRasterizerState(&rasterizerDesc, &state);
	if (FAILED(result))
		return;

	//Set rasterizer state.
	_d3dContext->RSSetState(state);
}

void DirectX11Game::SetBlendState(BlendState* blendState)
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	if (!blendState)
	{
		//Set default blend state.
		_d3dContext->OMSetBlendState(_blendState, blendFactor, 0xFFFFFFFF);
		return;
	}
	auto state = blendState->GetState();
	if (state)
	{
		_d3dContext->OMSetBlendState(state, blendFactor, 0xFFFFFFFF);
		return;
	}

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	//Blend Op : (Csrc X Fsrc) Op (Cdst X Fdst)
	//Fsrc : factor of the color which from we are to draw.
	//Fdst : factor of the color which from frame buffer.
	switch (blendState->GetBlendType())
	{
	case BlendType::AlphaBlend:
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		break;
	case BlendType::Additive:
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		break;
	case BlendType::Subtraction:
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		break;
	case BlendType::Multiply:
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
		break;
	case BlendType::Disable:
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		break;
	case BlendType::None:
		_d3dContext->OMSetBlendState(NULL, blendFactor, 0xFFFFFFFF);
		return;
	}
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HRESULT result = _d3dDevice->CreateBlendState(&blendDesc, &state);
	if (FAILED(result))
		return;

	//Set blend state.
	//The blend factor generally doesn't work unless be specified.
	_d3dContext->OMSetBlendState(state, blendFactor, 0xFFFFFFFF);
}

void DirectX11Game::SetRenderTarget(Texture2D* renderTexture, Color clearColor)
{
	if (!renderTexture)
		_d3dContext->OMSetRenderTargets(1, &_backBufferTarget, _depthStencil);
	else
	{
		auto pRenderTarget = renderTexture->GetRenderTargetView();
		_d3dContext->OMSetRenderTargets(1, &pRenderTarget, _depthStencil);
		_d3dContext->ClearRenderTargetView(renderTexture->GetRenderTargetView(), clearColor.ToArray());
	}
}

bool DirectX11Game::CompileShader(char* filePath, char* entry, char* shaderVersion, ID3DBlob** buffer)
{
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	ID3DBlob* errorBuffer = NULL;
	HRESULT result = D3DX11CompileFromFile(filePath, NULL, NULL, entry, shaderVersion,
		shaderFlags, NULL, NULL, buffer, &errorBuffer, NULL);
	if (FAILED(result))
	{
		if (errorBuffer != NULL)
		{
			OutputDebugStringA((char*)errorBuffer->GetBufferPointer());
			errorBuffer->Release();
		}
		return false;
	}
	if (errorBuffer != NULL)
		errorBuffer->Release();

	return true;
}

bool DirectX11Game::LoadVertexShader(char* filePath, char* entry, char* shaderVersion,
	D3D11_INPUT_ELEMENT_DESC* elementDesc, int totalElements, ID3D11VertexShader** vs, ID3D11InputLayout** layout)
{
	//Compile shader.
	ID3DBlob* vsBuffer = NULL;
	bool compileResult = CompileShader(filePath, entry, shaderVersion, &vsBuffer);
	if (!compileResult)
		return false;

	//Create vertex shader.
	HRESULT d3dResult = _d3dDevice->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), NULL, vs);
	if (FAILED(d3dResult))
	{
		if (vsBuffer)
			vsBuffer->Release();
		return false;
	}

	//Create input layout.
	d3dResult = _d3dDevice->CreateInputLayout(elementDesc, totalElements,
		vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), layout);
	vsBuffer->Release();
	if (FAILED(d3dResult))
		return false;

	return true;
}

bool DirectX11Game::LoadPixelShader(char* filePath, char* entry, char* shaderVersion, ID3D11PixelShader** ps)
{
	//Compile shader.
	ID3DBlob* psBuffer = NULL;
	bool compileResult = CompileShader(filePath, entry, shaderVersion, &psBuffer);
	if (!compileResult)
		return false;

	//Create vertex shader.
	HRESULT d3dResult = _d3dDevice->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), NULL, ps);
	psBuffer->Release();
	if (FAILED(d3dResult))
		return false;

	return true;
}

void DirectX11Game::UpdateSolidShader()
{
	_d3dContext->VSSetShader(_solidVS, NULL, 0);
	_d3dContext->PSSetShader(_solidPS, NULL, 0);
	_d3dContext->IASetInputLayout(_solidInputLayout);
}

void DirectX11Game::UpdateFontShader()
{
	_d3dContext->VSSetShader(_fontVS, NULL, 0);
	_d3dContext->PSSetShader(_fontPS, NULL, 0);
	_d3dContext->IASetInputLayout(_fontInputLayout);
}

void DirectX11Game::DisplayProfile(float dt)
{
	_profileFont->BeginDraw();

	//Draw FPS
	WCHAR fpsChar[15];
	swprintf_s(fpsChar, L"FPS : %.3f", 1 / dt);
	_profileFont->Draw(fpsChar, 0, 0);
	//Draw count of drawcall
	WCHAR drawcallChar[25];
	swprintf_s(drawcallChar, L"Draw call : %d", GetDrawCallCount());
	_profileFont->Draw(drawcallChar, 0, 18);
	//Draw memory used
	WCHAR memChar[35];
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	swprintf_s(memChar, L"Memory used : %dK/%dK", pmc.WorkingSetSize / 1000, pmc.PeakWorkingSetSize / 1000);
	_profileFont->Draw(memChar, 0, 36);

	_profileFont->EndDraw();
}

bool DirectX11Game::LoadContent()
{
	//Override by specifics.
	return true;
}

void DirectX11Game::UnloadContent()
{
	if (_solidVS) _solidVS->Release();
	if (_solidPS) _solidPS->Release();
	if (_solidInputLayout) _solidInputLayout->Release();
	if (_fontVS) _fontVS->Release();
	if (_fontPS) _fontPS->Release();
	if (_fontInputLayout) _fontInputLayout->Release();
	if (_profileFont) delete _profileFont;
	_profileFont = NULL;
	_fontInputLayout = NULL;
	_fontPS = NULL;
	_fontVS = NULL;
	_solidInputLayout = NULL;
	_solidPS = NULL;
	_solidVS = NULL;
}

void DirectX11Game::ClearScreen(Color color)
{
	ResetDrawCallCount();

	_d3dContext->ClearRenderTargetView(_backBufferTarget, color.ToArray());
	_d3dContext->ClearDepthStencilView(_depthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void DirectX11Game::Update(float dt)
{
	//Override by specifics.
}

void DirectX11Game::Render(float dt)
{
	Draw(dt);
	if (_showProfile) 
		DisplayProfile(dt);

	_swapChain->Present(0, 0);
}

void DirectX11Game::Draw(float dt)
{
	//Override by specifics.
}

void DirectX11Game::Shutdown()
{
	UnloadContent();
	if (_blendState) _blendState->Release();
	if (_rasterizerState) _rasterizerState->Release();
	if (_depthStencilState) _depthStencilState->Release();
	if (_depthTexture) _depthTexture->Release();
	if (_depthStencil) _depthStencil->Release();
	if (_backBufferTarget) _backBufferTarget->Release();
	if (_swapChain) _swapChain->Release();
	if (_d3dContext) _d3dContext->Release();
	if (_d3dDevice) _d3dDevice->Release();
	if (_keyboardDevice)
	{
		_keyboardDevice->Unacquire();
		_keyboardDevice->Release();
	}
	if (_directInput) _directInput->Release();
	_directInput = NULL;
	_keyboardDevice = NULL;
	_d3dDevice = NULL;
	_d3dContext = NULL;
	_swapChain = NULL;
	_backBufferTarget = NULL;
	_depthStencil = NULL;
	_depthTexture = NULL;
	_depthStencilState = NULL;
	_rasterizerState = NULL;
	_blendState = NULL;
}

bool DirectX11Game::IsKeyDown(int keyCode)
{
	return (_keyboardKeys[keyCode] & 0x80) != 0;
}

bool DirectX11Game::IsKeyUp(int keyCode)
{
	return (_prevKeyboardKeys[keyCode] & 0x80) && !IsKeyDown(keyCode);
}

bool DirectX11Game::IsKeyPressed(int keyCode)
{
	return !(_prevKeyboardKeys[keyCode] & 0x80) && IsKeyDown(keyCode);
}

void DirectX11Game::UpdateInputState()
{
	_keyboardDevice->GetDeviceState(sizeof(_keyboardKeys), (LPVOID)&_keyboardKeys);
}

void DirectX11Game::SaveInputState()
{
	memcpy(_prevKeyboardKeys, _keyboardKeys, sizeof(_keyboardKeys));
}