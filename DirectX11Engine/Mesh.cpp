#include <fstream>
#include "DirectX11Game.h"
#include "Mesh.h"
#include "Vertex.h"

namespace Engine
{
	union Byte_Type
	{
		char b[4];
		float f;
		int i;
	};

	Mesh::Mesh()
		: _device(NULL),
		_context(NULL),
		_vertexBuffer(NULL),
		_indexBuffer(NULL),
		_instanceBuffer(NULL)
	{
	}

	Mesh::~Mesh()
	{
		this->Release();
	}

	bool Mesh::Load(Game* game, const void* vertices, UINT vertexSize, UINT totalVertices,
		D3D11_PRIMITIVE_TOPOLOGY topology, bool dynamic)
	{
		_game = game;
		auto dx11 = dynamic_cast<DirectX11Game*>(game);
		if (!dx11)
			return false;
		//Get device and context.
		_device = dx11->GetDevice();
		_context = dx11->GetDeviceContext();

		_vertexSize = vertexSize;
		_totalVertices = totalVertices;
		_dynamic = dynamic;
		_topology = topology;
		//Create vertex buffer
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		if (dynamic)
		{
			vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		else
		{
			vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		}
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.ByteWidth = vertexSize * totalVertices;
		D3D11_SUBRESOURCE_DATA vertexData;
		ZeroMemory(&vertexData, sizeof(vertexData));
		vertexData.pSysMem = vertices;
		HRESULT result = _device->CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer);
		if (FAILED(result))
			return false;

		return true;
	}

	bool Mesh::LoadIndexed(Game* game, const void* vertices, UINT vertexSize, UINT totalVertices,
		D3D11_PRIMITIVE_TOPOLOGY topology, const void* indices, UINT totalIndices, bool dynamic)
	{
		_totalIndices = totalIndices;

		bool result = this->Load(game, vertices, vertexSize, totalVertices, topology, dynamic);
		if (!result)
			return false;
		//Create index buffer
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth = sizeof(WORD)* totalIndices;
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = indices;
		HRESULT d3dResult = _device->CreateBuffer(&desc, &data, &_indexBuffer);
		if (FAILED(d3dResult))
			return false;

		return true;
	}

	bool Mesh::LoadFromFile(Game* game, const char* filePath, bool dynamic)
	{
		std::ifstream fs(filePath, std::ios::binary);
		if (!fs.is_open())
			return false;

		char head[4];
		fs.read(head, 4);
		if (!(head[0] == 102 && head[1] == 77 && head[2] == 62 && head[3] == 97))
			return false;

		Byte_Type vertexCount;
		fs.read(vertexCount.b, 4);
		NormalVertex* vertices = new NormalVertex[vertexCount.i];
		for (int i = 0; i < vertexCount.i; ++i)
		{
			Byte_Type vx, vy, vz;
			fs.read(vx.b, 4);
			fs.read(vy.b, 4);
			fs.read(vz.b, 4);
			Byte_Type vnx, vny, vnz;
			fs.read(vnx.b, 4);
			fs.read(vny.b, 4);
			fs.read(vnz.b, 4);
			Byte_Type vtx, vty, vtz;
			fs.read(vtx.b, 4);
			fs.read(vty.b, 4);
			fs.read(vtz.b, 4);
			vertices[i].position = XMFLOAT3(vx.f, vy.f, vz.f);
			vertices[i].normal = XMFLOAT3(vnx.f, vny.f, vnz.f);
			vertices[i].texcoord = XMFLOAT2(vtx.f, vty.f);
		}
		Byte_Type indexCount;
		fs.read(indexCount.b, 4);
		WORD* indices = new WORD[indexCount.i];
		for (int i = 0; i < indexCount.i; ++i)
		{
			Byte_Type index;
			fs.read(index.b, 4);
			indices[i] = (WORD)index.i - 1;
		}
		fs.close();
		bool result = this->LoadIndexed(game, vertices, sizeof(NormalVertex), vertexCount.i,
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indices, indexCount.i, dynamic);
		delete[] vertices;
		delete[] indices;
		return result;
	}

