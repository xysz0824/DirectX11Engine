#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
using namespace std;

char head[4] = { 102, 77, 62, 97};

union Byte_Type
{
	char b[4];
	float f;
	int i;
};

struct Vector3
{
	Byte_Type x, y, z;
};

struct Vertex
{
	Vector3 v, vn, vt;
	Byte_Type index;
};

struct Object
{
	Vertex		*vertices;
	Byte_Type	*index;
};

int main(int argc, char* argv[])
{
	cout << "OBJ Converter 1.0" << endl;
	cout << "Copyright (c) 2015 StarX. All rights reserved." << endl << endl;;
	if (argc == 1)
	{
		cout << "You need to input a .obj file path behind it." << endl;
		return 0;
	}
	float scale;
	if (argc == 3)
	{
		sscanf(argv[2], "%f", &scale);
	}
	FILE* file = fopen(argv[1], "r");
	if (!file)
	{
		cout << "Error opening the file!" << endl;
		return 0;
	}
	cout << "Loading data......" << endl;
	char temp[50];
	vector<Vector3>								vertices;
	vertices.reserve(50000);
	vector<Vector3>								normals;
	normals.reserve(100000);
	vector<Vector3>								texcoords;
	texcoords.reserve(50000);
	unordered_map<string, Vertex>		faces;
	vector<int>										index;
	index.reserve(200000);
	int newIndex = 1;
	while (fscanf(file, "%s", temp) != EOF)
	{
		if (strcmp(temp, "v") == 0)
		{
			Vector3 v;
			if (fscanf(file, "%f%f%f", &v.x.f, &v.y.f, &v.z.f) != 3)
			{
				cout << "Error loading data. This program will be suspended." << endl;
				return 0;
			}
			if (argc == 3)
			{
				v.x.f *= scale;
				v.y.f *= scale;
				v.z.f *= scale;
			}
			vertices.push_back(v);
		}
		else if (strcmp(temp, "vn") == 0)
		{
			Vector3 vn;
			if (fscanf(file, "%f%f%f", &vn.x.f, &vn.y.f, &vn.z.f) != 3)
			{
				cout << "Error loading data. This program will be suspended." << endl;
				return 0;
			}
			normals.push_back(vn);
		}
		else if (strcmp(temp, "vt") == 0)
		{
			Vector3 vt;
			if (fscanf(file, "%f%f%f", &vt.x.f, &vt.y.f, &vt.z.f) != 3)
			{
				cout << "Error loading data. This program will be suspended." << endl;
				return 0;
			}
			texcoords.push_back(vt);
		}
		else if (strcmp(temp, "f") == 0)
		{
			string key;
			Vertex v1, v2, v3;
			int vi = 0, vti = 0, vni = 0;
			fscanf(file, "%s", temp);
			key = string(temp);
			if (faces.find(key) == faces.end() && sscanf(temp, "%d/%d/%d", &vi, &vti, &vni) == 3)
			{
				v1.v = vertices[vi - 1];
				v1.vt = texcoords[vti - 1];
				v1.vn = normals[vni - 1];
				v1.index.i = newIndex++;
				faces.emplace(key, v1);
			}
			index.push_back(faces[key].index.i);
			fscanf(file, "%s", temp);
			key = string(temp);
			if (faces.find(key) == faces.end() && sscanf(temp, "%d/%d/%d", &vi, &vti, &vni) == 3)
			{
				sscanf(temp, "%d/%d/%d", &vi, &vti, &vni);
				v2.v = vertices[vi - 1];
				v2.vt = texcoords[vti - 1];
				v2.vn = normals[vni - 1];
				v2.index.i = newIndex++;
				faces.emplace(key, v2);
			}
			index.push_back(faces[key].index.i);
			fscanf(file, "%s", temp);
			key = string(temp);
			if (faces.find(key) == faces.end() && sscanf(temp, "%d/%d/%d", &vi, &vti, &vni) == 3)
			{
				sscanf(temp, "%d/%d/%d", &vi, &vti, &vni);
				v3.v = vertices[vi - 1];
				v3.vt = texcoords[vti - 1];
				v3.vn = normals[vni - 1];
				v3.index.i = newIndex++;
				faces.emplace(key, v3);
			}
			index.push_back(faces[key].index.i);
		}
	}
	if (normals.empty())
	{
		cout << "The file contains no normal. This program will be suspended." << endl;
		return 0;
	}
	if (texcoords.empty())
	{
		cout << "The file contains no texcoord. This program will be suspended." << endl;
		return 0;
	}
	Object object;
	object.vertices = new Vertex[faces.size()];
	for (auto& item : faces)
	{
		object.vertices[item.second.index.i - 1].v = item.second.v;
		object.vertices[item.second.index.i - 1].vn = item.second.vn;
		object.vertices[item.second.index.i - 1].vt = item.second.vt;
	}
	object.index = new Byte_Type[index.size()];
	for (size_t i = 0; i < index.size(); ++i)
	{
		object.index[i].i = index[i];
	}
	fclose(file);
	cout << "Load complete." << endl;
	cout << "Converting......" << endl;
	char *fileName = strtok(argv[1], ".");
	ofstream fs(strcat(fileName, ".sm"), ios::binary);
	fs.write(head, 4);
	Byte_Type vertexCount;
	vertexCount.i = (int)faces.size();
	fs.write(vertexCount.b, 4);
	for (int i = 0; i < vertexCount.i; ++i)
	{
		fs.write(object.vertices[i].v.x.b, 4);
		fs.write(object.vertices[i].v.y.b, 4);
		fs.write(object.vertices[i].v.z.b, 4);
		fs.write(object.vertices[i].vn.x.b, 4);
		fs.write(object.vertices[i].vn.y.b, 4);
		fs.write(object.vertices[i].vn.z.b, 4);
		fs.write(object.vertices[i].vt.x.b, 4);
		fs.write(object.vertices[i].vt.y.b, 4);
		fs.write(object.vertices[i].vt.z.b, 4);
	}
	Byte_Type indexCount;
	indexCount.i = (int)index.size();
	fs.write(indexCount.b, 4);
	for (int i = 0; i < indexCount.i; ++i)
	{
		fs.write(object.index[i].b, 4);
	}
	cout << "Convertion complete." << endl;
	return 0;
}