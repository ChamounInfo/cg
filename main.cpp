#include <iostream>
#include <vector>

#include <GL/glew.h>
//#include <GL/gl.h> // OpenGL header not necessary, included by GLEW
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "GLSLProgram.h"
#include "GLTools.h"

#include <math.h>
// Standard window width
const int WINDOW_WIDTH  = 640;
// Standard window height
const int WINDOW_HEIGHT = 480;
// GLUT window id/handle
int glutID = 0;

cg::GLSLProgram program;

glm::mat4x4 view;
glm::mat4x4 projection;

float zNear = 0.1f;
float zFar  = 100.0f;

float zoom = 4.0f;

void convertRGBtoCMY(double red, double green, double blue, double &cyan, double &magenta, double &yellow) {
	if (red > 1 || red < 0 || blue < 0 || blue > 1 || green < 0 || green > 1) {
		std::cout << "r,g or b is out of range->" << "r: " << red << " g: " << green << " b: " << blue << "\n";
		return;
	}
	cyan = 1 - red;
	magenta = 1 - green;
	yellow = 1 - blue;

	std::cout << "result convert rgb to cmy: " << "c: " << cyan <<  "m: " << magenta << " y: " << yellow << "\n";
}
void convertRGBtoHSV(double red, double green, double blue, int &h, double &s, double &v) {

	if (red > 1 || red < 0 || blue < 0 || blue > 1 || green < 0 || green > 1) {
		std::cout << "r,g or b is out of range" << "r: " << red << "g" << green << "b" << blue << "\n";
		return;
	}
	double cMax = blue;
	double cMin = blue;

	if (red > cMax) {
		cMax = red;
	}
	if (green > cMax) {
		cMax = green;
	}
	if (red < cMin) {
		cMin = red;
	}
	if (green < cMin) {
		cMin = green;
	}

	v = cMax;

	double delta = cMax - cMin;
	if (delta == 0) {
		h = 0;
	}
	else if (cMax == red) {
		h = (int)(60 * (fmod((green - blue) / delta, 6)));
	}
	else if (cMax == green) {
		h = (int)(60 * (((blue - red) / delta) + 2));
	}

	else if (cMax == blue) {
		h = (int)(60 * (((red - green) / delta) + 4));
	}

	if (cMax == 0) {
		s = 0;
	}
	else {
		s = delta / cMax;
	}

	std::cout << "result convert rgb to hsv-> " << "h: " << h << " s: " << s << " v: " << v << "\n";
}
void convertCMYtoRGB(double cyan, double magenta, double yellow, double &red, double &green, double &blue) {
	if (cyan > 1 || cyan < 0 || magenta < 0 || magenta > 1 || yellow < 0 || yellow > 1) {
		std::cout << "c,m or y is out of range" << "c: " << cyan << "m" << magenta << "y" << yellow << "\n";
		return;
	}
	red = 1 - cyan;
	green = 1 - magenta;
	blue = 1 - yellow;
	std::cout << "result convert cmy to rgb-> " << "red: " << red << " green: " << green << " blue: " << blue << "\n";
}
void convertCMYtoHSV(double cyan, double magenta, double yellow, int &h, double &s, double &v) {
	double red = 1;
	double green = 1;
	double blue = 1;

	convertCMYtoRGB(cyan, magenta, yellow, red, green, blue);
	convertRGBtoHSV(red, green, blue, h, s, v);
	std::cout << "result convert cmy to hsv: " << "h: " << h << " s: " << s << " v: " << v << "\n";

}
void convertHSVtoRGB(int h, float s, float v, double &red, double &green, double &blue) {
	if (h > 360 || h < 0 || s < 0 || s > 1 || v < 0 || v > 1) {
		std::cout << "h,s or is out of range"<< "h: " << h << "s: " << s << "v: " << v<<"\n";
		return;
	}

	double c = v * s;
	double x = c * (1 - abs((fmod(abs(h / 60), 2) - 1)));
	double m = v - c;

	if (h >= 0 && h < 60) {
		red = c + m;
		green = x + m;
		blue = 0 + m;
	} else if (h >= 60 && h < 120) {
		red = x + m;
		green = c + m;
		blue = 0 + m;
	} else if (h >= 120 && h < 180) {
		red = 0 + m;
		green = c + m;
		blue = x + m;
	} else if (h >= 180 && h < 240) {
		red = 0 + m;
		green = x + m;
		blue = c + m;
	} else if (h >= 240 && h < 300) {
		red = x + m;
		green = 0 + m;
		blue = c + m;
	} else if (h >= 300 && h < 360) {
		red = c + m;
		green = 0 + m;
		blue = x + m;
	}
	std::cout << "result convert hsv to rgb-> " << "red: " << red << " green: " << green << " blue: " << blue << "\n";
}
void convertHSVtoCMY(int h, float s, float v, double &cyan, double &magenta, double &yellow) {
	convertHSVtoRGB(h, s, v, cyan, magenta, yellow);
	convertRGBtoCMY(cyan, magenta, yellow, cyan, magenta, yellow);
	std::cout << "result convert hsv to cmy: " << "c: " << cyan << " m: " << magenta << ":y " << yellow << "\n";
}

