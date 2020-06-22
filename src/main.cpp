#include "main.h"

void framebuffer_size_callback(GLFWwindow*, int, int);
void mouseinput_callback(GLFWwindow*, double, double);
void processInput(GLFWwindow* const, cameraobject* const, double);

//wrapper for bulletphysics rigid body creation
btRigidBody* CreateRigidBody(btDiscreteDynamicsWorld*, btCollisionShape*,
	float, const btVector3&, const btQuaternion&);

int main()
{
	//glfw initialization
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

	GLFWwindow* window = glfwCreateWindow(scrnx, scrny, windowtitle, nullptr, nullptr);
	if (window == nullptr)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) < 0)
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	//shader program initialization
	GLuint shaderprogram = glCreateProgram();
	{
		//vertex shader
		GLuint vertexshader = glCreateShader(GL_VERTEX_SHADER);
		{
			std::string vertexshader_src(vertexshader_text);
			const GLchar* const pvertexshader_src = vertexshader_src.c_str();
			glShaderSource(vertexshader, 1, &pvertexshader_src, nullptr);
			glCompileShader(vertexshader);
			{
				int success;
				glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &success);
				if (!success)
				{
					char log[512];
					glGetShaderInfoLog(vertexshader, 512, nullptr, log);
					std::cerr << "Vertex Shader Compilation Failed:\n" << log << std::endl;
				}
			}
		}
		//fragment shader
		GLuint fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
		{
			std::string fragmentshader_src(fragmentshader_text);
			const GLchar* const pfragmentshader_src = fragmentshader_src.c_str();
			glShaderSource(fragmentshader, 1, &pfragmentshader_src, nullptr);
			glCompileShader(fragmentshader);
			{
				int success;
				glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &success);
				if (!success)
				{
					char log[512];
					glGetShaderInfoLog(fragmentshader, 512, nullptr, log);
					std::cerr << "Fragment Shader Compilation Failed:\n" << log << std::endl;
				}
			}
		}

		glAttachShader(shaderprogram, vertexshader);
		glAttachShader(shaderprogram, fragmentshader);
		glLinkProgram(shaderprogram);
		{
			int success;
			glGetProgramiv(shaderprogram, GL_LINK_STATUS, &success);
			if (!success)
			{
				char log[512];
				glGetProgramInfoLog(shaderprogram, 512, nullptr, log);
				std::cerr << "Shader Linkage Failed:\n" << log << std::endl;
			}
			glDeleteShader(vertexshader);
			glDeleteShader(fragmentshader);
		}
	}

	//bullet physics initialization
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	//vector for storing Rigid Body Instances
	std::vector<btRigidBody*> Rigidbodies;

	//vector for storing Static Body Instances
	std::vector<btRigidBody*> StaticBodies;

	//vector for storing Collision Shapes to be used by Body Instances
	std::vector<btCollisionShape*> collisionShapes;

	//Sphere Collision Shape
	collisionShapes.push_back(new btSphereShape(btScalar(1.0f)));

	//Box Collision Shape (act as floor and walls)
	collisionShapes.push_back(new btBoxShape(btVector3(100.0f, 1.25f, 100.0f)));

	//Camera Instance
	cameraobject camera(glm::vec3(0.0f, 0.0f, 50.0f), glm::vec3(0.0f, 0.0f, -50.0f), glm::vec3(0.0f, 0.1f, 0.0f),
		45.0f, (float)scrnx / (float)scrny);

	//pass Camera Instance to Mouse Callback Function
	glfwSetWindowUserPointer(window, &camera);

	//bind Mouse Callback
	glfwSetCursorPosCallback(window, mouseinput_callback);

	//vector for storing model shape buffers for OpenGL (not model instances)
	std::vector<modelbuffer> models;

	//ball mesh
	models.push_back(std::move(modelbuffer("./ball.ply", shaderprogram)));

	//box mesh
	models.push_back(std::move(modelbuffer("./box.ply", shaderprogram)));

	//Set Bulletphysics Gravity
	dynamicsWorld->setGravity(btVector3(10, -90, 10));

	//vector for storing model instances for OpenGL (can have multiple instances for same shape, but only 1 buffer should exist per shape)
	std::vector<modelobject> objects;
	{
		//Creating Balls
		constexpr float _mass = 50.0f;
		const btQuaternion QuatIdentity(0.0f, 0.0f, 0.0f);
		float _y = 20.0f;
		for (float _x = -20.0f; _x < 20.0f; _x += 5.0f)
		{
			for (float _z = -20.0f; _z < 20.0f; _z += 5.0f)
			{
				//Model Instances for OpenGL
				objects.push_back(modelobject(&camera, models.data(), glm::vec3(_x, _y, _z)));

				//Collision Body for BulletPhysics
				Rigidbodies.push_back(CreateRigidBody(dynamicsWorld, collisionShapes[0], _mass, btVector3(_x, _y, _z), QuatIdentity));
			}
		}

		//Creating Floor and Walls
		objects.push_back(modelobject(&camera, models.data() + 1, glm::vec3(0.0f, 0.0f, 0.0f)));
		StaticBodies.push_back(CreateRigidBody(dynamicsWorld, collisionShapes[1], 0.0f, btVector3(0.0f, -23.75f, 0.0f),
			QuatIdentity));
		StaticBodies.push_back(CreateRigidBody(dynamicsWorld, collisionShapes[1], 0.0f, btVector3(-23.75f, 0.0f, 0.0f),
			btQuaternion(btVector3(0.0f, 0.0f, 1.0f), glm::radians(90.0f))));
		StaticBodies.push_back(CreateRigidBody(dynamicsWorld, collisionShapes[1], 0.0f, btVector3(0.0f, 23.75f, 0.0f),
			btQuaternion(btVector3(0.0f, 0.0f, 1.0f), glm::radians(180.0f))));
		StaticBodies.push_back(CreateRigidBody(dynamicsWorld, collisionShapes[1], 0.0f, btVector3(23.75f, 0.0f, 0.0f),
			btQuaternion(btVector3(0.0f, 0.0f, 1.0f), glm::radians(90.0f))));
		StaticBodies.push_back(CreateRigidBody(dynamicsWorld, collisionShapes[1], 0.0f, btVector3(0.0f, 0.0f, -23.75f),
			btQuaternion(btVector3(1.0f, 0.0f, 0.0f), glm::radians(90.0f))));
		StaticBodies.push_back(CreateRigidBody(dynamicsWorld, collisionShapes[1], 0.0f, btVector3(0.0f, 0.0f, 23.75f),
			btQuaternion(btVector3(1.0f, 0.0f, 0.0f), glm::radians(90.0f))));
	}

	//Get Shader Uniform Identifier
	GLuint ProjectionLoc = glGetUniformLocation(shaderprogram, "projection"),
		ViewLoc = glGetUniformLocation(shaderprogram, "view"),
		ModelLoc = glGetUniformLocation(shaderprogram, "model"),
		NormLoc = glGetUniformLocation(shaderprogram, "normal"),
		ObjColorLoc = glGetUniformLocation(shaderprogram, "objectColor"),
		LightColorLoc = glGetUniformLocation(shaderprogram, "lightColor"),
		LightPosLoc = glGetUniformLocation(shaderprogram, "lightPos"),
		ViewPosLoc = glGetUniformLocation(shaderprogram, "viewPos");

	//Bind Uniforms constant through all Model Instances
	glUseProgram(shaderprogram);
	glUniform3fv(ObjColorLoc, 1, glm::value_ptr(glm::vec3(0.8f, 0.8f, 0.8f)));
	glUniform3fv(LightColorLoc, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
	glUniform3fv(LightPosLoc, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 50.0f)));
	glUniform3fv(ViewPosLoc, 1, glm::value_ptr(camera.GetPos()));
	glUseProgram(0);

	//Uncomment this to render as wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//timing
	const double timestep = 1.0 / 60.0;
	double lastFrame = 0.0, accumulator = 0.0, deltatime = 0.0;

	//Main Loop
	while (!glfwWindowShouldClose(window))
	{
		//Process keyboard
		processInput(window, &camera, deltatime);

		//Clear Background
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Render Model Instances
		glUseProgram(shaderprogram);
		for (size_t i = 0 ; i < objects.size() ; i++)
		{
			//Model Matrix
			glm::mat4 ModelMatrix = objects[i].GetModelMatrix();

			//Bind Uniforms exclusive to the shape
			glUniformMatrix4fv(ProjectionLoc, 1, GL_FALSE, glm::value_ptr(camera.GetProjectionMatrix()));
			glUniformMatrix4fv(ViewLoc, 1, GL_FALSE, glm::value_ptr(objects[i].GetCamera()->GetViewMatrix()));
			glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, glm::value_ptr(ModelMatrix));
			glUniformMatrix3fv(NormLoc, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(ModelMatrix)))));

			//Bind VAO of the shape of the Instance
			glBindVertexArray(objects[i].GetModel()->GetVAO());

			//Draw Shape
			glDrawElements(GL_TRIANGLES, objects[i].GetModel()->IndexCount(), GL_UNSIGNED_INT, (void*)0);

			// size of "objects" is 1 larger than "Rigidbodies", with the last object being the box, which is static and requires no update
			if (i < Rigidbodies.size())
			{
				btTransform newTransform;
				Rigidbodies[i]->getMotionState()->getWorldTransform(newTransform);
				glm::vec3 newPos(
					newTransform.getOrigin().getX(),
					newTransform.getOrigin().getY(),
					newTransform.getOrigin().getZ());

				//Update positions of Model Instances
				objects[i].SetPos(newPos);
			}
		}

		glfwSwapBuffers(window);
		glfwPollEvents();


		//Timestep
		double currentFrame = glfwGetTime();
		deltatime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		accumulator += deltatime;

		//Update Physics
		while (accumulator >= timestep)
		{
			dynamicsWorld->stepSimulation(timestep, 1);
			accumulator -= timestep;
		}
	}

	for (auto it : Rigidbodies)
	{
		delete it->getMotionState();
		dynamicsWorld->removeRigidBody(it);
		delete it;
	}
	Rigidbodies.clear();

	for (auto it : StaticBodies)
	{
		delete it->getMotionState();
		dynamicsWorld->removeRigidBody(it);
		delete it;
	}
	StaticBodies.clear();

	for (auto it : collisionShapes)
		delete it;
	collisionShapes.clear();

	delete dynamicsWorld;
	delete solver;
	delete overlappingPairCache;
	delete dispatcher;
	delete collisionConfiguration;

	glDeleteProgram(shaderprogram);

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow*, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouseinput_callback(GLFWwindow* window, double posx, double posy)
{
	//Use mouse to steer view
	static double lastx = scrnx / 2, lasty = scrny / 2, yaw = -90.0, pitch = 0.0;
	static constexpr double sensitivity = 0.1;
	cameraobject* pcam = (cameraobject*)glfwGetWindowUserPointer(window);

	double offx = (posx - lastx) * sensitivity, offy = (lasty - posy) * sensitivity;
	lastx = posx, lasty = posy;
	yaw += offx, pitch += offy;
	if (pitch > 89.0)
		pitch = 89.0;
	if (pitch < -89.0)
		pitch = -89.0;
	pcam->SetFront(glm::normalize(glm::vec3(
		cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
		sin(glm::radians(pitch)),
		sin(glm::radians(yaw)) * cos(glm::radians(pitch)))));
}

void processInput(GLFWwindow* const window, cameraobject* const pcam, double delta)
{
	//use WASD to move camera
	float cameraSpeed = 25.0f * (float)delta;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		pcam->ModPos(cameraSpeed * pcam->GetFront());
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		pcam->ModPos(-cameraSpeed * pcam->GetFront());
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		pcam->ModPos(-glm::normalize(glm::cross(pcam->GetFront(), pcam->GetUp())) * cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		pcam->ModPos(glm::normalize(glm::cross(pcam->GetFront(), pcam->GetUp())) * cameraSpeed);
}

//Wrapper for creating Rigid Bodies
btRigidBody* CreateRigidBody(btDiscreteDynamicsWorld* dworld, btCollisionShape* shape,
	float _mass, const btVector3& _pos, const btQuaternion& _quat)
{
	btTransform transform;
	transform.setIdentity();

	btScalar mass(_mass);
	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	shape->calculateLocalInertia(mass, localInertia);

	transform.setOrigin(_pos);
	transform.setRotation(_quat);
	btDefaultMotionState* newMotionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, newMotionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	body->setRestitution(1.0f);
	dworld->addRigidBody(body);

	return body;
}