#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <bullet/btBulletDynamicsCommon.h>

#include <iostream>
#include <vector>
#include <cmath>

#include "modelbuffer.h"
#include "modelobject.h"
#include "shader_text.h"

constexpr char windowtitle[] = "GLFW";

constexpr unsigned int scrnx = 800;
constexpr unsigned int scrny = 600;

constexpr float PI = 3.141592653f;