void readFromConsoleRGB(double &red, double &green, double &blue) {
	if (red == NULL || green == NULL || blue == NULL) {
		std::cout << "either red, green or blue is null";
		return;
	}
	red = -1;
	while (red == -1) {
		std::cout << "Enter red/cyan value [0,1]:\n";
		std::cin >> red;
		if (red > 1 || red < 0) {
			std::cout << " red/cyan is out of range: " << red << "\n";
			red = -1;
		}
	}
	blue = -1;
	while (blue == -1) {
		std::cout << "Enter blue/magenta value [0,1]:\n";
		std::cin >> blue;
		if (blue > 1 || blue < 0) {
			std::cout << " blue/magenta is out of range: " << blue << "\n";
			blue = -1;
		}
	}
	green = -1;
	while (green == -1) {
		std::cout << "Enter green/yellow value [0,1]:\n";
		std::cin >> green;
		if (green > 1 || green < 0) {
			std::cout << " green/yellow is out of range: " << green << "\n";
			green = -1;
		}
	}
	std::cout << "Your input is red : " << red << " green: " << green << " blue: " << blue << "\n";
}
void readFromConsoleHSV(int &h, double &s, double &v) {
	if (h == NULL || s == NULL || v == NULL) {
		std::cout << "either h, s or v is null";
		return;
	}
	h = -1;
	while (h == -1) {
		std::cout << "Enter h value [0,360[:\n";
		std::cin >> h;
		if (h >= 360 || h < 0) {
			std::cout << " h is out of range: " << h << "\n";
			h = -1;
		}
	}
	s = -1;
	while (s == -1) {
		std::cout << "Enter s value [0,1]:\n";
		std::cin >> s;
		if (s > 1 || s < 0) {
			std::cout << " s is out of range: " << s << "\n";
			s = -1;
		}
	}
	v = -1;
	while (v == -1) {
		std::cout << "Enter v value [0,1]:\n";
		std::cin >> v;
		if (v > 1 || v < 0) {
			std::cout << " v is out of range: " << v << "\n";
			v = -1;
		}
	}
	std::cout << "Your input is h : " << h << " s: " << s << " v: " << v << "\n";
}

/*
Struct to hold data for object rendering.
*/
class Object
{
public:
  inline Object ()
    : vao(0),
      positionBuffer(0),
      colorBuffer(0),
      indexBuffer(0)
  {}

  inline ~Object () { // GL context must exist on destruction
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteBuffers(1, &colorBuffer);
    glDeleteBuffers(1, &positionBuffer);
  }

  GLuint vao;        // vertex-array-object ID
  
  GLuint positionBuffer; // ID of vertex-buffer: position
  GLuint colorBuffer;    // ID of vertex-buffer: color
  
  GLuint indexBuffer;    // ID of index-buffer
  
  glm::mat4x4 model; // model matrix
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> colors;
  std::vector<GLushort>  indices;
};

Object triangle;
std::vector<Object> triangles;
Object quad;

Object sphere;

