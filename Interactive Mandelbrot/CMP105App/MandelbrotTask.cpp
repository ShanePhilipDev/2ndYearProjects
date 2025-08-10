#include "MandelbrotTask.h"

// Calculating the mandelbrot set in slices.
void MandelbrotTask::run()
{
	for (int y = sliceTop; y < sliceBottom; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			// Work out the point in the complex plane that
			// corresponds to this pixel in the output image.
			complex<double> c(xPos + (left + (x * (right - left) / WIDTH)) * zoom,
				yPos + (top + (y * (bottom - top) / HEIGHT)) * zoom);

			// Start off z at (0, 0).
			complex<double> z(0.0, 0.0);

			// Iterate z = z^2 + c until z moves more than 2 units
			// away from (0, 0), or we've iterated too many times.
			int iterations = 0;
			while (abs(z) < 2.0 && iterations < mandelbrot->getIterations())
			{
				z = (z * z) + c;

				++iterations;

				// Set the colour of the mandelbrot at this point to be equal to the current iteration.
				mandelbrot->setImage(y, x, iterations);
			}

			if (iterations == mandelbrot->getIterations())
			{
				// z didn't escape from the circle.
				// This point is in the Mandelbrot set.
				mandelbrot->setImage(y, x, 0x000000);
			}
		}
	}
}
