#ifndef _BASE_H
#define _BASE_H
#include <d3d11.h>
#include <dinput.h>
#include "Game.h"

namespace Engine
{

	class DepthStencilState;
	class RasterizerState;
	class BlendState;
	class Texture2D;
	class SpriteFont;

#define SOLID_EFFECT_FILEPATH					"Sprite.fx"
#define SOLID_VERTEX_SHADER_ENTRY		"VS_Main"
#define SOLID_VERTEX_SHADER_VER			"vs_4_0"
#define SOLID_PIXEL_SHADER_ENTRY			"PS_Main"
#define SOLID_PIXEL_SHADER_VER				"ps_4_0"
#define FONT_EFFECT_FILEPATH					"Font.fx"
#define FONT_VERTEX_SHADER_ENTRY		"VS_Main"
#define FONT_VERTEX_SHADER_VER				"vs_4_0"
#define FONT_PIXEL_SHADER_ENTRY			"PS_Main"
#define FONT_PIXEL_SHADER_VER				"ps_4_0"

	class DirectX11Game : public Game
	{
	public:
		DirectX11Game();
		virtual ~DirectX11Game();
		virtual bool Init();
		virtual void Shutdown();
		virtual void Update(float dt);
		virtual void Render(float dt);
		ID3D11Device* GetDevice() { return _d3dDevice; }
		ID3D11DeviceContext* GetDeviceContext() { return _d3dContext; }
		bool LoadVertexShader(char* filePath, char* entry, char* shaderVersion,
			D3D11_INPUT_ELEMENT_DESC* elementDesc, int totalElements, ID3D11VertexShader** vs, ID3D11InputLayout** layout);
		bool LoadPixelShader(char* filePath, char* entry, char* shaderVersion, ID3D11PixelShader** ps);
		void UpdateSolidShader();
		void UpdateFontShader();
	protected:
		virtual bool LoadContent();
		virtual void UnloadContent();
		virtual void ClearScreen(Color color);
		virtual void Draw(float dt);
		void SetProfileDisplay(bool enable) { _showProfile = enable; }
		void SetDepthStencilState(DepthStencilState* depthStencilState);
		void SetRasterizerState(RasterizerState* rasterizerState);
		void SetBlendState(BlendState* blendState);
		void SetRenderTarget(Texture2D* renderTexture, Color clearColor = Color::Black());
		bool IsKeyDown(int keyCode);
		bool IsKeyUp(int keyCode);
		bool IsKeyPressed(int keyCode);
		void UpdateInputState();
		void SaveInputState();
	private:
		HINSTANCE									_hInstance;
		HWND											_hwnd;

		D3D_DRIVER_TYPE						_driverType;
		D3D_FEATURE_LEVEL					_featureLevel;
		ID3D11Device*								_d3dDevice;
		ID3D11DeviceContext*					_d3dContext;
		IDXGISwapChain*							_swapChain;
		ID3D11RenderTargetView*			_backBufferTarget;
		ID3D11Texture2D*						_depthTexture;
		ID3D11DepthStencilView*				_depthStencil;
		ID3D11DepthStencilState*				_depthStencilState;
		ID3D11RasterizerState*					_rasterizerState;
		ID3D11BlendState*						_blendState;

		ID3D11VertexShader*					_fontVS;
		ID3D11PixelShader*						_fontPS;
		ID3D11InputLayout*						_fontInputLayout;

		ID3D11VertexShader*					_solidVS;
		ID3D11PixelShader*						_solidPS;
		ID3D11InputLayout*						_solidInputLayout;

		LPDIRECTINPUT8							_directInput;
		LPDIRECTINPUTDEVICE8				_keyboardDevice;
		char												_keyboardKeys[256];
		char												_prevKeyboardKeys[256];

		SpriteFont*									_profileFont;
		bool												_showProfile;

		static bool CompileShader(char* filePath, char* entry, char* shaderVersion, ID3DBlob** buffer);
		void DisplayProfile(float dt);

		DirectX11Game(const DirectX11Game& rhs);
		DirectX11Game& operator =(const DirectX11Game& rhs);
	};
}
#endif