void renderTriangle(Object &triangle)
{
	// Create mvp.
	glm::mat4x4 mvp = projection * view * triangle.model;

	// Bind the shader program and set uniform(s).
	program.use();
	program.setUniform("mvp", mvp);

	// Bind vertex array object so we can render the 1 triangle.
	glBindVertexArray(triangle.vao);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

void renderTriangle()
{
  // Create mvp.
  glm::mat4x4 mvp = projection * view * triangle.model;
  
  // Bind the shader program and set uniform(s).
  program.use();
  program.setUniform("mvp", mvp);
  
  // Bind vertex array object so we can render the 1 triangle.
  glBindVertexArray(triangle.vao);
  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
  glBindVertexArray(0);
}

void renderQuad(Object &quad)
{
	// Create mvp.
	glm::mat4x4 mvp = projection * view * quad.model;

	// Bind the shader program and set uniform(s).
	program.use();
	program.setUniform("mvp", mvp);

	// Bind vertex array object so we can render the 2 triangles.
	glBindVertexArray(quad.vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

void renderQuad()
{
  // Create mvp.
  glm::mat4x4 mvp = projection * view * quad.model;
  
  // Bind the shader program and set uniform(s).
  program.use();
  program.setUniform("mvp", mvp);
  
  // Bind vertex array object so we can render the 2 triangles.
  glBindVertexArray(quad.vao);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
  glBindVertexArray(0);
}

void renderObject(Object& object, int numberOfPoints)
{
	// Create mvp.
	glm::mat4x4 mvp = projection * view * object.model;



	// Bind the shader program and set uniform(s).
	program.use();
	program.setUniform("mvp", mvp);

	// Bind vertex array object so we can render the 2 triangles.
	glBindVertexArray(object.vao);
	//glDrawElements(GL_TRIANGLES, object.indices.size(), GL_UNSIGNED_SHORT, 0);
	glDrawArrays(GL_TRIANGLES, 0, object.vertices.size());
	glBindVertexArray(0);
}

void initTriangle(Object &triangle )
{
	
	GLuint programId = program.getHandle();
	GLuint pos;

	// Step 0: Create vertex array object.
	glGenVertexArrays(1, &triangle.vao);
	glBindVertexArray(triangle.vao);

	// Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
	glGenBuffers(1, &triangle.positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, triangle.positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, triangle.vertices.size() * sizeof(glm::vec3), triangle.vertices.data(), GL_STATIC_DRAW);

	// Bind it to position.
	pos = glGetAttribLocation(programId, "position");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 2: Create vertex buffer object for color attribute and bind it to...
	glGenBuffers(1, &triangle.colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, triangle.colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, triangle.colors.size() * sizeof(glm::vec3), triangle.colors.data(), GL_STATIC_DRAW);

	// Bind it to color.
	pos = glGetAttribLocation(programId, "color");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 3: Create vertex buffer object for indices. No binding needed here.
	glGenBuffers(1, &triangle.indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangle.indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangle.indices.size() * sizeof(GLushort), triangle.indices.data(), GL_STATIC_DRAW);

	// Unbind vertex array object (back to default).
	glBindVertexArray(0);

	// Modify model matrix.
	triangle.model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.25f, 0.0f, 0.0f));
}

void initTriangle()
{
  // Construct triangle. These vectors can go out of scope after we have send all data to the graphics card.
  const std::vector<glm::vec3> vertices = { glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f) };
  const std::vector<glm::vec3> colors   = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) };
  const std::vector<GLushort>  indices  = { 0, 1, 2 };

  GLuint programId = program.getHandle();
  GLuint pos;

  // Step 0: Create vertex array object.
  glGenVertexArrays(1, &triangle.vao);
  glBindVertexArray(triangle.vao);
  
  // Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
  glGenBuffers(1, &triangle.positionBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, triangle.positionBuffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);



  
  // Bind it to position.
  pos = glGetAttribLocation(programId, "position");
  glEnableVertexAttribArray(pos);
  glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
  

  // Step 2: Create vertex buffer object for color attribute and bind it to...
  glGenBuffers(1, &triangle.colorBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, triangle.colorBuffer);
  glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);



  // Bind it to color.
  pos = glGetAttribLocation(programId, "color");
  glEnableVertexAttribArray(pos);
  glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

  
  // Step 3: Create vertex buffer object for indices. No binding needed here.
  glGenBuffers(1, &triangle.indexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangle.indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);


  
  // Unbind vertex array object (back to default).
  glBindVertexArray(0);
  
  // Modify model matrix.
  triangle.model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.25f, 0.0f, 0.0f));

}

