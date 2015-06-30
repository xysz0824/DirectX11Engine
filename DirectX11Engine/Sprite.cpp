#include "Sprite.h"
#include "Vertex.h"
#include "DirectX11Game.h"
#include "Texture2D.h"

Sprite::Sprite()
:	_game(NULL),
	_device(NULL),
	_context(NULL),
	_mwvpConstBuffer(NULL)
{
	_position.x = 0.0f;
	_position.y = 0.0f;
	_origin.x = 0.0f;
	_origin.y = 0.0f;
	_rotation = 0.0f;
	_scale.x = 1.0f;
	_scale.y = 1.0f;
}

Sprite::~Sprite()
{
	Release();
}

bool Sprite::Init(Game* game, Texture2D* texture)
{
	_game = dynamic_cast<DirectX11Game*>(game);
	if (!_game)
		return false;
	//Get device and context.
	_device = _game->GetDevice();
	_context = _game->GetDeviceContext();

	if (!texture)
		return false;

	_texture = texture;
	//Create const buffer.
	D3D11_BUFFER_DESC constDesc;
	ZeroMemory(&constDesc, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.ByteWidth = sizeof(XMMATRIX);
	constDesc.Usage = D3D11_USAGE_DEFAULT;
	HRESULT d3dResult = _device->CreateBuffer(&constDesc, NULL, &_mwvpConstBuffer);
	if (FAILED(d3dResult))
		return false;

	//Create vertex mesh.
	float halfWidth = _texture->GetWidth() / 2.0f;
	float halfHeight = _texture->GetHeight() / 2.0f;
	SpriteVertex vertices[] =
	{
		{ XMFLOAT3(-halfWidth, -halfHeight, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(halfWidth, -halfHeight, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-halfWidth, halfHeight, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(halfWidth, halfHeight, 1.0f), XMFLOAT2(1.0f, 1.0f) },
	};
	bool result = _mesh.Load(_game, vertices, sizeof(SpriteVertex), ARRAYSIZE(vertices),
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, true);
	if (!result)
		return false;

	return true;
}

void Sprite::Release()
{
	if (_mwvpConstBuffer) _mwvpConstBuffer->Release();
	_mwvpConstBuffer = NULL;
}

bool Sprite::SetRect(int x, int y, int width, int height)
{
	auto texWidth = _texture->GetWidth();
	auto texHeight = _texture->GetHeight();
	//_rect.left = x;
	//_rect.right = x + width;
	//_rect.top = y;
	//_rect.bottom = y + height;

	//Map vertex buffer.
	void* pData = NULL;
	bool result = _mesh.MapVertices(&pData);
	if (!result)
		return false;

	// Point to our vertex buffer's internal data.
	SpriteVertex *verticsPtr = (SpriteVertex*)pData;

	//Update texcoord.
	float scaleX = (float)x / texWidth;
	float scaleY = (float)y / texHeight;
	float scaleW = (float)width / texWidth;
	float scaleH = (float)height / texHeight;
	verticsPtr[0].texcoord = XMFLOAT2(scaleX, scaleY);
	verticsPtr[1].texcoord = XMFLOAT2(scaleX + scaleW, scaleY);
	verticsPtr[2].texcoord = XMFLOAT2(scaleX, scaleY + scaleH);
	verticsPtr[3].texcoord = XMFLOAT2(scaleX + scaleW, scaleY + scaleH);

	//Must unmap to cut off the access of GPU cache.
	_mesh.UnMapVertices();

	return true;
}

void Sprite::UpdateWorldViewProjectionMatrix(bool worldImpact)
{
	if (!_context)
		return;

	XMMATRIX mworld = !worldImpact ? XMMatrixIdentity(): GetWorldMatrix();
	XMMATRIX mwvp = XMMatrixMultiply(mworld, GetViewProjectionMatrix());
	mwvp = XMMatrixTranspose(mwvp);
	_context->UpdateSubresource(_mwvpConstBuffer, 0, NULL, &mwvp, 0, 0);
	_context->VSSetConstantBuffers(0, 1, &_mwvpConstBuffer);
}

XMMATRIX Sprite::GetWorldMatrix()
{
	auto width = _texture->GetWidth();
	auto height = _texture->GetHeight();
	XMMATRIX originTranslation = XMMatrixTranslation((0.5f - _origin.x) * width, (0.5f - _origin.y) * height, 0);
	XMMATRIX rotationZ = XMMatrixRotationZ(_rotation);
	XMMATRIX scale = XMMatrixScaling(_scale.x, _scale.y, 1.0f);
	XMMATRIX translation = XMMatrixTranslation(_position.x, _position.y, 0);
	XMMATRIX mworld;
	return originTranslation * rotationZ * scale * translation;
}

XMMATRIX Sprite::GetViewProjectionMatrix()
{
	XMMATRIX view = XMMatrixIdentity();
	XMMATRIX projection = XMMatrixOrthographicOffCenterLH(
		0, (float)_game->GetViewportWidth(), (float)_game->GetViewportHeight(), 0, 0.1f, 100.0f);
	return XMMatrixMultiply(view, projection);
}

void Sprite::Draw(bool useSolidShader)
{
	if (!_game)
		return;

	//update solid shader
	if (useSolidShader)
		_game->UpdateSolidShader();

	//Set texture context.
	_texture->UpdateColorMapAndSampler();

	//Calculate world-view-projection matrix and update to buffer
	UpdateWorldViewProjectionMatrix();

	//Draw texture mesh
	_mesh.Draw();
}

void Sprite::DrawInstanced(UINT totalInstances, bool useSolidShader)
{
	if (!_game)
		return;
	
	//update solid shader
	if (useSolidShader)
		_game->UpdateSolidShader();

	//Set texture context.
	_texture->UpdateColorMapAndSampler();

	//Calculate world-view-projection matrix and update to buffer.
	UpdateWorldViewProjectionMatrix(false);

	//Draw instanced texture mesh
	_mesh.DrawInstanced(totalInstances);
}

bool Sprite::CreateInstanceBuffer(UINT vertexSize, UINT capacity)
{
	return _mesh.CreateInstanceBuffer(vertexSize, capacity);
}

bool Sprite::MapInstances(void** resourceData)
{
	return _mesh.MapInstances(resourceData);
}

void Sprite::UnMapInstances()
{
	_mesh.UnMapInstances();
}