#include "Mandelbrot.h"


Mandelbrot::Mandelbrot()
{
}

// Generating the mandelbrot set using the GPU. 
// This could be improved by running on multiple accelerators at once (i.e. the GPU and the integrated graphics of the CPU), and this can then be further improved by splitting the processing of the image into tasks, like with the CPU. I wasn't able to implement this due to time constraints.
void Mandelbrot::compute_mandelbrot_gpu(double left, double right, double top, double bottom, double zoom, double xPos, double yPos)
{
	// Creating a pointer to the image.
	uint32_t* pImage = &(image[0][0]);

	// Create an array view for the image, then clear it for writing to with the GPU.
	array_view<uint32_t, 2> a(HEIGHT, WIDTH, pImage);
	a.discard_data();

	// MAX_ITERATIONS can't be directly accessed from within the parallel_for_each function, so a new const int is created to be used in the function.
	const int max_iter = MAX_ITERATIONS;
	
	// For each point in the array view, it will determine whether it is within the mandelbrot set and assign the appropriate colour to that point. This is performed across many GPU threads concurrently.
	parallel_for_each(a.extent, [=](concurrency::index<2> idx) restrict(amp)
	{
			// Thread index for mapping to the complex plane.
			int x = idx[1];
			int y = idx[0];

			// Work out the point in the complex plane that
			// corresponds to this pixel in the output image.
			Complex c;
			c.x = xPos + (left + (x * (right - left) / WIDTH)) * zoom;
			c.y = yPos + (top + (y * (bottom - top) / HEIGHT)) * zoom;

			// Start off z at (0, 0).
			Complex z;
			z.x = 0.0f;
			z.y = 0.0f;

			// Iterate z = z^2 + c until z moves more than 2 units
			// away from (0, 0), or we've iterated too many times.
			int iterations = 0;
			while (c_abs(z) < 2.0 && iterations < max_iter)
			{
				z = c_add(c_mul(z, z), c);
				++iterations;

				// Set colour to equal the current iteration.
				// Simple way to make it a blue gradient.
				// iterations = 0 will give black, 255 will give blue, beyond that it's just brighter shades of blue. So closer to the mandelbrot set will be bright blue, far away dark blue.
				a[y][x] = iterations;
			}

			if (iterations == max_iter)
			{
				// z didn't escape from the circle.
				// This point is in the Mandelbrot set.
				a[y][x] = 0x000000; // black
			}
	});	
	a.synchronize();
}

// Thread function for computing a horizontal slice of the mandelbrot set. Works the same as a normal mandelbrot calculation, but the outside for loop is restricted to a specified range rather than being from the top of the image to the bottom.
void Mandelbrot::compute_mandelbrot_thread(double left, double right, double top, double bottom, double zoom, double xPos, double yPos, double start, double end)
{
	for (int y = start; y < end; ++y) // Compute within the specified horizontal slice.
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
			while (abs(z) < 2.0 && iterations < MAX_ITERATIONS)
			{
				z = (z * z) + c;
				++iterations;

				// Set colour to equal the current iteration.
				image[y][x] = iterations;
			}

			if (iterations == MAX_ITERATIONS)
			{
				// z didn't escape from the circle.
				// This point is in the Mandelbrot set.
				image[y][x] = 0x000000; // black
			}
		}
	}
}

// Function for computing the mandelbrot set using the above thread function.
void Mandelbrot::compute_mandelbrot_cpu(double left, double right, double top, double bottom, double zoom, double xPos, double yPos, int threads)
{
	// Create a vector of threads.
	std::vector<std::thread*> threadsVector;

	// Adds the specified number of threads to the vector, and passes the section that needs to be computed as the arguments.
	for (int i = 0; i < threads; i++)
	{
		threadsVector.push_back(new std::thread(&Mandelbrot::compute_mandelbrot_thread, this, left, right, top, bottom, zoom, xPos, yPos, i * HEIGHT / threads, (i + 1) * HEIGHT / threads));
	}

	// Waits for each of the threads to be completed, then deletes them.
	for (int i = 0; i < threads; i++)
	{
		threadsVector[i]->join();
		delete threadsVector[i];
	}
}