void initQuad(Object &quad)
{
	// Construct triangle. These vectors can go out of scope after we have send all data to the graphics card.
	//const std::vector<glm::vec3> vertices = { { -1.0f, 1.0f, 0.0f },{ -1.0, -1.0, 0.0 },{ 1.0f, -1.0f, 0.0f },{ 1.0f, 1.0f, 0.0f } };
	//const std::vector<glm::vec3> colors = { { 1, 0, 0 },{ 1, 0, 0 },{ 1, 0, 0 },{ 1, 0, 0 } };
	//const std::vector<GLushort>  indices = { 0, 1, 2, 0, 2, 3 };

	GLuint programId = program.getHandle();
	GLuint pos;

	// Step 0: Create vertex array object.
	glGenVertexArrays(1, &quad.vao);
	glBindVertexArray(quad.vao);

	// Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
	glGenBuffers(1, &quad.positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad.positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, quad.vertices.size() * sizeof(glm::vec3), quad.vertices.data(), GL_STATIC_DRAW);

	// Bind it to position.
	pos = glGetAttribLocation(programId, "position");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 2: Create vertex buffer object for color attribute and bind it to...
	glGenBuffers(1, &quad.colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad.colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, quad.colors.size() * sizeof(glm::vec3), quad.colors.data(), GL_STATIC_DRAW);

	// Bind it to color.
	pos = glGetAttribLocation(programId, "color");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 3: Create vertex buffer object for indices. No binding needed here.
	glGenBuffers(1, &quad.indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, quad.indices.size() * sizeof(GLushort), quad.indices.data(), GL_STATIC_DRAW);

	// Unbind vertex array object (back to default).
	glBindVertexArray(0);

	// Modify model matrix.
	quad.model = glm::translate(glm::mat4(1.0f), glm::vec3(1.25f, 0.0f, 0.0f));
}

void initQuad()
{
  // Construct triangle. These vectors can go out of scope after we have send all data to the graphics card.
  const std::vector<glm::vec3> vertices = { { -1.0f, 1.0f, 0.0f }, { -1.0, -1.0, 0.0 }, { 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f } };
  const std::vector<glm::vec3> colors   = { { 1, 0, 0 }, {1, 0, 0 },{ 1, 0, 0 },{ 1, 0, 0 } };
  const std::vector<GLushort>  indices  = { 0, 1, 2, 0, 2, 3 };

  GLuint programId = program.getHandle();
  GLuint pos;
  
  // Step 0: Create vertex array object.
  glGenVertexArrays(1, &quad.vao);
  glBindVertexArray(quad.vao);
  
  // Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
  glGenBuffers(1, &quad.positionBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, quad.positionBuffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
  
  // Bind it to position.
  pos = glGetAttribLocation(programId, "position");
  glEnableVertexAttribArray(pos);
  glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
  
  // Step 2: Create vertex buffer object for color attribute and bind it to...
  glGenBuffers(1, &quad.colorBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, quad.colorBuffer);
  glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
  
  // Bind it to color.
  pos = glGetAttribLocation(programId, "color");
  glEnableVertexAttribArray(pos);
  glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
  
  // Step 3: Create vertex buffer object for indices. No binding needed here.
  glGenBuffers(1, &quad.indexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
  
  // Unbind vertex array object (back to default).
  glBindVertexArray(0);
  
  // Modify model matrix.
  quad.model = glm::translate(glm::mat4(1.0f), glm::vec3(1.25f, 0.0f, 0.0f));
}

void initObject(Object& object)
{

	GLuint programId = program.getHandle();
	GLuint pos;

	// Step 0: Create vertex array object.
	glGenVertexArrays(1, &object.vao);
	glBindVertexArray(object.vao);

	// Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
	glGenBuffers(1, &object.positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, object.positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, object.vertices.size() * sizeof(glm::vec3), object.vertices.data(), GL_STATIC_DRAW);

	// Bind it to position.
	pos = glGetAttribLocation(programId, "position");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 2: Create vertex buffer object for color attribute and bind it to...
	glGenBuffers(1, &object.colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, object.colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, object.colors.size() * sizeof(glm::vec3), object.colors.data(), GL_STATIC_DRAW);

	// Bind it to color.
	pos = glGetAttribLocation(programId, "color");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 3: Create vertex buffer object for indices. No binding needed here.
	glGenBuffers(1, &object.indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, object.indices.size() * sizeof(GLushort), object.indices.data(), GL_STATIC_DRAW);

	// Unbind vertex array object (back to default).
	glBindVertexArray(0);

	// Modify model matrix.
	object.model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.25f, 0.0f, 0.0f));
}

