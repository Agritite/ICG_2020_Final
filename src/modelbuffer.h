#pragma once
#ifndef _modelbuffer_h
#define _modelbuffer_h

#include <glad/glad.h>
#include <vector>

class modelbuffer
{
public:
	modelbuffer(const char* const, const GLuint);
	modelbuffer(modelbuffer&&) noexcept;
	modelbuffer(const modelbuffer&) = delete;
	~modelbuffer();
	inline GLuint GetVAO() const { return vao; }
	inline GLuint GetEBO() const { return ebo; }
	inline size_t IndexCount() const { return indices.size(); }

private:
	GLuint shader;
	GLuint vao;
	GLuint vbo;
	GLuint ebo;

	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
};

#endif
