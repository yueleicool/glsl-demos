#include "Plane.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include "TextureManager.h"
using namespace std;

glm::vec2 texSize;

Plane::Plane(GLFWwindow* window, int size){
	this->size = size;
	this->window = window;
	planePos = vec3(0.0f, 0.0f, 2.5f);
}

void Plane::init(){
	genPlane();
	genBuffers();

	// init matrices
	modelMatrix = glm::mat4(1.0f);
	viewMatrix = glm::lookAt(
		vec3(0.0f, 0.0f, -1.0f), //eye
		vec3(0.0f, 0.0f, 0.0f), //center
		vec3(0.0f, 1.0f, 0.0f)); //up
	projectionMatrix = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 100.0f);

	// load shaders
	try {
		shader.compileShader("shader/glsl40_2D_outline.vert");
		shader.compileShader("shader/glsl40_2D_outline.frag");

		shader.link();
		shader.use();
	}
	catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	shader.printActiveAttribs();

	glActiveTexture(GL_TEXTURE0);
	TextureManager::Inst()->LoadTexture("..\\..\\resources\\tex14.png", 1);
	texSize = vec2(TextureManager::Inst()->size[1]);
	cout << glm::to_string(texSize) << endl;

}

void Plane::update(double deltaTime){
	// matrices setup
	modelMatrix = mat4(1.0f); // identity
	modelMatrix = glm::translate(modelMatrix, planePos); // translate back
	modelViewMatrix = viewMatrix * modelMatrix;
	modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

	// set var MVP on the shader
	shader.setUniform("MVP", modelViewProjectionMatrix); //ModelViewProjection
	shader.setUniform("TexSize", texSize);
}

void Plane::render(){
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vaoID);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (GLubyte *)NULL);
	glBindVertexArray(0);
}

void Plane::genBuffers() {

	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	unsigned int handle[3];
	glGenBuffers(3, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), (GLvoid*)&vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
	glEnableVertexAttribArray(0);  // Vertex position -> layout 0 in the VS

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, texUV.size() * sizeof(vec2), (GLvoid*)&texUV[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)));
	glEnableVertexAttribArray(1);  // texture coords -> layout 1 in the VS

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), (GLvoid*)&indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Plane::resize(int x, int y) {

}

void Plane::genPlane() {

	// v0 -- top left
	vertices.push_back(vec3(-size, size, 0.0f));
	texUV.push_back(vec2(0.0f, 1.0f));

	//v1 -- bottom left
	vertices.push_back(vec3(-size, -size, 0.0f));
	texUV.push_back(vec2(0.0f, 0.0f));

	// v2 -- bottom right
	vertices.push_back(vec3(size, -size, 0.0f));
	texUV.push_back(vec2(1.0f, 0.0f));

	//v3 -- top right
	vertices.push_back(vec3(size, size, 0.0f));
	texUV.push_back(vec2(1.0f, 1.0f));

	// we'll have two triangles, one being v0,v1,v2 and the other v2,v3,v0
	// triangle 1
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	//// triangle 2
	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(3);


}