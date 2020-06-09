#pragma once
#include <stack>
#include "GLSLProgram.h"
#include "Sphere.h"
#include "Axis.h"
class PlanetSystem
{
public: 
		PlanetSystem(cg::GLSLProgram* program);
		~PlanetSystem();
		void init();
		void render();
		void setView(glm::mat4x4 view);
		void setProjection(glm::mat4x4 projection);
		
		void increaseZangle();
		void decreaseZangle();

		void increasePlanetOffset();
		void decreasePlanetOffset();

		void increaseSpeed();
		void decreaseSpeed();

		void pausePlanet();
		void resumePlanet();
		bool isPlanetPaused();

private:
	void renderSun();
	void renderPlanet(int et);
	void renderMoons(int et);

	std::stack <glm::mat4x4> stack;

	cg::GLSLProgram* program;
	glm::mat4x4 model;
	glm::mat4x4 view;
	glm::mat4x4 projection;


	Sphere* sun;
	Sphere* planet;
	Sphere* moon;
	Axis* axis;
	
	// variables
	double zAngle;
	double planetOffset;
	double planetAngle;
	double speed;
	bool isPaused;

	// Time variable
	int lastRenderTime;
	int elapsedTime;

	// constants
	const double sunRadius = 0.5;
	const double planetRadius = 0.5;
	const double moonRadius = 0.2;
	const double zAngleStepsize = 1.0;
	const double maxZAngle = 360.0;
	const double minZAngle = 0.0;
	const double speedStep = 0.2;
	const glm::vec3 sunColor = glm::vec3(1.0f, 0.5f, .0f);
	const glm::vec3 planetColor = glm::vec3(0.0f, 1.0f, 0.0f);
	const glm::vec3 moonColor = glm::vec3(0.0f, 0.0f, 1.0f);
	const glm::vec3 axisColor = glm::vec3(1.0f, 0.0f, 0.0f);
	const glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	const glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);


	// Planet  Konstanten
	const int moonsPlanet = 2;
	const double planetMoonDistance = 1.0;
	const double planetOffsetStepsize = 0.2;
	const double planetDistance = 6.0;
	const double planetStartAngle = 0.0;
	const double planetOrbitSpeed = 10.0 / 1000.0;        // 10° pro sekunde
	const double planetRotationSpeed = 30.0 / 1000.0;     // 30° pro sekunde
	const double planetMoonRotationSpeed = 20.0 / 1000.0; // 20° pro sekunde
};

