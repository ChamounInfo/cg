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
#include "PlanetSystem.h"
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

PlanetSystem * planetSystem = new PlanetSystem(&program);


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
	planetSystem->init();
	planetSystem->setView(view);
	
	

  return true;
}

/*
 Rendering.
 */
void render()
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	planetSystem->render();
}

void glutDisplay ()
{
	GLCODE(render());
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
  planetSystem->setProjection(projection);
}

/*
 Callback for char input.
 */
void glutKeyboard (unsigned char keycode, int x, int y)
{
	switch (keycode)
	{
	case 27: // ESC
		glutDestroyWindow(glutID);
		return;
	case 'q':
		planetSystem->increaseZangle();
		break;
	case 'w':
		planetSystem->decreaseZangle();
		break;
	case 'i':
		planetSystem->decreasePlanetOffset();
		break;
	case 'u':
		planetSystem->increasePlanetOffset();
		break;
	case 'd':
		planetSystem->decreaseSpeed();
		break;
	case 'f':
		planetSystem->increaseSpeed();
		break;

	case 'g':
		if (planetSystem->isPlanetPaused()) {
			planetSystem->resumePlanet();
		}
		else {
			planetSystem->pausePlanet();
		}
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
  glutIdleFunc(glutDisplay); // redisplay when idle
  
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
