#include "PlanetSystem.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <GL/freeglut.h>

PlanetSystem::PlanetSystem(cg::GLSLProgram* program)
	: program(program),
	model(glm::mat4x4(1.0f)),
	zAngle(0.0),
	planetOffset(0.0),
	planetAngle(45.0),
	speed(1.0),
	lastRenderTime(0),
	elapsedTime(0),
	isPaused(false)
{
	sun = new Sphere(program);
	sun->setRadius(sunRadius);
	sun->setColor(sunColor);
	planet = new Sphere(program);
	planet->setRadius(planetRadius);
	planet->setColor(planetColor);
	moon = new Sphere(program);
	moon->setRadius(moonRadius);
	moon->setColor(moonColor);
	axis = new Axis(program);
	axis->setColor(axisColor);
}


PlanetSystem::~PlanetSystem()
{
	delete sun;
	delete planet;
	delete moon;
	delete axis;
}


void PlanetSystem::init()
{
	sun->init();
	planet->init();
	moon->init();
	axis->init();
}

void PlanetSystem::render()
{
	// Berechne verstrichene Zeit
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	elapsedTime = currentTime - lastRenderTime;
	lastRenderTime = currentTime;

	// Baue die szene
	stack.push(model);

	// Sonne
	renderSun();

	// Planet 1
	renderPlanet(elapsedTime);

	model = stack.top();
	stack.pop();
}

void PlanetSystem::setView(glm::mat4x4 view)
{
	this->view = view;
}

void PlanetSystem::setProjection(glm::mat4x4 projection)
{
	this->projection = projection;
}

void PlanetSystem::increaseZangle()
{
	if (zAngle < maxZAngle)
	{
		zAngle += zAngleStepsize;
	}
}

void PlanetSystem::decreaseZangle()
{
	if (zAngle > minZAngle)
	{
		zAngle -= zAngleStepsize;
	}
}

void PlanetSystem::increasePlanetOffset()
{
	planetOffset += planetOffsetStepsize;
}

void PlanetSystem::decreasePlanetOffset()
{
	planetOffset -= planetOffsetStepsize;
}

void PlanetSystem::increaseSpeed()
{
	speed += speedStep;
}

void PlanetSystem::decreaseSpeed()
{
	speed -= speedStep;
}

void PlanetSystem::renderSun()
{
	stack.push(model);

	// Sonnensystem Neigung
	model = glm::rotate(model, glm::radians<float>(zAngle), zAxis);

	// Sonne
	sun->render(projection * view * model);

	// Sonnenachse
	axis->render(projection * view * model);

	model = stack.top();
	stack.pop();
}

void PlanetSystem::renderPlanet(int elapsedTime)
{
	static float currentOrbitAngle = glm::radians<float>(planetStartAngle);
	static float currentRotationAngle = 0.0f;

	stack.push(model);

	// Sonnensystem Neigung
	model = glm::rotate(model, glm::radians<float>(zAngle), zAxis);

	// Y Offset
	model = glm::translate(model, glm::vec3(0.0f, planetOffset, 0.0f));

	
		// Position
	if (!isPaused) {
		currentOrbitAngle += glm::radians<float>(elapsedTime * speed * planetOrbitSpeed);
	}
		model = glm::translate(
			model,
			glm::vec3(planetDistance * glm::cos(currentOrbitAngle), 0.0f, -planetDistance * glm::sin(currentOrbitAngle))
		);

	

	// Rotation
	stack.push(model);
	if (!isPaused) {
		currentRotationAngle += glm::radians<float>(elapsedTime * speed * planetRotationSpeed);
	}
	model = glm::rotate(model, currentRotationAngle, yAxis);
	planet->render(projection * view * model);
	model = stack.top();
	stack.pop();

	// Achse
	axis->render(projection * view * model);

	// Monde
	renderMoons(elapsedTime);

	model = stack.top();
	stack.pop();
}

void PlanetSystem::renderMoons(int elapsedTime)
{
	static float currentOrbitAngle = 0.0f;
	currentOrbitAngle += glm::radians<float>(elapsedTime * speed * planetMoonRotationSpeed);

	for (int i = 0; i < moonsPlanet; i++)
	{
		stack.push(model);
		model = glm::rotate(
			model,
			currentOrbitAngle + glm::two_pi<float>() / moonsPlanet * i,
			yAxis);
		model = glm::translate(model, glm::vec3(planetMoonDistance, 0.0f, 0.0f));
		moon->render(projection * view * model);
		model = stack.top();
		stack.pop();
	}
}

void PlanetSystem::pausePlanet() {
	isPaused = true;
}

void PlanetSystem::resumePlanet() {
	isPaused = false;
}

bool PlanetSystem::isPlanetPaused() {
	return isPaused;
}