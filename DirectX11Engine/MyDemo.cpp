#include "MyDemo.h"
#include "SpriteFont.h"
#include "State.h"

MyDemo::MyDemo()
{
}

MyDemo::~MyDemo()
{
}

bool MyDemo::LoadContent()
{
	this->SetProfileDisplay(true);
	//Load texture and init sprite.
	_texture.Load(this, "Wood.dds");
	_renderTexture.Create(this, GetViewportWidth(), GetViewportHeight());
	_sprite.Init(this, &_renderTexture);
	_sprite.SetPosition(XMFLOAT2(100, 100));

	//load effect.
	D3D11_INPUT_ELEMENT_DESC inputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	_effect.Load(this, "Model.fx", inputElements, ARRAYSIZE(inputElements));
	//Create constant buffer to shader for mvp.
	_effect.CreateConstantBuffer(0, sizeof(XMMATRIX));
	//_effect.CreateConstantBuffer(1, sizeof(XMFLOAT4));
	
	//Load mesh.
	_mesh.LoadCylinder(this, 100, 100, false);

	//Initialize camera.
	_camera.Init(XMFLOAT3(5, 2, 0), XMFLOAT3(0, 0, 0), GetViewportWidth(), GetViewportHeight(), 0.1f, 100.0f);

	return true;
}

void MyDemo::UnloadContent()
{
	DirectX11Game::UnloadContent();
	_texture.Release();
	_sprite.Release();
	_effect.Release();
	_mesh.Release();
}

static float angle = 0;
static float h = 0;

void MyDemo::Update(float dt)
{
	this->UpdateInputState();
	if (IsKeyDown(DIK_UP)) h++;
	if (IsKeyDown(DIK_DOWN)) h--;
	if (IsKeyDown(DIK_LEFT)) angle--;
	if (IsKeyDown(DIK_RIGHT)) angle++;
	_camera.SetPosition(XMFLOAT3(5 * cos(angle / 180 * XM_PI), 2 * sin(h / 180 * XM_PI), 5 * sin(angle / 180 * XM_PI)));
	this->SaveInputState();
}

void MyDemo::Draw(float dt)
{
	//Clear screen
	Color clearColor = { 0.0f, 0.0f, 0.25f, 1.0f };
	this->ClearScreen(clearColor);
	//SetRenderTarget(&_renderTexture, clearColor);

	//Set shaders
	XMMATRIX mvp = _camera.GetViewProjecitonMatrix();
	_effect.SetConstantBuffer(0, &mvp);
	//XMFLOAT3 viewPos = _camera.GetPosition();
	//_effect.SetConstantBuffer(1, &viewPos);
	_effect.UpdateShader(0, 0);
	//Draw meshes
	_texture.UpdateColorMapAndSampler();
	_mesh.Draw();

	//Draw sprites

	//Draw texts

	//SetRenderTarget(NULL);
	//_sprite.Draw();
}