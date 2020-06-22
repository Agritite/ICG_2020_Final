#pragma once
#include "modelbuffer.h"
#include <happly.h>

modelbuffer::modelbuffer(const char* const path, const GLuint _shader) :
	vao(0),
	vbo(0),
	ebo(0),
	shader(_shader)
{
	{
		happly::PLYData plyin(path);
		std::vector<float> v[6]
		{
			plyin.getElement("vertex").getProperty<GLfloat>("x"),
			plyin.getElement("vertex").getProperty<GLfloat>("y"),
			plyin.getElement("vertex").getProperty<GLfloat>("z"),
			plyin.getElement("vertex").getProperty<GLfloat>("nx"),
			plyin.getElement("vertex").getProperty<GLfloat>("ny"),
			plyin.getElement("vertex").getProperty<GLfloat>("nz")
		};
		for (size_t i = 0; i < v[0].size(); i++)
			for (size_t j = 0; j < 6; j++)
				vertices.push_back(v[j][i]);
		auto f = plyin.getFaceIndices<GLuint>();
		for (auto vec : f)
			for (auto i : vec)
				indices.push_back(i);
	}

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

modelbuffer::modelbuffer(modelbuffer&& __r) noexcept :
	vao(__r.vao),
	vbo(__r.vbo),
	ebo(__r.ebo),
	shader(__r.shader),
	vertices(std::move(__r.vertices)),
	indices(std::move(__r.indices))
{
	__r.vao = 0;
	__r.vbo = 0;
	__r.ebo = 0;
	__r.shader = 0;
}

modelbuffer::~modelbuffer()
{
	if(vao)
		glDeleteVertexArrays(1, &vao);
	if(vbo)
		glDeleteBuffers(1, &vbo);
	if(ebo)
		glDeleteBuffers(1, &ebo);
}
