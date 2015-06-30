#include <vector>
#include <deque>
#include <string>
#include <fstream>
#include <memory>
#include "DirectX11Game.h"
#include "Effect.h"

static bool MethodParse(char* buffer, unsigned int start, unsigned int end, const char* name, std::vector<std::string>& params);
static int IgnoreChar(char* buffer, unsigned int start, unsigned int end, const char ignore);
static int KmpSearch(const char* s, unsigned int start, unsigned int end, const char* p);
static int KmpSearchCount(const char *s, unsigned int start, unsigned int end, const char* p);

bool EffectParser::Parse(char* buffer, EFFECT_DESC** effectDesc)
{
	unsigned int length = strlen(buffer);
	*effectDesc = new EFFECT_DESC;

	//Parse techinques.
	int totalTechniques = KmpSearchCount(buffer, 0, length, "technique");
	if (totalTechniques == 0)
		return false;
	(*effectDesc)->TechniqueCount = totalTechniques;
	(*effectDesc)->Techniques.resize(totalTechniques);
		int pos = -1;
	for (int i = 0; i < totalTechniques; ++i)
	{
		pos = KmpSearch(buffer, pos + 1, length, "technique");
		TechniqueParse(buffer, pos, length, (*effectDesc)->Techniques[i]);
	}
	return true;
}

void EffectParser::TechniqueParse(char* buffer, unsigned int start, unsigned int end, TECHNIQUE_DESC& techniqueDesc)
{
	int pos = KmpSearch(buffer, start, end, " ");
	if (pos == 0)
		return;
	pos = IgnoreChar(buffer, pos, end, ' ');

	//Find technique name.
	std::string name = "";
	int i = pos;
	while (i < (int)strlen(buffer))
	{
		if (buffer[i] == ' ' || buffer[i] == '\n')
			break;
		if (isalpha(buffer[i]) != 0 || (i != pos && isdigit(buffer[i])) || buffer[i] == '_')
			name += buffer[i];
		i++;
	}
	if (name.empty())
		return;
	techniqueDesc.TechniqueName = name.c_str();

	//Parse passes.
	int startPos = KmpSearch(buffer, pos, end, "{") + 1;
	if (startPos == -1)
		return;
	int endPos = KmpSearch(buffer, startPos, end, "technique");
	if (endPos == -1)
		endPos = strlen(buffer);
	int totalPasses = KmpSearchCount(buffer, startPos, endPos, "pass");
	techniqueDesc.PassCount = totalPasses;
	techniqueDesc.Passes.resize(totalPasses);
	pos = startPos;
	for (int i = 0; i < totalPasses; ++i)
	{
		techniqueDesc.Passes[i].PassIndex = i;
		pos = KmpSearch(buffer, pos + 1, endPos, "pass");
		PassParse(buffer, pos, endPos, techniqueDesc.Passes[i]);
	}
}

void EffectParser::PassParse(char* buffer, unsigned int start, unsigned int end, PASS_DESC& passDesc)
{
	//Find vertex shader.
	int pos = KmpSearch(buffer, start, end, "SetVertexShader");
	if (pos == -1)
		return;
	std::vector<std::string> vsParams;
	bool result = MethodParse(buffer, pos + 1, end, "CompileShader", vsParams);
	if (!result)
		return;
	passDesc.VertexShaderVersion = vsParams[0];
	passDesc.VertexShaderEntry = vsParams[1];

	//Find vertex shader.
	pos = KmpSearch(buffer, pos, end, "SetPixelShader");
	if (pos == -1)
		return;
	std::vector<std::string> psParams;
	result = MethodParse(buffer, pos + 1, end, "CompileShader", psParams);
	if (!result)
		return;
	passDesc.PixelShaderVersion = psParams[0];
	passDesc.PixelShaderEntry = psParams[1];
}

