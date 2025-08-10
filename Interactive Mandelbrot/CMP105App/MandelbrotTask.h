#pragma once
#include "task.h"
#include "Mandelbrot.h"

class Mandelbrot;

class MandelbrotTask :
	public Task
{
public:
	// Set all the values in the constructor.
	MandelbrotTask(double l, double r, double t, double b, double z, double xP, double yP, double sT, double sB, Mandelbrot* m)
	{
		left = l;
		right = r;
		top = t;
		bottom = b;
		zoom = z;
		xPos = xP;
		yPos = yP;
		sliceTop = sT;
		sliceBottom = sB;
		mandelbrot = m;
	};

	// Function to run the task.
	void run();
private:
	// Variables for holding the parameters of the mandelbrot set.
	double left, right, top, bottom, zoom, xPos, yPos, sliceTop, sliceBottom;

	// Pointer to the mandelbrot object for writing to its image and getting the max number of iterations.
	Mandelbrot* mandelbrot;
};


