#ifndef _MYDEMO_H
#define _MYDEMO_H
#include "DirectX11Game.h"
#include "Camera.h"
#include "SpriteFont.h"
#include "Effect.h"
#include "Texture2D.h"

class MyDemo : public DirectX11Game
{
public:
	MyDemo();
	virtual ~MyDemo();
	virtual void Update(float dt);
protected:
	virtual bool LoadContent();
	virtual void UnloadContent();
	virtual void Draw(float dt);
private:
	Texture2D							_texture;
	Texture2D							_renderTexture;
	Sprite								_sprite;
	SpriteFont							_spriteFont;
	Effect								_effect;
	Mesh								_mesh;
	Camera								_camera;
};
#endif
