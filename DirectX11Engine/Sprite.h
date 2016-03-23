#ifndef _SPRITE_H
#define _SPRITE_H
#include <d3d11.h>
#include <xnamath.h>
#include "Mesh.h"

namespace Engine
{
	class DirectX11Game;
	class Texture2D;
	struct InstanceVertex;

	class Sprite
	{
	public:
		Sprite();
		~Sprite();
		bool Init(Game* game, Texture2D* texture);
		void Release();
		void SetPosition(XMFLOAT2 position) { _position = position; }
		void SetOrigin(XMFLOAT2 origin) { _origin = origin; }
		void SetRotation(float rotation) { _rotation = rotation; }
		void SetScale(XMFLOAT2 scale) { _scale = scale; }
		bool SetRect(int x, int y, int width, int height);
		void Draw(bool useSolidShader = true);
		void DrawInstanced(UINT totalInstances, bool useSolidShader = true);
		bool CreateInstanceBuffer(UINT vertexSize, UINT capacity);
		bool MapInstances(void** resourceData);
		void UnMapInstances();
	private:
		Mesh								_mesh;
		DirectX11Game*				_game;
		ID3D11Device*					_device;
		ID3D11DeviceContext*     _context;
		Texture2D*						_texture;
		ID3D11Buffer*					_mwvpConstBuffer;
		XMFLOAT2						_position;
		XMFLOAT2						_origin;
		XMFLOAT2						_scale;
		float									_rotation;

		Sprite(const Sprite& rhs);
		Sprite& operator=(const Sprite& rhs);
		XMMATRIX GetWorldMatrix();
		XMMATRIX GetViewProjectionMatrix();
		void UpdateWorldViewProjectionMatrix(bool worldImpact = true);
	};
}
#endif