void normalize(glm::vec3 origin, glm::vec3 v, float length, glm::vec3& normalizedVector) {
	
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

//	std::cout << normalizedVector[0] << " " << normalizedVector[1] << " " << normalizedVector[2] << "\n";
}

void subdivide(glm::vec3& v1, glm::vec3& v2, glm::vec3& v3, long depth, Object &object, int index, glm::vec3 origin, float radius)
{
	glm::vec3 v12, v23, v31;
	GLint i;
	
	if (depth == 0) {
		object.vertices.push_back(v1);
		object.vertices.push_back(v2);
		object.vertices.push_back(v3);
		object.indices.push_back(index);
		index++;
		object.indices.push_back(index);
		index++;
		object.indices.push_back(index);
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
	index += 12;
	subdivide(v1, v12, v31, depth - 1, object, index, origin, radius);
	subdivide(v2, v23, v12, depth - 1, object, index, origin, radius);
	subdivide(v3, v31, v23, depth - 1, object, index, origin, radius);
	subdivide(v12, v23, v31, depth - 1, object, index, origin, radius);
}

void computeHalfVertex(const glm::vec3 v1, const glm::vec3 v2, glm::vec3 newV, const float radius)
{
	newV[0] = v1[0] + v2[0];    // x
	newV[1] = v1[1] + v2[1];    // y
	newV[2] = v1[2] + v2[2];    // z
	float scale = radius / sqrtf(newV[0] * newV[0] + newV[1] * newV[1] + newV[2] * newV[2]);
	newV[0] *= scale;
	newV[1] *= scale;
	newV[2] *= scale;
}

void initSphere(float radius) {
	glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);
	std::vector<glm::vec3> vertices = { 
		//oben
		glm::vec3(0.0f, 1.0f, 0.0f),
		//unten
		glm::vec3(0.0f, -1.0f, 0.0f),
		//vorne
		glm::vec3(0.0f, 0.0f, 1.0f),
		//rechts
		glm::vec3(1.0f, 0.0f, 0.0f),
		//links
		glm::vec3(-1.0f, 0.0f, 0.0f),
		//hinten
		glm::vec3(0.0f, 0.0f, -1.0f)
	};
	std::vector<glm::vec3> colors = { glm::vec3(1.0f, 0.0f, 0.0f),glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f)};
	std::vector<GLushort>  indices = { 

							 0,2,3,    
							 0,2,4,
							 0,5,4,
							 0,5,3,
							 1,2,4,
							 1,2,3,
							 1,5,3,
							 1,5,4
	};
	std::cout << "Beginn\n";
	for (auto &i : vertices) {
		std::cout <<"vertice: " << i.x << " " << i.y << " " << i.z << "\n";
	//	normalize(origin, i, radius, i);
		std::cout << "normalized vertice: " << i.x << " " << i.y << " " << i.z << "\n";
	}
	std::cout << "End\n";
	sphere.colors.clear();
	sphere.vertices.clear();
	sphere.indices.clear();

	std::vector<glm::vec3> tmpVertices;
	std::vector<GLushort> tmpIndices;
	const glm::vec3* v1, * v2, * v3;          // ptr to original vertices of a triangle
	glm::vec3 newV1, newV2, newV3; // new vertex positions
	unsigned int index;


	int subdivision = 0;
	// iterate all subdivision levels
	/*
	for (int i = 1; i <= subdivision; ++i)
	{
		// copy prev vertex/index arrays and clear
		tmpVertices = vertices;
		tmpIndices = indices;
		vertices.clear();
		indices.clear();
		index = 0;

		// perform subdivision for each triangle
		for (int j = 0; j < tmpIndices.size(); j += 3)
		{
			// get 3 vertices of a triangle
			v1 = &tmpVertices[tmpIndices[j] * 3];
			v2 = &tmpVertices[tmpIndices[j + 1]*3];
			v3 = &tmpVertices[tmpIndices[j + 2] *3];

			// compute 3 new vertices by spliting half on each edge
			//         v1       
			//        / \       
			// newV1 *---* newV3
			//      / \ / \     
			//    v2---*---v3   
			//       newV2      
			computeHalfVertex(*v1, *v2, newV1, radius);
			computeHalfVertex(*v2, *v3, newV2, radius);
			computeHalfVertex(*v1, *v3, newV3, radius);

			// add 4 new triangles to vertex array
			vertices.push_back(*v1);
			vertices.push_back(newV1);
			vertices.push_back(newV3);

			vertices.push_back(newV1);
			vertices.push_back(*v2);
			vertices.push_back(newV2);

			vertices.push_back(newV1);
			vertices.push_back(newV2);
			vertices.push_back(newV3);

			vertices.push_back(newV3);
			vertices.push_back(newV2);
			vertices.push_back(*v3);


			// add indices of 4 new triangles
			indices.push_back(index);
			indices.push_back(index+1);
			indices.push_back(index+2);

			indices.push_back(index+3);
			indices.push_back(index+4);
			indices.push_back(index+5);

			indices.push_back(index+6);
			indices.push_back(index+7);
			indices.push_back(index+8);

			indices.push_back(index+9);
			indices.push_back(index+10);
			indices.push_back(index+11);

			index += 12;    // next index
		}
	}
	*/


	for (auto &i : vertices) {
		//normalize(i, origin, 1, i);
	}
	
	subdivide(vertices[0], vertices[2], vertices[4], subdivision, sphere, 0, origin, radius);
	subdivide(vertices[0], vertices[3], vertices[2], subdivision, sphere, 0, origin, radius);
	subdivide(vertices[0], vertices[3], vertices[5], subdivision, sphere, 0, origin, radius);
	subdivide(vertices[0], vertices[4], vertices[5], subdivision, sphere, 0, origin, radius);


	subdivide(vertices[1], vertices[2], vertices[4], subdivision, sphere, 0, origin, radius);
	subdivide(vertices[1], vertices[3], vertices[2], subdivision, sphere, 0, origin, radius);
	subdivide(vertices[1], vertices[3], vertices[5], subdivision, sphere, 0, origin, radius);
	subdivide(vertices[1], vertices[4], vertices[5], subdivision, sphere, 0, origin, radius);


	/*
	subdivide(vertices[0], vertices[1], vertices[2], 0, sphere, 0, origin, radius);
	subdivide(vertices[0], vertices[2], vertices[3], 0, sphere, 0, origin, radius);
	subdivide(vertices[0], vertices[3], vertices[4], 0, sphere, 0, origin, radius);
	subdivide(vertices[0], vertices[4], vertices[1], 0, sphere, 0, origin, radius);


	
	subdivide(vertices[5], vertices[1], vertices[2], 0, sphere, 0, origin, radius);
	subdivide(vertices[5], vertices[2], vertices[3], 0, sphere, 0, origin, radius);
	subdivide(vertices[5], vertices[3], vertices[4], 0, sphere, 0, origin, radius);
	subdivide(vertices[5], vertices[4], vertices[1], 0, sphere, 0, origin, radius);*/
	for (auto i : sphere.vertices) {
		//std::cout << i.x << " " << i.y << " " << i.z << "\n";
	}

	for (int i = 0; i < sphere.vertices.size(); i++) {
		sphere.colors.push_back({ 1.0f, 1.0f, 0.0f });
	}

	std::cout << sphere.vertices.size() << "\n";
	std::cout << sphere.indices.size() << "\n";
	std::cout << sphere.colors.size() << "\n";
	initObject(sphere);
}


