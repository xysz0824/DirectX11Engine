#ifndef _VERTEX_H
#define _VERTEX_H
#include <xnamath.h>
namespace Engine
{
	struct SpriteVertex
	{
		XMFLOAT3 position;
		XMFLOAT2 texcoord;
	};
	struct ColorVertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};
	struct NormalVertex
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texcoord;
	};
	struct TangentVertex
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT2 texcoord;
	};
	struct FontVertex
	{
		XMFLOAT4 texcoordScale;
		XMFLOAT3 position;
		XMFLOAT3 positionScale;
	};
}
#endif