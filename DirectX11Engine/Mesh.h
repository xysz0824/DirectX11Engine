#ifndef _MESH_H
#define _MESH_H
#include <d3d11.h>
#include <xnamath.h>

class Game;

class Mesh
{
public:
	Mesh();
	~Mesh();
	bool Load(Game* game, const void* vertices, UINT vertexSize, UINT totalVertices, 
		D3D11_PRIMITIVE_TOPOLOGY topology, bool dynamic);
	bool LoadIndexed(Game* game, const void* vertices, UINT vertexSize, UINT totalVertices, 
		D3D11_PRIMITIVE_TOPOLOGY topology, const void* indices, UINT totalIndices, bool dynamic);
	bool LoadFromFile(Game* game, const char* filePath, bool dynamic);
	bool LoadColumn(Game* game, int slice, int stack, float(*f)(float), XMFLOAT4 color, bool dynamic);
	bool LoadSphere(Game* game, int slice, int stack, XMFLOAT4 color, bool dynamic);
	bool LoadSphere(Game* game, int slice, int stack, bool dynamic);
	bool LoadCylinder(Game* game, int slice, int stack, XMFLOAT4 color, bool dynamic);
	bool LoadCylinder(Game* game, int slice, int stack, bool dynamic);
	bool LoadCube(Game* game, XMFLOAT4 color, bool dynamic);
	bool LoadCube(Game* game, bool dynamic);
	bool LoadQuad(Game* game, XMFLOAT4 color, bool dynamic);
	bool LoadQuad(Game* game, bool dynamic);
	bool CreateInstanceBuffer(UINT vertexSize, UINT capacity);
	void Release();
	void Draw();
	void DrawInstanced(UINT totalInstances);
	bool MapVertices(void** resourceData);
	void UnMapVertices();
	bool MapInstances(void** resourceData);
	void UnMapInstances();
private:
	Game*													_game;
	ID3D11Device*										_device;
	ID3D11DeviceContext*							_context;
	D3D11_PRIMITIVE_TOPOLOGY				_topology;
	ID3D11Buffer*										_vertexBuffer;
	UINT														_vertexSize;
	UINT														_totalVertices;
	ID3D11Buffer*										_indexBuffer;
	UINT														_totalIndices;
	ID3D11Buffer*										_instanceBuffer;
	UINT														_instanceSize;
	UINT														_totalInstances;
	bool														_dynamic;
	bool														_unused[3];

	bool MapBuffer(ID3D11Buffer* buffer, void** resourceData);
	void UnMapBuffer(ID3D11Buffer* buffer);

	Mesh(const Mesh& rhs);
	Mesh& operator =(const Mesh& rhs);
};
#endif