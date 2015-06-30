#include <fstream>
#include "DirectX11Game.h"
#include "SpriteFont.h"
#include "Vertex.h"
#include "Texture2D.h"

SpriteFont::SpriteFont()
:	_game(NULL),
	_listLen(0)
{
}

SpriteFont::~SpriteFont()
{
	Release();
}

bool SpriteFont::Load(Game* game, char* filePath, float fontSize)
{
	_game = dynamic_cast<DirectX11Game*>(game);
	if (!_game)
		return false;

	_size = fontSize;
	//Open file and create input stream
	std::ifstream fs(filePath, std::ifstream::in);
	if (!fs.is_open())
		return false;
	//Get file size
	fs.seekg(0, std::ios::end);
	auto size = fs.tellg();
	fs.seekg(0, std::ios::beg);
	//Create buffer for reading whole file
	_buffer = SHARED_CHAR_ARRAY(new char[(UINT)size]);
	memset(_buffer.get(), '\0', (UINT)size);
	fs.read(_buffer.get(), (UINT)size);
	fs.close();
	//Parse and get the xml node
	_dom.parse<0>(_buffer.get());
	_info = _dom.first_node()->first_node();
	sscanf_s(_info->first_attribute("size")->value(), "%f", &_originSize);
	XML_NODE* chars = _info->next_sibling("chars");
	//Build char map
	for (auto node = chars->first_node(); node != NULL; node = node->next_sibling())
	{
		int charId, charX, charY, charWidth, charHeight;
		sscanf_s(node->first_attribute("id")->value(), "%d", &charId);
		sscanf_s(node->first_attribute("x")->value(), "%d", &charX);
		sscanf_s(node->first_attribute("y")->value(), "%d", &charY);
		sscanf_s(node->first_attribute("width")->value(), "%d", &charWidth);
		sscanf_s(node->first_attribute("height")->value(), "%d", &charHeight);
		FontCharInfo info;
		info.x = charX;
		info.y = charY;
		info.width = charWidth;
		info.height = charHeight;
		_charMap.emplace(charId, info);
	}
	//Load texture
	XML_NODE* page = _info->next_sibling("pages")->first_node();
	char* texPath = page->first_attribute("file")->value();
	if (!_texture.Load(game, texPath))
		return false;
	//Init sprite
	if (!_sprite.Init(game, &_texture))
		return false;
	if (!_sprite.CreateInstanceBuffer(sizeof(FontVertex), MaxFontInstance))
		return false;

	return true;
}

void SpriteFont::Release()
{
}

void SpriteFont::Draw(const WCHAR* text, FLOAT x, FLOAT y)
{
	if (!_began)
		return;

	auto width = (float)_texture.GetWidth();
	auto height = (float)_texture.GetHeight();
	auto halfWidth = width * 0.5f;
	auto halfHeight = height * 0.5f;
	auto scale = _size / _originSize;
	//Set the vertex to instance buffer
	UINT len = wcslen(text);
	void *pData = NULL;
	if (!_sprite.MapInstances(&pData))
		return;

	FontVertex *instancesPtr = (FontVertex*)pData;
	for (UINT i = _listLen; i < _listLen + len && i < MaxFontInstance; ++i)
	{
		//Adjust vertex data according to each text
		auto charInfo = _charMap.find(text[i - _listLen]);
		if (charInfo != _charMap.end())
		{
			instancesPtr[i].texcoordScale = XMFLOAT4(
				charInfo->second.x / width, charInfo->second.y / height,
				charInfo->second.width / width, charInfo->second.height / height);
			instancesPtr[i].positionScale = XMFLOAT3(
				instancesPtr[i].texcoordScale.z * scale, instancesPtr[i].texcoordScale.w * scale, 1);
			instancesPtr[i].position = XMFLOAT3(
				halfWidth + x / instancesPtr[i].positionScale.x, halfHeight + y / instancesPtr[i].positionScale.y, 0);
			x += charInfo->second.width * scale;
		}
		else
			x += _size;
	}
	_sprite.UnMapInstances();
	//Increase the length of text list
	_listLen += len;
}

void SpriteFont::BeginDraw()
{
	_listLen = 0;
	_began = true;
}

void SpriteFont::EndDraw()
{
	if (!_game)
		return;

	_game->UpdateFontShader();
	_sprite.DrawInstanced(_listLen, false);
	_began = false;
}

//TEXT_SIZE SpriteFont::GetTextSize(const char* text)
//{
		//TODO
//}
