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
- Simple Phong lightning DONE.
- Multiple lights CURRENT...
- ShadowMap
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
glm::vec3 directionalLightPos(2.5f, 0.0f, 0.0f);

glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};

glm::vec3 testPositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};


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

	//glfwSwapInterval(1);
	// NOTE: OpenGL error checks have been omitted for brevity

	// Callbacks
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGL()) {
		std::cerr << "Something went wrong!" << std::endl;
		exit(EXIT_FAILURE);
	}

	glDebugMessageCallback(opengl_error_callback, nullptr);

	// UVs
	//Either use both this AND aiProcess_FlipUVs in model with assimp
	//Or nothing
	//stbi_set_flip_vertically_on_load(true);

	// Shader program
	Shader modelShader{ "resources/shaders/shader.vert", "resources/shaders/shader.frag" };
	modelShader.use();

	// Models
	Model backpack("resources/models/backpack/backpack.obj");

	//Options
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//std::cout << "DeltaTime : " << deltaTime << std::endl;

		key_callback(window);

		//glfwGetFramebufferSize(window, &width, &height);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		modelShader.use();
		modelShader.setVec3("viewPos", camera.getPosition());
		modelShader.setFloat("material.shininess", 32.0f); //TODO : extract from assimp model

		// directional light
		modelShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		modelShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		modelShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		modelShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		modelShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		modelShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		modelShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		modelShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		modelShader.setFloat("pointLights[0].constant", 1.0f);
		modelShader.setFloat("pointLights[0].linear", 0.09);
		modelShader.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		modelShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		modelShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		modelShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		modelShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		modelShader.setFloat("pointLights[1].constant", 1.0f);
		modelShader.setFloat("pointLights[1].linear", 0.09);
		modelShader.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		modelShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		modelShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		modelShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		modelShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		modelShader.setFloat("pointLights[2].constant", 1.0f);
		modelShader.setFloat("pointLights[2].linear", 0.09);
		modelShader.setFloat("pointLights[2].quadratic", 0.032);
		// point light 4
		modelShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		modelShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		modelShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		modelShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		modelShader.setFloat("pointLights[3].constant", 1.0f);
		modelShader.setFloat("pointLights[3].linear", 0.09);
		modelShader.setFloat("pointLights[3].quadratic", 0.032);
		// spotLight
		modelShader.setVec3("spotLight.position", camera.getPosition());
		modelShader.setVec3("spotLight.direction", camera.getFront());
		modelShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		modelShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		modelShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		modelShader.setFloat("spotLight.constant", 1.0f);
		modelShader.setFloat("spotLight.linear", 0.09);
		modelShader.setFloat("spotLight.quadratic", 0.032);
		modelShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		modelShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		//Camera
		glm::mat4 proj = glm::perspective(glm::radians(camera.getFov()), (float)width / (float)height, 0.1f, 100.0f);
		glm::mat4 view = camera.getViewMatrix();
		modelShader.setMat4("proj", proj);
		modelShader.setMat4("view", view);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		modelShader.setMat4("model", model);

		//backpack.draw(modelShader);

		for (unsigned int i = 0; i < 10; i++) {

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, testPositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			model = glm::scale(model, glm::vec3(0.3f));

			modelShader.setMat4("model", model);

			backpack.draw(modelShader);
		}



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