Object koodinates;
void renderKoodinates()
{
	// Create mvp.
	glm::mat4x4 mvp = projection * view * koodinates.model;

	// Bind the shader program and set uniform(s).
	program.use();
	program.setUniform("mvp", mvp);

	// Bind vertex array object 
	glBindVertexArray(koodinates.vao);
	glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_SHORT, 0);// der 2. Parameter gibt an, wieviele Knoten gemacht werden müssen (3 Triangle, 6 Quad)
	glBindVertexArray(0);
}
void initKoodinates()
{
	GLuint programId = program.getHandle();
	GLuint pos;
	const int size = 10;
	const std::vector<glm::vec3> vertices = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(size, 0.0f, 0.0f), glm::vec3(0.0f, size, 0.0f), glm::vec3(0.0f, 0.0f,size) };
	const std::vector<glm::vec3> colors = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) };
	const std::vector<GLushort>  indices = { 0, 1, 1, 0, 2, 2, 0, 3, 3 };

	// Step 0: Create vertex array object.
	glGenVertexArrays(1, &koodinates.vao);//Unterschied zu Quad
	glBindVertexArray(koodinates.vao);//Unterschied zu Quad

	// Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
	glGenBuffers(1, &koodinates.positionBuffer);//Unterschied zu Quad
	glBindBuffer(GL_ARRAY_BUFFER, koodinates.positionBuffer);//Unterschied zu Quad
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

	// Bind it to position.
	pos = glGetAttribLocation(programId, "position");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 2: Create vertex buffer object for color attribute and bind it to...
	glGenBuffers(1, &koodinates.colorBuffer);//Unterschied zu Quad
	glBindBuffer(GL_ARRAY_BUFFER, koodinates.colorBuffer);//Unterschied zu Quad
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);

	// Bind it to color.
	pos = glGetAttribLocation(programId, "color");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 3: Create vertex buffer object for indices. No binding needed here.
	glGenBuffers(1, &koodinates.indexBuffer); // Unterschied zu Quad
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, koodinates.indexBuffer);//Unterschied zu Quad
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

	// Unbind vertex array object (back to default).
	glBindVertexArray(0);

	// Modify model matrix.
	koodinates.model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.25f, 0.0f, 0.0f));//Unterschied zu Quad

}
/*
 Initialization. Should return true if everything is ok and false if something went wrong.
 */
