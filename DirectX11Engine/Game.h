#ifndef _GAME_H
#define _GAME_H
#include "Color.h"

class Game
{
public:
	virtual bool Init() = 0;
	virtual void Shutdown() = 0;
	virtual void Update(float dt) = 0;
	virtual void Render(float dt) = 0;
	UINT GetViewportWidth() { return _viewportWidth; }
	UINT GetViewportHeight() { return _viewportHeight; }
	void AddDrawCallCount() { _drawCallCount++; }
	void ResetDrawCallCount() { _drawCallCount = 0; }
	int GetDrawCallCount() { return _drawCallCount; }
protected:
	UINT				_viewportWidth;
	UINT				_viewportHeight;

	virtual bool LoadContent() = 0;
	virtual void UnloadContent() = 0;
	virtual void ClearScreen(Color color) = 0;
	virtual void Draw(float dt) = 0;
private:
	static int		_drawCallCount;
};
#endif