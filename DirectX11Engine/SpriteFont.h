#ifndef _SPRITE_FONT_H
#define _SPRITE_FONT_H
#include <memory>
#include <unordered_map>
#include <vector>
#include "Sprite.h"
#include "Texture2D.h"
#include "rapidxml.hpp"

static const int MaxFontInstance = 1000;

struct FontCharInfo
{
	int x, y, width, height;
};

typedef std::pair<int, int>												TEXT_SIZE;
typedef std::shared_ptr<char>										SHARED_CHAR_ARRAY;
typedef rapidxml::xml_document<>								XML_DOCUMENT;
typedef rapidxml::xml_node<>										XML_NODE;
typedef std::unordered_map<int, FontCharInfo>			CHARINFO_MAP;
typedef std::vector<WCHAR>										WCHAR_VECTOR;

class SpriteFont
{
public:
	SpriteFont();
	~SpriteFont();
	bool Load(Game* game, char* filePath, float fontSize);
	void Release();
	void SetSize(float size) { _size = size; }
	void BeginDraw();
	void Draw(const WCHAR* text, FLOAT x, FLOAT y);
	TEXT_SIZE GetTextSize(const char* text);
	void EndDraw();
private:
	DirectX11Game*						_game;
	Texture2D									_texture;
	Sprite										_sprite;
	SHARED_CHAR_ARRAY				_buffer;
	XML_DOCUMENT						_dom;
	XML_NODE*								_info;
	CHARINFO_MAP						_charMap;
	float											_originSize;
	float											_size;
	int											_listLen;
	bool											_began;

	SpriteFont(const SpriteFont& rhs);
	SpriteFont& operator=(const SpriteFont& rhs);
};
#endif