// Function for computing the mandelbrot set using a task farm. Instead of having as many slices as there is threads, this will improve performance by having more slices than there is threads and adding them to a task queue.
// This should provide a performance boost over the standard CPU method, as some slices take longer to calculate than others. This means that instead of just waiting for other threads after computing their slice, the thread will find a new slice to work on, reducing the time it takes to compute the set.
void Mandelbrot::compute_mandelbrot_task_farm(double left, double right, double top, double bottom, double zoom, double xPos, double yPos, int threads, int tasks)
{
	// Create a task farm, and pass through the number of threads that should be created.
	Farm f;
	f.set_threads(threads);

	// Create tasks for all of the slices of the set, then add them to the task farm.
	int slices = tasks;
	for (int i = 0; i < tasks; i++)
	{
		f.add_task(new MandelbrotTask(left, right, top, bottom, zoom, xPos, yPos, i * HEIGHT / slices, (i + 1) * HEIGHT / slices, this));
	}

	// Run the task farm.
	f.run();
}

// query if AMP accelerator exists on hardware
bool Mandelbrot::query_AMP_support()
{
	std::vector<accelerator> accls = accelerator::get_all();
	if (accls.empty())
	{
		std::cout << "No accelerators found that are compatible with C++ AMP" << std::endl;
		return false;
	}
	else
	{
		std::cout << "Accelerators found that are compatible with C++ AMP" << std::endl;
		list_accelerators();
		return true;
	}
	
} // query_AMP_support

// Write the image to a TGA file with the given name.
void Mandelbrot::write_tga(const char* filename)
{
	ofstream outfile(filename, ofstream::binary);

	uint8_t header[18] = {
		0, // no image ID
		0, // no colour map
		2, // uncompressed 24-bit image
		0, 0, 0, 0, 0, // empty colour map specification
		0, 0, // X origin
		0, 0, // Y origin
		WIDTH & 0xFF, (WIDTH >> 8) & 0xFF, // width
		HEIGHT & 0xFF, (HEIGHT >> 8) & 0xFF, // height
		24, // bits per pixel
		0, // image descriptor
	};
	outfile.write((const char*)header, 18);

	for (int y = 0; y < HEIGHT; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			uint8_t pixel[3] = {
				image[y][x] & 0xFF, // blue channel
				(image[y][x] >> 8) & 0xFF, // green channel
				(image[y][x] >> 16) & 0xFF, // red channel
			};
			outfile.write((const char*)pixel, 3);
		}
	}

	outfile.close();
	if (!outfile)
	{
		// An error has occurred at some point since we opened the file.
		//cout << "Error writing to " << filename << endl;
		//exit(1);
	}
}

void Mandelbrot::report_accelerator(const accelerator a)
{
	const std::wstring bs[2] = { L"false", L"true" };
	std::wcout << ": " << a.description << " "
		<< std::endl << "       device_path                       = " << a.device_path
		<< std::endl << "       dedicated_memory                  = " << std::setprecision(4) << float(a.dedicated_memory) / (1024.0f * 1024.0f) << " Mb"
		<< std::endl << "       has_display                       = " << bs[a.has_display]
		<< std::endl << "       is_debug                          = " << bs[a.is_debug]
		<< std::endl << "       is_emulated                       = " << bs[a.is_emulated]
		<< std::endl << "       supports_double_precision         = " << bs[a.supports_double_precision]
		<< std::endl << "       supports_limited_double_precision = " << bs[a.supports_limited_double_precision]
		<< std::endl;
}

// List and select the accelerator to use
void Mandelbrot::list_accelerators()
{
	//get all accelerators available to us and store in a vector so we can extract details
	std::vector<accelerator> accls = accelerator::get_all();

	// iterates over all accelerators and print characteristics
	for (int i = 0; i < accls.size(); i++)
	{
		accelerator a = accls[i];
		report_accelerator(a);
	}

	//Use default accelerator
	accelerator a = accelerator(accelerator::default_accelerator);
	std::wcout << " default acc = " << a.description << std::endl;
} // list_accelerators


