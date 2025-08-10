// Mandelbrot class, adapted from Adam Sampson's lab example

#pragma once
#include "SFML\Graphics.hpp"
#include "Framework/Input.h"
#include "Framework/GameObject.h"
#include "farm.h"
#include "task.h"
#include "MandelbrotTask.h"
#include <cstdint>
#include <cstdlib>
#include <complex>
#include <fstream>
#include <iostream>
#include <amp.h>
#include <amp_math.h>
#include <iomanip>

// Import things we need from the standard library
using std::complex;
using std::ofstream;
using namespace concurrency;

// The size of the image to generate.
const int WIDTH = 1920;
const int HEIGHT = 1200;

// Using our own Complex number structure and definitions as the Complex type is not available in the Concurrency namespace.
// ----- //
struct Complex {
	double x;
	double y;
};

inline Complex c_add(Complex c1, Complex c2) restrict(cpu, amp)
{
	Complex tmp;
	double a = c1.x;
	double b = c1.y;
	double c = c2.x;
	double d = c2.y;
	tmp.x = a + c;
	tmp.y = b + d;
	return tmp;
} // c_add

inline double c_abs(Complex c) restrict(amp)
{
	return concurrency::fast_math::sqrt(c.x * c.x + c.y * c.y);
} // c_abs amp

inline double c_abs(Complex c) restrict(cpu)
{
	return concurrency::fast_math::sqrt(c.x * c.x + c.y * c.y);
} // c_abs cpu

inline Complex c_mul(Complex c1, Complex c2) restrict(cpu, amp)
{
	Complex tmp;
	double a = c1.x;
	double b = c1.y; 
	double c = c2.x;
	double d = c2.y;
	tmp.x = a * c - b * d;
	tmp.y = b * c + a * d;
	return tmp;
} // c_mul
// ----- //

// Inherits from GameObject, allowing for the object itself to have a texture and position for drawing it to the window.
class Mandelbrot :
	public GameObject
{
public:
	Mandelbrot();
	// Functions for generating the mandelbrot set using different methods.
	void compute_mandelbrot_gpu(double left, double right, double top, double bottom, double zoom, double xPos, double yPos);
	void compute_mandelbrot_cpu(double left, double right, double top, double bottom, double zoom, double xPos, double yPos, int threads);
	void compute_mandelbrot_task_farm(double left, double right, double top, double bottom, double zoom, double xPos, double yPos, int threads, int tasks);

	// Function to check if the user's machine supports AMP.
	bool query_AMP_support();

	// Function to output the mandelbrot to a .tga file.
	void write_tga(const char* filename);
	
	// Getter and setter functions for the max number of iterations.
	int getIterations()
	{
		return MAX_ITERATIONS;
	};
	void setIterations(int x)
	{
		MAX_ITERATIONS = x;
	};

	// Function to set the colour of a pixel in the image, used in the mandelbrot task class when generating the mandelbrot as it does not have direct access to the image.
	void setImage(int x, int y, int colour)
	{
		image[x][y] = colour;
	}

private:
	// Functions for listing the details about each accelerator that supports AMP.
	void report_accelerator(const accelerator a);
	void list_accelerators();

	// Function to be passed into threads for computing a slice of the mandelbrot set's image.
	void compute_mandelbrot_thread(double left, double right, double top, double bottom, double zoom, double xPos, double yPos, double start, double end);

	// The image data.
	// Each pixel is represented as 0xRRGGBB.
	uint32_t image[HEIGHT][WIDTH];

	// The number of times to iterate before we assume that a point isn't in the mandelbrot set.
	int MAX_ITERATIONS = 500;
};

