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

void convertRGBtoCMY(double red, double green, double blue, double &cyan, double &magenta, double &yellow) {
	std::cout << "input convert rgb to cmy-> " << "r: " << red << " g: " << green << " b: " << blue << "\n";
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
	std::cout << "input convert rgb to hsv-> " << "r: " << red << " g: " << green << " b: " << blue << "\n";

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
	std::cout << "input convert cmy to rgb-> " << "c: " << cyan << " m: " << magenta << " y: " << yellow << "\n";
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
	std::cout << "input convert cmy to hsv-> " << "c: " << cyan << " m: " << magenta << " y: " << yellow << "\n";
	double red = 1;
	double green = 1;
	double blue = 1;

	convertCMYtoRGB(cyan, magenta, yellow, red, green, blue);
	convertRGBtoHSV(red, green, blue, h, s, v);
	std::cout << "result convert cmy to hsv: " << "h: " << h << " s: " << s << " v: " << v << "\n";

}
void convertHSVtoRGB(int h, float s, float v, double &red, double &green, double &blue) {
	std::cout << "input convert hsv to rgb: " << "h: " << h << " s: " << s << " v: " << v << "\n";
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
	std::cout << "convert hsv to cmy: " << "h: " << h << " s: " << s << " v: " << v << "\n";
	convertHSVtoRGB(h, s, v, cyan, magenta, yellow);
	convertRGBtoCMY(cyan, magenta, yellow, cyan, magenta, yellow);
	std::cout << "convert hsv to cmy: " << "c: " << cyan << " m: " << magenta << ":y " << yellow << "\n";
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

/*
 Initialization. Should return true if everything is ok and false if something went wrong.
 */
bool init()
{
	// OpenGL: Set "background" color and enable depth testing.
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// Construct view matrix.
	glm::vec3 eye(0.0f, 0.0f, 4.0f);
	glm::vec3 center(0.0f, 0.0f, 0.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);

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
	quad.vertices.push_back({ -1.0f, 1.0f, 0.0f });
	quad.vertices.push_back({ -1.0, -1.0, 0.0 });
	quad.vertices.push_back({ 1.0f, -1.0f, 0.0f });
	quad.vertices.push_back({ 1.0f, 1.0f, 0.0f });
	quad.colors.push_back({1, 0, 0});
	quad.colors.push_back({ 1, 0, 0 });
	quad.colors.push_back({ 1, 0, 0 });
	quad.colors.push_back({ 1, 0, 0 });
	  quad.indices = { 0, 1, 2, 0, 2, 3 };
  initTriangle();
  initQuad(quad);
  
  return true;
}

/*
 Rendering.
 */
void render()
{
	/*
	double red = 1;
	double green = 1;
	double blue = 1;

	int h = 1;
	double s = 1;
	double v = 1;

	double cyan = 1;
	double magenta = 1;
	double  yellow = 1;

	readFromConsoleRGB(red, green, blue);

	convertRGBtoCMY(red, green, blue, cyan, magenta, yellow);
	convertRGBtoHSV(red, green, blue, h, s, v);
	 h = 1;
	 s = 1;
	 v = 1;
	readFromConsoleHSV(h, s, v);
	convertHSVtoCMY(h, s, v, cyan, magenta, yellow);
	convertHSVtoRGB(h, s, v, red, green, blue);*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto triangle : triangles) {
		renderTriangle(triangle);
	}
	//renderTriangle();
	renderQuad(quad);
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
    // do something
    break;
  case 'y':
    // do something
    break;
  case 'z':
    // do something
    break;
  case 'c':
	  
	  readFromConsoleRGB(cyan, magenta, yellow);
	  convertCMYtoRGB(cyan, magenta, yellow, red, blue, green);

	  
	  for (auto& color : quad.colors) {
		  color[0] = red;
		  color[1] = green;
		  color[2] = blue;
		  std::cout << "hi\n";
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
