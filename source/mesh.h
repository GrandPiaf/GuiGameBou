#pragma once

#include <string>
#include <vector>

#include <glad\glad.h>
#include <glm\glm.hpp>

#include "shader.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normals;
	glm::vec2 texCoords;
};

struct Texture {
	GLuint id;
	std::string type;
	std::string path; // we store the path of the texture to compare with other textures
};

class Mesh {

public:
	// mesh data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void draw(Shader &shader);

private:
	// render data
	GLuint VAO, VBO, EBO;
	void setupMesh();

};