bool init()
{
	// OpenGL: Set "background" color and enable depth testing.
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// Construct view matrix.
	glm::vec3 eye(0.0f, 0.0f, zoom); //Kamera Position
	glm::vec3 center(0.0f, 0.0f, 0.0f); // looking at
 	glm::vec3 up(0.0f, 1.0f, 0.0f); // what direction ius up

	view = glm::lookAt(eye, center, up);

	// Create a shader program and set light direction.
	if (!program.compileShaderFromFile("shader/simple.vert", cg::GLSLShader::VERTEX)) {
		std::cerr << program.log();
		return false;
	}

	if (!program.compileShaderFromFile("shader/simple.frag", cg::GLSLShader::FRAGMENT)) {
		std::cerr << program.log();
		return false;
	}

	if (!program.link()) {
		std::cerr << program.log();
		return false;
	}

	// Create all objects.
	initSphere(1);
	initKoodinates();
	//  initObject(triangle);
  return true;
}

/*
 Rendering.
 */
void render()
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*
	for (auto triangle : triangles) {
		renderTriangle(triangle);
	}
	//renderTriangle();
	renderQuad(quad);
	*/
	//renderObject(triangle, 3);
	renderObject(sphere, sphere.vertices.size());
	renderKoodinates();
}

void glutDisplay ()
{
   render();
   glutSwapBuffers();
}

/*
 Resize callback.
 */
void glutResize (int width, int height)
{
  // Division by zero is bad...
  height = height < 1 ? 1 : height;
  glViewport(0, 0, width, height);
  
  // Construct projection matrix.
  projection = glm::perspective(45.0f, (float) width / height, zNear, zFar);
}

/*
 Callback for char input.
 */