	bool Mesh::LoadColumn(Game* game, int slice, int stack, float(*f)(float), XMFLOAT4 color, bool dynamic)
	{
		int vertsPerRow = slice + 1;
		int nRows = stack - 1;

		int totalVertices = vertsPerRow * nRows + 2;
		int totalIndices = nRows * slice * 6;

		ColorVertex* vertices = new ColorVertex[totalVertices];
		for (int i = 1; i <= nRows; ++i)
		{
			float phy = XM_PI * i / stack;
			float tmpRadius = f(phy);
			for (int j = 0; j < vertsPerRow; ++j)
			{
				float theta = XM_2PI * j / slice;
				UINT index = (i - 1)*vertsPerRow + j;
				float x = tmpRadius*cos(theta);
				float y = cos(phy);
				float z = tmpRadius*sin(theta);
				vertices[index] = { XMFLOAT3(x, y, z), color };
			}
		}
		int size = vertsPerRow * nRows;
		vertices[size] = { XMFLOAT3(0.f, 1, 0.f), color };
		vertices[size + 1] = { XMFLOAT3(0.f, -1, 0.f), color };

		WORD* indices = new WORD[totalIndices];
		int tmp = 0;
		int start1 = 0;
		int start2 = totalVertices - vertsPerRow - 2;
		int top = size;
		int bottom = size + 1;
		for (int i = 0; i < slice; ++i)
		{
			indices[tmp] = (WORD)top;
			indices[tmp + 1] = (WORD)(start1 + i + 1);
			indices[tmp + 2] = (WORD)(start1 + i);
			tmp += 3;
		}
		for (int i = 0; i < slice; ++i)
		{
			indices[tmp] = (WORD)bottom;
			indices[tmp + 1] = (WORD)(start2 + i);
			indices[tmp + 2] = (WORD)(start2 + i + 1);
			tmp += 3;
		}
		for (int i = 0; i < nRows - 1; ++i)
		{
			for (int j = 0; j < slice; ++j)
			{
				indices[tmp] = (WORD)(i * vertsPerRow + j);
				indices[tmp + 1] = (WORD)((i + 1) * vertsPerRow + j + 1);
				indices[tmp + 2] = (WORD)((i + 1) * vertsPerRow + j);
				indices[tmp + 3] = (WORD)(i * vertsPerRow + j);
				indices[tmp + 4] = (WORD)(i * vertsPerRow + j + 1);
				indices[tmp + 5] = (WORD)((i + 1) * vertsPerRow + j + 1);
				tmp += 6;
			}
		}
		bool result = this->LoadIndexed(game, vertices, sizeof(ColorVertex), totalVertices,
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indices, totalIndices, dynamic);
		delete[] vertices;
		delete[] indices;
		return result;
	}

	bool Mesh::LoadSphere(Game* game, int slice, int stack, XMFLOAT4 color, bool dynamic)
	{
		return this->LoadColumn(game, slice, stack, [](float x)->float {return sin(x); }, color, dynamic);
	}

	bool Mesh::LoadSphere(Game* game, int slice, int stack, bool dynamic)
	{
		int vertsPerRow = slice + 1;
		int totalVertices = (slice + 1) * (stack + 1);
		int totalIndices = stack * slice * 6;

		NormalVertex* vertices = new NormalVertex[totalVertices];
		for (int i = 0; i < stack + 1; ++i)
		{
			float phy = XM_PI * i / stack;
			float tmpRadius = sin(phy);
			for (int j = 0; j < vertsPerRow; ++j)
			{
				float theta = XM_2PI * j / slice;
				UINT index = i * vertsPerRow + j;
				float x = tmpRadius*cos(theta);
				float y = cos(phy);
				float z = tmpRadius*sin(theta);
				vertices[index].position = XMFLOAT3(x, y, z);
				auto vector = XMLoadFloat3(&vertices[index].position);
				XMStoreFloat3(&vertices[index].normal, XMVector3Normalize(vector));
				vertices[index].texcoord = XMFLOAT2((float)j / slice, (float)i / stack);
			}
		}

		WORD* indices = new WORD[totalIndices];
		int tmp = 0;
		for (int i = 0; i < stack; ++i)
		{
			for (int j = 0; j < slice; ++j)
			{
				indices[tmp] = (WORD)(i * vertsPerRow + j);
				indices[tmp + 1] = (WORD)((i + 1) * vertsPerRow + j + 1);
				indices[tmp + 2] = (WORD)((i + 1) * vertsPerRow + j);
				indices[tmp + 3] = (WORD)(i * vertsPerRow + j);
				indices[tmp + 4] = (WORD)(i * vertsPerRow + j + 1);
				indices[tmp + 5] = (WORD)((i + 1) * vertsPerRow + j + 1);
				tmp += 6;
			}
		}
		bool result = this->LoadIndexed(game, vertices, sizeof(NormalVertex), totalVertices,
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indices, totalIndices, dynamic);
		delete[] vertices;
		delete[] indices;
		return result;
	}

