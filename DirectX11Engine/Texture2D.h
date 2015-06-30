#ifndef _TEXTURE2D_H
#define _TEXTURE2D_H
#include <d3d11.h>

class Game;
class DirectX11Game;

class Texture2D
{
public:
	Texture2D();
	~Texture2D();
	bool Create(Game* game, int width, int height);
	bool Load(Game* game, char* filePath);
	void Release();
	void UpdateColorMapAndSampler();
	int GetWidth() { return _width; }
	int GetHeight() { return _height; }
	ID3D11RenderTargetView* GetRenderTargetView() { return _renderTarget; }
private:
	ID3D11DeviceContext*						_context;
	ID3D11ShaderResourceView*			_colorMap;
	ID3D11SamplerState*						_sampler;
	ID3D11RenderTargetView*				_renderTarget;
	int													_width;
	int													_height;

	bool CreateSampler(ID3D11Device* device);

	Texture2D(const Texture2D& rhs);
	Texture2D& operator=(const Texture2D& rhs);
};
#endif