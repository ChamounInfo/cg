#pragma once
#include "Object.h"
#include <vector>

class Sphere : public Object
{
public:
	Sphere(cg::GLSLProgram* prog);
	~Sphere();

	void init();
	void render(glm::mat4x4);
	void setColor(glm::vec3);
	void setSubDivisions(unsigned int);
	void setRadius(float);

private:
	cg::GLSLProgram* program;

	std::vector<glm::vec3> vertices;
	std::vector<GLushort> indices;
	std::vector<glm::vec3> colors;
	glm::vec3 color = { 1.0f, 1.0f, 0.0f };
	unsigned int subdivision = 0;
	float radius = 1.0f;

	void calculateVertices();
	void normalize(glm::vec3 origin, glm::vec3 v, float length, glm::vec3& normalizedVector);
	void subdivide(glm::vec3& v1, glm::vec3& v2, glm::vec3& v3, long depth, glm::vec3 origin);
};