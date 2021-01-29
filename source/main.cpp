#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>
#include <random>
#include <sstream>
#include <fstream>
#include <string>

#include "shader.h"
#include "model.h"

/**
## TODO List :

- Clean repositories DONE.
- Clean source code DONE.
- Create GLFW Window (resizeable) DONE.
- Model loading & Meshes (trying with a simple cube at first) DONE.
- Shader for Model & Meshes DONE.
- Camera controls CURRENT...
- Simple Phong lightning (not necessarly Spot & Point base, could be direct lightning only like from the sun) DONE.
- Baked ShadowMap
- Framebuffer
- SSAO
**/

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int width = 800;
int height = 800;

static void error_callback(int /*error*/, const char* description)
{
	std::cerr << "Error: " << description << std::endl;
}

static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

std::default_random_engine generator;
std::uniform_real_distribution<float> distribution01(0, 1);
std::uniform_real_distribution<float> distributionWorld(-1, 1);


void APIENTRY opengl_error_callback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar *message,
		const void *userParam)
{
	std::cout << message << std::endl;
}

glm::vec3 ScreenCoordinatesToWorldCoordinates(double &xpos, double &ypos) {
	return glm::vec3(xpos / width * 2.0f - 1.0f, (ypos / height * 2.0f - 1.0f) * -1.0f, 1.0f);
}

int main(void)
{
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	window = glfwCreateWindow(width, height, "Simple example", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	// NOTE: OpenGL error checks have been omitted for brevity

	if(!gladLoadGL()) {
		std::cerr << "Something went wrong!" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Callbacks
	glDebugMessageCallback(opengl_error_callback, nullptr);

	// Camera
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));

	// Shader program
	Shader program{ "resources/shaders/shader.vert", "resources/shaders/shader.frag" };
	program.use();

	Model backpack("resources/models/backpack/backpack.obj");

	//Options
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

		program.use();
		program.setMat4("proj", proj);
		program.setMat4("view", view);
		program.setMat4("model", model);

		backpack.draw(program);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