	bool Mesh::LoadCylinder(Game* game, int slice, int stack, XMFLOAT4 color, bool dynamic)
	{
		return LoadColumn(game, slice, stack, [](float) ->float{return 1; }, color, dynamic);
	}

	bool Mesh::LoadCylinder(Game* game, int slice, int stack, bool dynamic)
	{
		int vertsPerRow = slice + 1;
		int totalVertices = (slice + 1) * (stack + 1);
		int totalIndices = stack * slice * 6;

		NormalVertex* vertices = new NormalVertex[totalVertices];
		for (int i = 0; i < stack + 1; ++i)
		{
			float tmpRadius = 1;
			if (i == 0 || i == stack)
				tmpRadius = 0;
			for (int j = 0; j < vertsPerRow; ++j)
			{
				float theta = XM_2PI * j / slice;
				UINT index = i * vertsPerRow + j;
				float x = tmpRadius*cos(theta);
				float y = cos(XM_PI * i / stack);
				float z = tmpRadius*sin(theta);
				vertices[index].position = XMFLOAT3(x, y, z);
				if (i == 0 || i == stack)
				{
					auto vector = XMLoadFloat3(&vertices[index].position);
					XMStoreFloat3(&vertices[index].normal, XMVector3Normalize(vector));
				}
				else
				{
					auto vector = XMVectorSet(vertices[index].position.x, 0, vertices[index].position.z, 1);
					XMStoreFloat3(&vertices[index].normal, XMVector3Normalize(vector));
				}
				vertices[index].texcoord = XMFLOAT2((float)j / slice, (float)i / stack);
			}
		}

		WORD* indices = new WORD[totalIndices];
		int tmp = 0;
		for (int i = 0; i < stack; ++i)
		{
			for (int j = 0; j < slice; ++j)
			{
				indices[tmp] = (WORD)(i * vertsPerRow + j);
				indices[tmp + 1] = (WORD)((i + 1) * vertsPerRow + j + 1);
				indices[tmp + 2] = (WORD)((i + 1) * vertsPerRow + j);
				indices[tmp + 3] = (WORD)(i * vertsPerRow + j);
				indices[tmp + 4] = (WORD)(i * vertsPerRow + j + 1);
				indices[tmp + 5] = (WORD)((i + 1) * vertsPerRow + j + 1);
				tmp += 6;
			}
		}
		bool result = this->LoadIndexed(game, vertices, sizeof(NormalVertex), totalVertices,
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indices, totalIndices, dynamic);
		delete[] vertices;
		delete[] indices;
		return result;
	}

