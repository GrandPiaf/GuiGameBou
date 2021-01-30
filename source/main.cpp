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
#include "camera.h"
#include <stb_image.h>

/**
## TODO List :

- Clean repositories DONE.
- Clean source code DONE.
- Create GLFW Window (resizeable) DONE.
- Model loading & Meshes (trying with a simple cube at first) DONE.
- Shader for Model & Meshes DONE.
- Camera controls DONE.
- Simple Phong lightning (not necessarly Spot & Point base, could be direct lightning only like from the sun) CURRENT...
- Baked ShadowMap
- Framebuffer
- SSAO
**/

static void error_callback(int /*error*/, const char *description);
static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
static void key_callback(GLFWwindow *window);
static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void APIENTRY opengl_error_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

std::default_random_engine generator;
std::uniform_real_distribution<float> distribution01(0, 1);
std::uniform_real_distribution<float> distributionWorld(-1, 1);

// Time
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int width = 800;
int height = 800;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
bool firstMouse = true;
float lastX = width / 2.0f;
float lastY = height / 2.0f;

// lighting
glm::vec3 lightPos(2.5f, 0.0f, 0.0f);

int main(void) {
	GLFWwindow *window;
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	window = glfwCreateWindow(width, height, "GuiGameBou", NULL, NULL);

	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);
	// NOTE: OpenGL error checks have been omitted for brevity

	if (!gladLoadGL()) {
		std::cerr << "Something went wrong!" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Callbacks
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glDebugMessageCallback(opengl_error_callback, nullptr);

	// UVs
	//Either use both this AND aiProcess_FlipUVs in model with assimp
	//Or nothing
	//stbi_set_flip_vertically_on_load(true);

	// Shader program
	Shader program{ "resources/shaders/shader.vert", "resources/shaders/shader.frag" };
	program.use();

	// Models
	Model backpack("resources/models/backpack/backpack.obj");

	//Options
	glEnable(GL_DEPTH_TEST);

	glm::mat4 view;
	glm::mat4 proj;

	while (!glfwWindowShouldClose(window)) {

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		key_callback(window);

		glfwGetFramebufferSize(window, &width, &height);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Camera
		proj = glm::perspective(glm::radians(camera.getFov()), (float)width / (float)height, 0.1f, 100.0f);
		view = camera.getViewMatrix();

		glm::mat4 model = glm::mat4(1.0f);

		program.use();
		program.setMat4("proj", proj);
		program.setMat4("view", view);
		program.setMat4("model", model);

		program.setFloat("material.shininess", 32.0f);

		program.setVec3("light.position", lightPos);
		program.setVec3("viewPos", camera.getPosition());
		glm::vec3 lightColor;
		lightColor.x = sin(glfwGetTime() * 2.0f);
		lightColor.y = sin(glfwGetTime() * 0.7f);
		lightColor.z = sin(glfwGetTime() * 1.3f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
		//program.setVec3("light.ambient", ambientColor);
		//program.setVec3("light.diffuse", diffuseColor);
		program.setVec3("light.ambient", glm::vec3(0.0f, 1.0f, 1.0f));
		program.setVec3("light.diffuse", glm::vec3(1.0f, 1.0f, 0.0f));
		program.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		backpack.draw(program);

		//model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f));
		//program.setMat4("model", model);
		//backpack.draw(program);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}


static void error_callback(int /*error*/, const char *description) {
	std::cerr << "Error: " << description << std::endl;
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

static void key_callback(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.keyProcess(CameraMovement::FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.keyProcess(CameraMovement::BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.keyProcess(CameraMovement::LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.keyProcess(CameraMovement::RIGHT, deltaTime);
	}
}

static void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
	if (firstMouse) { // initially set to true
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	camera.mouseProcess(xoffset, yoffset);
}

static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
	camera.scrollProcess(yoffset);
}

void APIENTRY opengl_error_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
	std::cout << message << std::endl;
}