void glutKeyboard (unsigned char keycode, int x, int y)
{
	double cyan = 1;
	double magenta = 1;
	double yellow = 1;
	double red = 1;
	double green = 1;
	double blue = 1;
	int hue = 1;
	double saturation = 1;
	double value = 1;

  switch (keycode) {
  case 27: // ESC
    glutDestroyWindow ( glutID );
    return;
    
  case '+':
    // do something
    break;
  case '-':
    // do something
    break;
  case 'x':
	  sphere.model = glm::rotate(sphere.model, glm::radians(45.0f), glm::vec3(1.0, 0.0, 0.0));
    break;
  case 'y':
	  sphere.model = glm::rotate(sphere.model, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
	  break;
  case 'z':
	  sphere.model = glm::rotate(sphere.model, glm::radians(45.0f), glm::vec3(0.0, 0.0, 1.0));
	  break;/*
  case 'c':
	  
	  readFromConsoleRGB(cyan, magenta, yellow);
	  convertCMYtoRGB(cyan, magenta, yellow, red, blue, green);

	  
	  for (auto& color : quad.colors) {
		  color[0] = red;
		  color[1] = green;
		  color[2] = blue;
	  }
	
	  initQuad(quad);
	  break;
  case 'h':
	 
	  readFromConsoleHSV(hue, saturation, value);
	  convertHSVtoRGB(hue, saturation, value, red, blue, green);


	  for (auto& color : quad.colors) {
		  color[0] = red;
		  color[1] = green;
		  color[2] = blue;
	  }

	  initQuad(quad);
	  break;
  case 'r':
	
	  readFromConsoleRGB(red, green, blue);

	  for (auto& color : quad.colors) {
		  color[0] = red;
		  color[1] = green;
		  color[2] = blue;
	  }

	  initQuad(quad);
	  break;
  case 'a':

	  readFromConsoleRGB(red, green, blue);
	  convertRGBtoCMY(red, green, blue, cyan, magenta, yellow);
	  convertRGBtoHSV(red, green, blue, hue, saturation, value);
	  break;
  case 'b':

	  readFromConsoleHSV(hue, saturation, value);
	  convertHSVtoRGB(hue, saturation, value, red, green, blue);
	  convertHSVtoCMY(hue, saturation, value, cyan, magenta, yellow);
	  break; */
  case 'a'://NEU
	  if (zoom > 0) {
		  zoom = zoom - 0.25;
		  init();
	  }
	  std::cout << "zoomStufe: " << zoom << std::endl;
	  break;
  case 's'://NEU
	  if (zoom < 15) {
		  zoom = zoom + 0.25;
		  init();
	  }
	  std::cout << "zoomStufe: " << zoom << std::endl;
	  break;
  }
 

  glutPostRedisplay();
}

int main(int argc, char** argv)
{
  // GLUT: Initialize freeglut library (window toolkit).
  glutInitWindowSize    (WINDOW_WIDTH, WINDOW_HEIGHT);
  glutInitWindowPosition(40,40);
  glutInit(&argc, argv);
  
  // GLUT: Create a window and opengl context (version 4.3 core profile).
  glutInitContextVersion(4, 3);
  glutInitContextFlags  (GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
  glutInitDisplayMode   (GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
  
  glutCreateWindow("Aufgabenblatt 01");
  glutID = glutGetWindow();
  
  // GLEW: Load opengl extensions
  //glewExperimental = GL_TRUE;
  
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


  if (glewInit() != GLEW_OK) {
    return -1;
  }
#if _DEBUG
  if (glDebugMessageCallback) {
    std::cout << "Register OpenGL debug callback " << std::endl;
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(cg::glErrorVerboseCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE,
			  GL_DONT_CARE,
			  GL_DONT_CARE,
			  0,
			  nullptr,
			  true); // get all debug messages
  } else {
    std::cout << "glDebugMessageCallback not available" << std::endl;
  }
#endif

  // GLUT: Set callbacks for events.
  glutReshapeFunc(glutResize);
  glutDisplayFunc(glutDisplay);
  //glutIdleFunc   (glutDisplay); // redisplay when idle
  
  glutKeyboardFunc(glutKeyboard);
  
  // init vertex-array-objects.
  bool result = init();
  if (!result) {
    return -2;
  }


 // 
  // GLUT: Loop until the user closes the window
  // rendering & event handling
  glutMainLoop ();
  
  // Cleanup in destructors:
  // Objects will be released in ~Object
  // Shader program will be released in ~GLSLProgram
 
  return 0;
}