	bool Mesh::LoadCube(Game* game, XMFLOAT4 color, bool dynamic)
	{
		ColorVertex vertices[] =
		{
			{ XMFLOAT3(-0.5f, 0.5f, -0.5f), color },
			{ XMFLOAT3(-0.5f, 0.5f, 0.5f), color },
			{ XMFLOAT3(0.5f, 0.5f, 0.5f), color },
			{ XMFLOAT3(0.5, 0.5f, -0.5f), color },

			{ XMFLOAT3(-0.5f, -0.5f, -0.5f), color },
			{ XMFLOAT3(-0.5f, -0.5f, 0.5f), color },
			{ XMFLOAT3(0.5f, -0.5f, 0.5f), color },
			{ XMFLOAT3(0.5, -0.5f, -0.5f), color },
		};
		WORD indices[] =
		{
			0, 3, 4, 3, 7, 4,
			3, 2, 7, 2, 6, 7,
			1, 0, 5, 0, 4, 5,
			1, 2, 0, 2, 3, 0,
			4, 7, 5, 7, 6, 5,
			2, 1, 6, 1, 5, 6
		};
		return this->LoadIndexed(game, vertices, sizeof(ColorVertex), ARRAYSIZE(vertices),
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indices, ARRAYSIZE(indices), dynamic);
	}

	bool Mesh::LoadCube(Game* game, bool dynamic)
	{
		NormalVertex vertices[] =
		{
			{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 1) },
			{ XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
			{ XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 0) },
			{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 1) },

			{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(-1, 0, 0), XMFLOAT2(0, 1) },
			{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(-1, 0, 0), XMFLOAT2(0, 0) },
			{ XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(-1, 0, 0), XMFLOAT2(1, 0) },
			{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(-1, 0, 0), XMFLOAT2(1, 1) },

			{ XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(0, 0, 1), XMFLOAT2(0, 1) },
			{ XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(0, 0, 1), XMFLOAT2(0, 0) },
			{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(0, 0, 1), XMFLOAT2(1, 0) },
			{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(0, 0, 1), XMFLOAT2(1, 1) },

