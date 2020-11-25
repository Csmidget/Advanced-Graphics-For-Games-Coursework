#include "HeightMap.h"
#include <iostream>

HeightMap::HeightMap(const std::string& name) {
	int iWidth, iHeight, iChans;

	unsigned char* data = SOIL_load_image(name.c_str(), &iWidth, &iHeight, &iChans, 1);

	if (!data) {
		std::cout << "Heightmap can't load file!\n";
		return;
	}

	

	numVertices = iWidth * iHeight;
	numIndices = (iWidth - 1) * (iHeight - 1) * 6;
	
	vertices = new Vector3[numVertices];
	colours = new Vector4[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];

	Vector3 vertexScale = Vector3(16.0f, 1.0f, 16.0f);
	Vector2 textureScale = Vector2(1 / 16.0f, 1 / 16.0f);

	size.x = iWidth;
	size.y = 0;
	size.z = iHeight;

	for (int z = 0; z < iHeight; ++z) {
		for (int x = 0; x < iWidth; ++x) {
			int offset = (z * iWidth) + x;
			size.y = std::max(size.y, (float)data[offset]);
			vertices[offset] = Vector3(x, data[offset], z) * vertexScale;
			colours[offset] = Vector4((float)data[offset] / 255.0f, (float)data[offset] / 255.0f, (float)data[offset] / 255.0f, 1.0f);
			textureCoords[offset] = Vector2(x, z) * textureScale;
		}
	}

	size = size * vertexScale;
	SOIL_free_image_data(data);

	int i = 0;

	for (int z = 0; z < iHeight - 1; ++z) {
		for (int x = 0; x < iWidth - 1; ++x) {
			int a = (z * (iWidth)) + x;
			int b = (z * (iWidth)) + (x + 1);
			int c = ((z + 1) * (iWidth)) + (x + 1);
			int d = ((z + 1) * (iWidth)) + x;

			indices[i++] = a;
			indices[i++] = c;
			indices[i++] = b;
			indices[i++] = c;
			indices[i++] = a;
			indices[i++] = d;
		}
	}

	GenerateNormals();
	GenerateTangents();
	BufferData();
}