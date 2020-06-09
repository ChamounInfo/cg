#pragma once
#include "GLSLProgram.h"
#include "Object.h"

class Axis : Object
{
public:
	Axis(cg::GLSLProgram* prog);
	~Axis();
	void init();
	void render(glm::mat4x4 mvp);
	void setColor(glm::vec3);

private:
	cg::GLSLProgram* program;

	// Konstanten
	const float yAxisStart = -2.0f;
	const float yAxisEnd = 2.0f;

	glm::vec3 color = { 1.0f, 0.0f, 0.0f };

	std::vector<glm::vec3> vertices;
	std::vector<GLushort> indices;
	std::vector<glm::vec3> colors;
};