#ifndef _EFFECT_PARSER_H
#define _EFFECT_PARSER_H
#include<d3d11.h>
#include<map>
#include<vector>

namespace Engine
{
	class Game;
	struct PASS_DESC;
	struct TECHNIQUE_DESC;

	typedef std::vector<PASS_DESC> PASS_DESC_VECTOR;
	typedef std::vector<TECHNIQUE_DESC> TECHNIQUE_DESC_VECTOR;
	typedef std::map<const char*, ID3D11InputLayout*>		ID3D11InputLayoutMap;
	typedef std::map<const char*, ID3D11VertexShader*>		ID3D11VertexShaderMap;
	typedef std::map<const char*, ID3D11PixelShader*>			ID3D11PixelShaderMap;
	typedef std::map<UINT, ID3D11Buffer*>							ID3D11BufferMap;

	struct PASS_DESC
	{
		unsigned int										PassIndex;
		std::string											VertexShaderEntry;
		std::string											VertexShaderVersion;
		std::string											PixelShaderEntry;
		std::string											PixelShaderVersion;
	};

	struct TECHNIQUE_DESC
	{
		std::string											TechniqueName;
		PASS_DESC_VECTOR							Passes;
		unsigned int										PassCount;
	};

	struct EFFECT_DESC
	{
		TECHNIQUE_DESC_VECTOR				Techniques;
		unsigned int										TechniqueCount;
	};

	class EffectParser
	{
	public:
		static bool Parse(char* buffer, EFFECT_DESC** effectDesc);
	private:
		static void TechniqueParse(char* buffer, unsigned int start, unsigned int end, TECHNIQUE_DESC& techniqueDesc);
		static void PassParse(char* buffer, unsigned int start, unsigned int end, PASS_DESC& passDesc);
	};

	class Effect
	{
	public:
		Effect();
		~Effect();
		bool Load(Game* game, char* filePath, D3D11_INPUT_ELEMENT_DESC* elementDesc, int totalElements);
		void Release();
		const EFFECT_DESC* GetDesc();
		void UpdateShader(UINT techniqueIndex, UINT passIndex);
		bool CreateConstantBuffer(UINT registerSlot, UINT byteWidth);
		void SetConstantBuffer(UINT registerSlot, const void* pData);
		bool ExistConstantBuffer(UINT registerSlot);
	private:
		ID3D11Device*								_device;
		ID3D11DeviceContext*					_context;
		EFFECT_DESC*								_effectDesc;
		ID3D11BufferMap							_constBuffers;
		ID3D11InputLayoutMap				_inputLayouts;
		ID3D11VertexShaderMap				_vertexShaders;
		ID3D11PixelShaderMap					_pixelShaders;

		void SetVertexShader(const char* entry, ID3D11VertexShader* shader, ID3D11InputLayout* inputLayout);
		bool ExistVertexShader(const char* entry);
		void SetPixelShader(const char* entry, ID3D11PixelShader* shader);
		bool ExistPixelShader(const char* entry);

		Effect(const Effect& rhs);
		Effect& operator=(const Effect& rhs);
	};
}
#endif