			{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 1) },
			{ XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 0) },
			{ XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 0) },
			{ XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 1) },

			{ XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(0, 1, 0), XMFLOAT2(0, 1) },
			{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(0, 1, 0), XMFLOAT2(0, 0) },
			{ XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(0, 1, 0), XMFLOAT2(1, 0) },
			{ XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(0, 1, 0), XMFLOAT2(1, 1) },

			{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(0, -1, 0), XMFLOAT2(0, 1) },
			{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0, -1, 0), XMFLOAT2(0, 0) },
			{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(0, -1, 0), XMFLOAT2(1, 0) },
			{ XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(0, -1, 0), XMFLOAT2(1, 1) },
		};
		WORD indices[] =
		{
			0, 1, 2, 0, 2, 3,
			4, 5, 6, 4, 6, 7,
			8, 9, 10, 8, 10, 11,
			12, 13, 14, 12, 14, 15,
			16, 17, 18, 16, 18, 19,
			20, 21, 22, 20, 22, 23
		};
		return this->LoadIndexed(game, vertices, sizeof(NormalVertex), ARRAYSIZE(vertices),
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indices, ARRAYSIZE(indices), dynamic);
	}

	bool Mesh::LoadQuad(Game* game, XMFLOAT4 color, bool dynamic)
	{
		ColorVertex vertices[] =
		{
			{ XMFLOAT3(-0.5f, 0, -0.5f), color },
			{ XMFLOAT3(-0.5f, 0, 0.5f), color },
			{ XMFLOAT3(0.5f, 0, 0.5f), color },
			{ XMFLOAT3(0.5, 0, -0.5f), color },
		};
		WORD indices[] =
		{
			0, 1, 2, 2, 3, 0
		};
		return this->LoadIndexed(game, vertices, sizeof(ColorVertex), ARRAYSIZE(vertices),
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indices, ARRAYSIZE(indices), dynamic);
	}

	bool Mesh::LoadQuad(Game* game, bool dynamic)
	{
		NormalVertex vertices[] =
		{
			{ XMFLOAT3(-0.5f, 0, -0.5f), XMFLOAT3(0, 1, 0), XMFLOAT2(0, 0) },
			{ XMFLOAT3(-0.5f, 0, 0.5f), XMFLOAT3(0, 1, 0), XMFLOAT2(0, 1) },
			{ XMFLOAT3(0.5f, 0, 0.5f), XMFLOAT3(0, 1, 0), XMFLOAT2(1, 1) },
			{ XMFLOAT3(0.5, 0, -0.5f), XMFLOAT3(0, 1, 0), XMFLOAT2(1, 0) },
		};
		WORD indices[] =
		{
			0, 1, 2, 2, 3, 0
		};
		return this->LoadIndexed(game, vertices, sizeof(NormalVertex), ARRAYSIZE(vertices),
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indices, ARRAYSIZE(indices), dynamic);
	}

	bool Mesh::CreateInstanceBuffer(UINT vertexSize, UINT capacity)
	{
		if (!_device)
			return false;

		_instanceSize = vertexSize;
		//Create instance buffer
		D3D11_BUFFER_DESC instanceBufferDesc;
		ZeroMemory(&instanceBufferDesc, sizeof(instanceBufferDesc));
		instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		instanceBufferDesc.ByteWidth = capacity * vertexSize;
		void* instances = malloc(capacity * vertexSize);
		ZeroMemory(instances, capacity * vertexSize);
		D3D11_SUBRESOURCE_DATA instanceData;
		instanceData.pSysMem = instances;
		instanceData.SysMemPitch = 0;
		instanceData.SysMemSlicePitch = 0;
		HRESULT result = _device->CreateBuffer(&instanceBufferDesc, &instanceData, &_instanceBuffer);
		if (FAILED(result))
			return false;
		free(instances);
		instances = NULL;

		return true;
	}

	void Mesh::Release()
	{
		if (_instanceBuffer) _instanceBuffer->Release();
		if (_indexBuffer) _indexBuffer->Release();
		if (_vertexBuffer) _vertexBuffer->Release();
		_vertexBuffer = NULL;
		_indexBuffer = NULL;
		_instanceBuffer = NULL;
	}

	void Mesh::Draw()
	{
		if (!_game)
			return;

		unsigned int stride = _vertexSize;
		unsigned int offset = 0;
		_context->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
		_context->IASetPrimitiveTopology(_topology);
		if (_indexBuffer)
		{
			_context->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
			_context->DrawIndexed(_totalIndices, 0, 0);
		}
		else
		{
			_context->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
			_context->Draw(_totalVertices, 0);
		}
		_game->AddDrawCallCount();
	}

	void Mesh::DrawInstanced(UINT totalInstances)
	{
		if (!_context || !_game)
			return;

		unsigned int stride[2] = { _vertexSize, _instanceSize };
		unsigned int offset[2] = { 0, 0 };
		ID3D11Buffer* bufferPoints[2] = { _vertexBuffer, _instanceBuffer };
		_context->IASetVertexBuffers(0, 2, bufferPoints, stride, offset);
		_context->IASetPrimitiveTopology(_topology);
		if (_indexBuffer)
		{
			_context->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
			_context->DrawIndexedInstanced(_totalIndices, totalInstances, 0, 0, 0);
		}
		else
		{
			_context->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
			_context->DrawInstanced(_totalVertices, totalInstances, 0, 0);
		}
		_game->AddDrawCallCount();
	}

	bool Mesh::MapBuffer(ID3D11Buffer* buffer, void** resourceData)
	{
		if (!_context)
			return false;

		D3D11_MAPPED_SUBRESOURCE mapResource;
		HRESULT d3dResult = _context->Map(buffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mapResource);
		if (FAILED(d3dResult))
			return false;
		*resourceData = mapResource.pData;

		return true;
	}
	void Mesh::UnMapBuffer(ID3D11Buffer* buffer)
	{
		if (!_context)
			return;

		_context->Unmap(buffer, 0);
	}

	bool Mesh::MapVertices(void** resourceData)
	{
		if (!_dynamic)
			return false;

		return this->MapBuffer(_vertexBuffer, resourceData);
	}

	void Mesh::UnMapVertices()
	{
		if (!_dynamic)
			return;

		this->UnMapBuffer(_vertexBuffer);
	}

	bool Mesh::MapInstances(void** resourceData)
	{
		return this->MapBuffer(_instanceBuffer, resourceData);
	}

	void Mesh::UnMapInstances()
	{
		this->UnMapBuffer(_instanceBuffer);
	}
}