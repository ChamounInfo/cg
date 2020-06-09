#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "GLSLProgram.h"
#include "GLTools.h"
#include "Sphere.h"

Sphere::Sphere(cg::GLSLProgram* prog) : program(prog) {  }

Sphere::~Sphere() {  }

void Sphere::init()
{
	calculateVertices();
	for (int i = 0; i < vertices.size(); i++)
	{
		colors.push_back(color);
	}

	GLuint programId = program->getHandle();
	GLuint pos;

	// Step 0: Create vertex array object.
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

	// Bind it to position.
	pos = glGetAttribLocation(programId, "position");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 2: Create vertex buffer object for color attribute and bind it to...
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);

	// Bind it to color.
	pos = glGetAttribLocation(programId, "color");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 3: Create vertex buffer object for indices. No binding needed here.
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

	// Unbind vertex array object (back to default).
	glBindVertexArray(0);
}

void Sphere::render(glm::mat4x4 mvp)
{
	// Bind the shader program and set uniform(s).
	program->use();
	program->setUniform("mvp", mvp);

	// Bind vertex array object so we can render the 2 triangles.
	glBindVertexArray(vao);
	//glDrawElements(GL_TRIANGLES, object.indices.size(), GL_UNSIGNED_SHORT, 0);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);
}

void Sphere::setColor(glm::vec3 c)
{
	color = c;
}

void Sphere::setSubDivisions(unsigned int div)
{
	subdivision = div;
}

void Sphere::setRadius(float r)
{
	radius = r;
}

void Sphere::normalize(glm::vec3 origin, glm::vec3 v, float length, glm::vec3& normalizedVector) {

	//get the distance between aand b along the xand y axes;;
	float dx = v[0] - origin[0];
	float dy = v[1] - origin[1];
	float dz = v[2] - origin[2];
	//right now, sqrt(dx ^ 2 + dy ^ 2) = distance(a, b).
	float distance = sqrt((dx * dx) + (dy * dy) + (dz * dz));
	//we want to modify them so that sqrt(dx ^ 2 + dy ^ 2) = the given length.

	dx = dx * length / distance;
	distance = sqrt((dx * dx) + (dy * dy) + (dz * dz));
	dy = dy * length / distance;
	distance = sqrt((dx * dx) + (dy * dy) + (dz * dz));
	dz = dz * length / distance;
	normalizedVector[0] = origin[0] + dx;
	normalizedVector[1] = origin[1] + dy;
	normalizedVector[2] = origin[2] + dz;
}

void Sphere::subdivide(glm::vec3& v1, glm::vec3& v2, glm::vec3& v3, long depth, glm::vec3 origin)
{
	glm::vec3 v12, v23, v31;
	GLint i;

	if (depth == 0) {
		vertices.push_back(v1);
		vertices.push_back(v2);
		vertices.push_back(v3);
		return;
	}
	v12.x = v1.x + v2.x;
	v12.y = v1.y + v2.y;
	v12.z = v1.z + v2.z;

	v23.x = v2.x + v3.x;
	v23.y = v2.y + v3.y;
	v23.z = v2.z + v3.z;

	v31.x = v3.x + v1.x;
	v31.y = v3.y + v1.y;
	v31.z = v3.z + v1.z;


	normalize(origin, v12, radius, v12);
	normalize(origin, v23, radius, v23);
	normalize(origin, v31, radius, v31);
	subdivide(v1, v12, v31, depth - 1,  origin);
	subdivide(v2, v23, v12, depth - 1,  origin);
	subdivide(v3, v31, v23, depth - 1,  origin);
	subdivide(v12, v23, v31, depth - 1, origin);
}

void Sphere::calculateVertices() {
	std::cout << "Error in calculateVertices?";
	glm::vec3 origin = { 0.0f,0.0f,0.0f };
	std::vector<glm::vec3> vertices2 = {
		//oben
		//glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(origin.x, origin.y + radius, origin.z),
		//unten
		//glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(origin.x, origin.y - radius, origin.z),
		//vorne
		//glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(origin.x, origin.y, origin.z + radius),
		//rechts
		//glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(origin.x + radius, origin.y, origin.z),
		//links
		//glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(origin.x - radius, origin.y, origin.z),
		//hinten
		//glm::vec3(0.0f, 0.0f, -1.0f)
		glm::vec3(origin.x, origin.y, origin.z - radius)
	};
	
	colors.clear();
	vertices.clear();
	indices.clear();

	for (auto& vertice : vertices2) {
		normalize(origin, vertice, radius, vertice);
	}
	// iterate all subdivision levels

	subdivide(vertices2[0], vertices2[2], vertices2[4], subdivision,   origin );
	subdivide(vertices2[0], vertices2[3], vertices2[2], subdivision,   origin);
	subdivide(vertices2[0], vertices2[3], vertices2[5], subdivision,  origin);
	subdivide(vertices2[0], vertices2[4], vertices2[5], subdivision,  origin);


	subdivide(vertices2[1], vertices2[2], vertices2[4], subdivision,  origin);
	subdivide(vertices2[1], vertices2[3], vertices2[2], subdivision,  origin);
	subdivide(vertices2[1], vertices2[3], vertices2[5], subdivision,  origin);
	subdivide(vertices2[1], vertices2[4], vertices2[5], subdivision,  origin);
	std::cout << "Error in calculateVertices?NO";
}
