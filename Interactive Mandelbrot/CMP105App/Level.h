#pragma once

#include <SFML/Graphics.hpp>
#include "Framework/Input.h"
#include "Framework/GameObject.h"
#include "Framework/Collision.h"
#include <string.h>
#include <iostream>
#include "Mandelbrot.h"
#include <fstream>

using std::chrono::duration_cast;
using std::chrono::milliseconds;

class Level{
public:
	Level(sf::RenderWindow* hwnd, Input* in);
	~Level();

	// SFML functions.
	void handleInput(float dt);
	void update(float dt);
	void render();

	// Computes the mandelbrot set.
	void computeMandelbrot();

	// Gets a pointer to the mandelbrot object.
	Mandelbrot* getMandelbrot()
	{
		return mandelbrot;
	}

	// Get the time taken to compute the last mandelbrot set.
	float getMandelbrotTime()
	{
		return mandelbrotTime;
	}

	// Toggle between using the GPU or CPU.
	void toggleModes()
	{
		gpuMode = !gpuMode;
	}

	// Return whether the mode is currently set to GPU or CPU.
	bool getGpuMode()
	{
		return gpuMode;
	}

	// Setter and getter for zoomIncrement variable.
	void setZoomIncrement(double x)
	{
		zoomIncrement = x;
	}
	double getZoomIncrement()
	{
		return zoomIncrement;
	}

	// Setter and getter for moveIncrement variable.
	void setMoveIncrement(double x)
	{
		moveIncrement = x;
	}
	double getMoveIncrement()
	{
		return moveIncrement;
	}

	// Setter and getter for the number of threads.
	void setThreads(int x)
	{
		cpuThreads = x;
	}
	int getThreads()
	{
		return cpuThreads;
	}

	// Setter and getter for the number of tasks.
	void setTasks(int x)
	{
		taskCount = x;
	}
	int getTasks()
	{
		return taskCount;
	}

	// Function that handles generating the results file.
	void processResults(int x);

	// Mutex and conditon variable to prevent the console input thread from changing values such as max iterations during the computation of the mandelbrot set.
	bool computingMandelbrot = false;
	std::mutex mandelbrotMutex;
	std::condition_variable mandelbrotCV;
private:
	// Default functions for rendering to the screen.
	void beginDraw();
	void endDraw();
	
	// Functions for manipulating the mandelbrot set.
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();
	void zoomIn();
	void zoomOut();

	// Resets the mandelbrot's parameter values.
	void reset();

	// Function used in generating the results file.
	void generateResults(std::ofstream* results, double l, double r, double t, double b, int testTimes);

	// For timing.
	typedef std::chrono::steady_clock the_clock;

	// Default variables for level class.
	sf::RenderWindow* window;
	Input* input;
	sf::Font font;

	// Pointer to a mandelbrot object. The object holds the image of the mandelbrot set as a texture by inheriting from the GameObject class, and also contains the functions necessary to compute it.
	Mandelbrot* mandelbrot;

	// SFML objects for the user interface.
	// ----- //
	GameObject cursor;
	GameObject leftButton;
	GameObject rightButton;
	GameObject upButton;
	GameObject downButton;
	GameObject zoomInButton;
	GameObject zoomOutButton;
	GameObject resetButton;
	GameObject outputWindow;

	sf::RectangleShape crosshair;

	sf::Texture mandelbrotTexture;
	sf::Texture crosshairTexture;
	sf::Texture arrowTextures[4];
	sf::Texture plusTexture;
	sf::Texture minusTexture;
	sf::Texture resetTexture;
	
	sf::Text text;
	// ----- //

	// Co-ordinates and zoom for the mandelbrot set.
	double left, right, top, bottom, zoom;

	// Keep track of offset so mandelbrot will always zoom into the centre of the image.
	double xPos, yPos;

	// For holding how much the zoom and movement are increased by when receiving input.
	double zoomIncrement, moveIncrement;

	// Holds how long it takes to calculate the mandelbrot set.
	float mandelbrotTime;
	
	// Whether the mandelbrot should use the GPU or CPU.
	bool gpuMode;

	// Whether the program is currently generating the results file. When it is, we don't want anything else to be able to interfere with the mandelbrot generation.
	bool generatingResults;

	// The number of CPU threads and tasks.
	int cpuThreads;
	int taskCount;
};