static bool MethodParse(char* buffer, unsigned int start, unsigned int end, const char* name,std::vector<std::string>& params)
{
	//Check method name.
	int pos = KmpSearch(buffer, start, end, name);
	if (pos == -1)
		return false;
	pos = KmpSearch(buffer, pos + 1, end, "(");

	//Parse method parameters.
	std::deque<char> brackets;
	std::string param;
	for (int i = pos; i < (int)end; ++i)
	{
		if (buffer[i] == '(')
		{
			brackets.push_back(buffer[i]);
		}
		else if (buffer[i] == ')')
		{
			if (brackets.empty())
				return false;
			char topChar = brackets.back();
			brackets.pop_back();
			if (topChar != '(')
				return false;
			if (brackets.empty())
				break;
		}
		else if (buffer[i] == ',')
		{
			params.push_back(param);
			param.clear();
		}
		else if (isalpha(buffer[i]) != 0 || (i != pos && isdigit(buffer[i])) || buffer[i] == '_')
		{
			param += buffer[i];
		}
		else if (buffer[i] != ' ')
			return false;
	}
	if (!param.empty())
		params.push_back(param);
	return true;
}

static int IgnoreChar(char* buffer, unsigned int start, unsigned int end, const char ignore)
{
	unsigned i = start;
	for (; i < end; ++i)
		if (buffer[i] != ignore)
			break;
	return i;
}

static int KmpSearch(const char* s, unsigned int start, unsigned int end, const char* p)
{
	//KMP use two cursor to go through source string and pattern string 
	//that can avoid backward checking.
	int i = start, j = 0;
	int sLen = end;
	int pLen = strlen(p);
	
	//Build next array, that can determine the index of element
	//which the cursor will go to where comparison failed.
	int next[30];
	next[0] = -1;
	next[1] = 0;
 	for (int j = 2; j < pLen; ++j)
	{
		next[j] = next[j - 1];
		if (p[j - 1] == p[next[j]])
			next[j]++;
	}

	while (i < sLen && j < pLen)
	{
		if (j == -1 || s[i] == p[j])
		{
			i++;
			j++;
		}
		else
			j = next[j];
	}
	if (j == pLen)
		return i - j;
	return -1;
}

static int KmpSearchCount(const char *s, unsigned int start, unsigned int end, const char* p)
{
	int count = 0;
	int pos = start;
	while (pos != -1)
	{
		pos = KmpSearch(s, pos + 1, end, p);
		count++;
	}
	return count;
}

Effect::Effect()
:	_device(NULL),
	_context(NULL),
	_effectDesc(NULL)
{
}

Effect::~Effect()
{
	Release();
}

bool Effect::Load(Game* game, char* filePath, D3D11_INPUT_ELEMENT_DESC* elementDesc, int totalElements)
{
	auto dx11 = dynamic_cast<DirectX11Game*>(game);
	if (!dx11)
		return false;
	//Get device and context.
	_device = dx11->GetDevice();
	_context = dx11->GetDeviceContext();

	//Read stream.
	std::ifstream fileStream;
	fileStream.open(filePath, std::ifstream::in);
	if (!fileStream.is_open())
		return false;
	fileStream.seekg(0, std::ios::end);
	std::streamoff fileSize = fileStream.tellg();
	fileStream.seekg(0, std::ios::beg);
	if (fileSize <= 0)
		return false;
	std::unique_ptr<char> buffer(new char[(UINT)fileSize]);
	memset(buffer.get(), '\0', (UINT)fileSize);
	fileStream.read(buffer.get(), (UINT)fileSize);
	fileStream.close();

	//Parse effect.
	EFFECT_DESC* effectDesc = NULL;
	bool result = EffectParser::Parse(buffer.get(), &effectDesc);
	if (!result)
		return false;

	//load shader.
	_effectDesc = effectDesc;
	for (UINT i = 0; i < effectDesc->TechniqueCount; ++i)
	{
		for (UINT j = 0; j < effectDesc->Techniques[i].PassCount; ++j)
		{
			char* vsEntry = const_cast<char*>(effectDesc->Techniques[i].Passes[j].VertexShaderEntry.c_str());
			char* vsVersion = const_cast<char*>(effectDesc->Techniques[i].Passes[j].VertexShaderVersion.c_str());
			char* psEntry = const_cast<char*>(effectDesc->Techniques[i].Passes[j].PixelShaderEntry.c_str());
			char* psVersion = const_cast<char*>(effectDesc->Techniques[i].Passes[j].PixelShaderVersion.c_str());
			if (!ExistVertexShader(vsEntry))
			{
				ID3D11VertexShader* vs = NULL;
				ID3D11InputLayout* inputLayout = NULL;
				result = dx11->LoadVertexShader(filePath, vsEntry, vsVersion, elementDesc, totalElements, &vs, &inputLayout);
				if (!result)
					return false;
				SetVertexShader(vsEntry, vs, inputLayout);
			}
			if (!ExistPixelShader(psEntry))
			{
				ID3D11PixelShader* ps = NULL;
				result = dx11->LoadPixelShader(filePath, psEntry, psVersion, &ps);
				if (!result)
					return false;
				SetPixelShader(psEntry, ps);
			}
		}
	}

	return true;
}

