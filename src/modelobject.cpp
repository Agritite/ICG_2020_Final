#pragma once
#include "modelobject.h"
#include <glm/gtc/matrix_transform.hpp>

modelobject::modelobject(
	const cameraobject* const _pcamera,
	const modelbuffer* const _pmodel,
	const glm::vec3& _pos) noexcept :
	pcamera(_pcamera),
	pmodel(_pmodel),
	position(_pos)
{

}

cameraobject::cameraobject(
	const glm::vec3& _pos, const glm::vec3& _front, const glm::vec3& _up,
	const float _pov_angle, const float _aspect) noexcept :
	position(_pos),
	front(glm::normalize(_front)),
	up(_up),
	pov(_pov_angle),
	aspect(_aspect),
	projectionmatrix(glm::perspective(glm::radians(pov), aspect, 0.1f, 1000.0f))
{
	
}
