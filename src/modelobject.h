#pragma once
#ifndef _modelobject_h
#define _modelobject_h

#include "modelbuffer.h"
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

class cameraobject
{
public:
	cameraobject(const glm::vec3&, const glm::vec3&, const glm::vec3& , const float, const float) noexcept;
	inline void SetFront(const glm::vec3& _front) { front = glm::normalize(_front); }
	inline void ModFront(const glm::vec3& _front) { front = glm::normalize(front + _front); }
	inline void SetPos(const glm::vec3& _pos) { position = _pos; }
	inline void ModPos(const glm::vec3& _pos) { position += _pos; }
	inline const glm::vec3& GetPos() const { return position; }
	inline const glm::vec3& GetFront() const { return front; }
	inline const glm::vec3& GetUp() const { return up; }
	inline glm::mat4 GetProjectionMatrix() const { return projectionmatrix; }
	inline glm::mat4 GetViewMatrix() const { return glm::lookAt(position, position + front, up); }

private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	float pov;
	float aspect;
	glm::mat4 projectionmatrix;
};

class modelobject
{
public:
	modelobject(const cameraobject* const, const modelbuffer* const, const glm::vec3&) noexcept;
	inline const modelbuffer* const GetModel() const { return pmodel; }
	inline const cameraobject* const GetCamera() const { return pcamera; }
	inline const glm::vec3& GetPos() const { return position; }
	inline void SetPos(const glm::vec3 _pos) { position = _pos; }
	inline void Translate(const glm::vec3 delta) { position += delta; }
	inline glm::mat4 GetModelMatrix() const { return glm::translate(identity, position); }

private:
	static constexpr glm::mat4 identity = glm::mat4(1.0f);
	const cameraobject* pcamera;
	const modelbuffer* pmodel;
	glm::vec3 position;
};



#endif