void Effect::Release()
{
	if (_effectDesc != NULL)
		delete _effectDesc;

	for (auto& bIt : _constBuffers)
	if (bIt.second != NULL)
		bIt.second->Release();

	for (auto& vsIt : _vertexShaders)
	if (vsIt.second != NULL)
		vsIt.second->Release();

	for (auto& psIt : _pixelShaders)
	if (psIt.second != NULL)
		psIt.second->Release();

	for (auto& ilIt : _inputLayouts)
	if (ilIt.second != NULL)
		ilIt.second->Release();
}

const EFFECT_DESC* Effect::GetDesc()
{
	return _effectDesc;
}

void Effect::SetVertexShader(const char* entry, ID3D11VertexShader* shader, ID3D11InputLayout* inputLayout)
{
	if (ExistVertexShader(entry))
	{
		if (_vertexShaders[entry] != NULL)
			_vertexShaders[entry]->Release();
		_vertexShaders.erase(entry);
		if (_inputLayouts[entry] != NULL)
			_inputLayouts[entry]->Release();
		_inputLayouts.erase(entry);
	}
	_vertexShaders[entry] = shader;
	_inputLayouts[entry] = inputLayout;
}

bool Effect::ExistVertexShader(const char* entry)
{
	for (const auto& it : _vertexShaders)
	{
		if (strcmp(it.first, entry) == 0)
			return true;
	}
	return false;
}

void Effect::SetPixelShader(const char* entry, ID3D11PixelShader* shader)
{
	if (ExistPixelShader(entry))
	{
		if (_pixelShaders[entry] != NULL)
			_pixelShaders[entry]->Release();
		_pixelShaders.erase(entry);
	}
	_pixelShaders[entry] = shader;
}

bool Effect::ExistPixelShader(const char* entry)
{
	for (const auto& it : _pixelShaders)
	{
		if (strcmp(it.first, entry) == 0)
			return true;
	}
	return false;
}

void Effect::UpdateShader(UINT techniqueIndex, UINT passIndex)
{
	auto desc = GetDesc();
	const char* entry = nullptr;
	if (_context == NULL)
		return;
	entry = desc->Techniques[techniqueIndex].Passes[passIndex].VertexShaderEntry.c_str();
	if (ExistVertexShader(entry))
	{
		_context->VSSetShader(_vertexShaders[entry], NULL, 0);
		_context->IASetInputLayout(_inputLayouts[entry]);
	}
	entry = desc->Techniques[techniqueIndex].Passes[passIndex].PixelShaderEntry.c_str();
	if (Effect::ExistPixelShader(entry))
		_context->PSSetShader(_pixelShaders[entry], NULL, 0);
}

bool Effect::CreateConstantBuffer(UINT registerSlot, UINT byteWidth)
{
	if (_device == NULL)
		return false;

	if (ExistConstantBuffer(registerSlot))
	{
		if (_constBuffers[registerSlot] != NULL)
			_constBuffers[registerSlot]->Release();
		_constBuffers.erase(registerSlot);
	}
	_constBuffers[registerSlot] = NULL;

	D3D11_BUFFER_DESC constDesc;
	ZeroMemory(&constDesc, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.ByteWidth = byteWidth;
	constDesc.Usage = D3D11_USAGE_DEFAULT;
	HRESULT result = _device->CreateBuffer(&constDesc, NULL, &_constBuffers[registerSlot]);
	if (FAILED(result))
		return false;

	return true;
}

void Effect::SetConstantBuffer(UINT registerSlot, const void* pData)
{
	if (_context == NULL)
		return;
	_context->UpdateSubresource(_constBuffers[registerSlot], 0, NULL, pData, 0, 0);
	_context->VSSetConstantBuffers(registerSlot, 1, &_constBuffers[registerSlot]);
}

bool Effect::ExistConstantBuffer(UINT registerSlot)
{
	return _constBuffers.count(registerSlot) == 1;
}