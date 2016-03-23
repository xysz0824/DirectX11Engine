#include <d3dx11.h>
#include "DirectX11Game.h"
#include "Texture2D.h"

namespace Engine
{
	Texture2D::Texture2D()
		: _context(NULL),
		_colorMap(NULL),
		_sampler(NULL),
		_renderTarget(NULL)
	{
	}

	Texture2D::~Texture2D()
	{
		this->Release();
	}

	bool Texture2D::Create(Game* game, int width, int height)
	{
		auto dx11 = dynamic_cast<DirectX11Game*>(game);
		if (!dx11)
			return false;
		//Get device and context.
		auto device = dx11->GetDevice();
		_context = dx11->GetDeviceContext();

		//Create texture.
		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.SampleDesc.Count = 1;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		ID3D11Texture2D* tex = NULL;
		HRESULT result = device->CreateTexture2D(&texDesc, NULL, &tex);
		if (FAILED(result))
			return false;

		//Create render target view.
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format = texDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		result = device->CreateRenderTargetView(tex, &renderTargetViewDesc, &_renderTarget);
		if (FAILED(result))
			return false;

		//Create shader resource view.
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format = texDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		result = device->CreateShaderResourceView(tex, &shaderResourceViewDesc, &_colorMap);
		if (FAILED(result))
			return false;

		//Create color map sampler.
		if (!this->CreateSampler(device))
			return false;

		//Get width and height. 
		_width = width;
		_height = height;

		return true;
	}

	bool Texture2D::Load(Game* game, char* filePath)
	{
		auto dx11 = dynamic_cast<DirectX11Game*>(game);
		if (!dx11)
			return false;
		//Get device and context.
		auto device = dx11->GetDevice();
		_context = dx11->GetDeviceContext();

		//Create shader resource view.
		HRESULT d3dResult = D3DX11CreateShaderResourceViewFromFile(device, filePath, NULL, NULL, &_colorMap, NULL);
		if (FAILED(d3dResult))
			return false;

		//Create color map sampler.
		if (!this->CreateSampler(device))
			return false;

		//Get width and height. 
		ID3D11Resource* colorTex;
		_colorMap->GetResource(&colorTex);
		D3D11_TEXTURE2D_DESC colorTexDesc;
		((ID3D11Texture2D*)colorTex)->GetDesc(&colorTexDesc);
		colorTex->Release();
		if (colorTexDesc.ArraySize > 1)
		{
			this->Release();
			return false;
		}
		_width = colorTexDesc.Width;
		_height = colorTexDesc.Height;

		return true;
	}

	void Texture2D::Release()
	{
		if (_colorMap) _colorMap->Release();
		if (_sampler) _sampler->Release();
		if (_renderTarget) _renderTarget->Release();
		_renderTarget = NULL;
		_sampler = NULL;
		_colorMap = NULL;
	}

	bool Texture2D::CreateSampler(ID3D11Device* device)
	{
		D3D11_SAMPLER_DESC sampleDesc;
		ZeroMemory(&sampleDesc, sizeof(sampleDesc));
		sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampleDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		sampleDesc.MaxAnisotropy = 0;
		sampleDesc.MinLOD = 0;
		sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
		sampleDesc.MipLODBias = 0.0f;
		HRESULT d3dResult = device->CreateSamplerState(&sampleDesc, &_sampler);
		if (FAILED(d3dResult))
			return false;

		return true;
	}

	void Texture2D::UpdateColorMapAndSampler()
	{
		if (!_context)
			return;

		_context->PSSetShaderResources(0, 1, &_colorMap);
		_context->PSSetSamplers(0, 1, &_sampler);
	}
}