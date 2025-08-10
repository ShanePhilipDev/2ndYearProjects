#include "Level.h"

Level::Level(sf::RenderWindow* hwnd, Input* in)
{
	window = hwnd;
	input = in;

	// Mandelbrot is handled by creating a new object then pointing to it.
	mandelbrot = new Mandelbrot();

	// Set default values.
	moveIncrement = 0.05;
	zoomIncrement = 0.1;
	mandelbrot->setIterations(500);
	cpuThreads = 16;
	taskCount = 200;
	reset();
	generatingResults = false;
	mandelbrotTime = 0.0;
	
	// Check for AMP support then set the mode to either GPU or CPU.
	if (mandelbrot->query_AMP_support())
	{
		gpuMode = true;
	}
	else
	{
		gpuMode = false;
	}
	
	// Generate the initial mandelbrot image and output to 'output.tga'.
	computeMandelbrot();

	// Load the texture from file, assign it to the mandelbrot object, and position the mandelbrot within the window.
	mandelbrotTexture.loadFromFile("output.tga");
	mandelbrot->setTexture(&mandelbrotTexture);
	mandelbrot->setSize(sf::Vector2f(1440, 900));
	mandelbrot->setPosition(380, 90);
	mandelbrot->setCollisionBox(0, 0, mandelbrot->getSize().x, mandelbrot->getSize().y);
	
	// SFML - Setting up the UI.
	// ----- //
	font.loadFromFile("font/arial.ttf");
	
	crosshairTexture.loadFromFile("gfx/crosshair.png");
	arrowTextures[0].loadFromFile("gfx/arrowL.png");
	arrowTextures[1].loadFromFile("gfx/arrowR.png");
	arrowTextures[2].loadFromFile("gfx/arrowU.png");
	arrowTextures[3].loadFromFile("gfx/arrowD.png");
	plusTexture.loadFromFile("gfx/plus.png");
	minusTexture.loadFromFile("gfx/minus.png");
	resetTexture.loadFromFile("gfx/reset.png");

	crosshair.setTexture(&crosshairTexture);
	crosshair.setSize(sf::Vector2f(50, 50));
	crosshair.setPosition(mandelbrot->getPosition().x + 0.5 * mandelbrot->getSize().x - 0.5 * crosshair.getSize().x, mandelbrot->getPosition().y + 0.5 * mandelbrot->getSize().y - 0.5 * crosshair.getSize().y);

	cursor.setSize(sf::Vector2f(1, 1));
	cursor.setPosition(0, 0);
	cursor.setCollisionBox(0, 0, cursor.getSize().x, cursor.getSize().y);

	leftButton.setSize(sf::Vector2f(100, 100));
	leftButton.setPosition(25, 250);
	leftButton.setCollisionBox(0, 0, leftButton.getSize().x, leftButton.getSize().y);
	leftButton.setTexture(&arrowTextures[0]);
	leftButton.setOutlineColor(sf::Color::Black);
	leftButton.setOutlineThickness(2);

	rightButton.setSize(sf::Vector2f(100, 100));
	rightButton.setPosition(250, 250);
	rightButton.setCollisionBox(0, 0, rightButton.getSize().x, rightButton.getSize().y);
	rightButton.setTexture(&arrowTextures[1]);
	rightButton.setOutlineColor(sf::Color::Black);
	rightButton.setOutlineThickness(2);

	upButton.setSize(sf::Vector2f(100, 100));
	upButton.setPosition(137.5, 190);
	upButton.setCollisionBox(0, 0, upButton.getSize().x, upButton.getSize().y);
	upButton.setTexture(&arrowTextures[2]);
	upButton.setOutlineColor(sf::Color::Black);
	upButton.setOutlineThickness(2);

	downButton.setSize(sf::Vector2f(100, 100));
	downButton.setPosition(137.5, 310);
	downButton.setCollisionBox(0, 0, downButton.getSize().x, downButton.getSize().y);
	downButton.setTexture(&arrowTextures[3]);
	downButton.setOutlineColor(sf::Color::Black);
	downButton.setOutlineThickness(2);

	zoomInButton.setSize(sf::Vector2f(100, 100));
	zoomInButton.setPosition(250, 460);
	zoomInButton.setCollisionBox(0, 0, zoomInButton.getSize().x, zoomInButton.getSize().y);
	zoomInButton.setTexture(&plusTexture);
	zoomInButton.setOutlineColor(sf::Color::Black);
	zoomInButton.setOutlineThickness(2);

	zoomOutButton.setSize(sf::Vector2f(100, 100));
	zoomOutButton.setPosition(25, 460);
	zoomOutButton.setCollisionBox(0, 0, zoomOutButton.getSize().x, zoomOutButton.getSize().y);
	zoomOutButton.setTexture(&minusTexture);
	zoomOutButton.setOutlineColor(sf::Color::Black);
	zoomOutButton.setOutlineThickness(2);

	resetButton.setSize(sf::Vector2f(100, 100));
	resetButton.setPosition(137.5, 460);
	resetButton.setCollisionBox(0, 0, resetButton.getSize().x, resetButton.getSize().y);
	resetButton.setTexture(&resetTexture);
	resetButton.setOutlineColor(sf::Color::Black);
	resetButton.setOutlineThickness(2);

	outputWindow.setSize(sf::Vector2f(325, 400));
	outputWindow.setPosition(25, 600);
	outputWindow.setOutlineColor(sf::Color::Black);
	outputWindow.setOutlineThickness(2);

	text.setFont(font);
	text.setCharacterSize(18);
	text.setFillColor(sf::Color::Black);
	
	text.setPosition(outputWindow.getPosition());
	// ----- //
}

Level::~Level()
{

}

// handle user input
void Level::handleInput(float dt)
{
	// User input handled using SFML. If valid input is received, it will call the relevant function for manipulating the mandelbrot and then recalculate the set.
	// ----- //
	cursor.setPosition(input->getMouseX(), input->getMouseY());
	if (!generatingResults) // Can't input while generating results file as this would result in multiple mandelbrots being calculated at the same time, writing to the same image array.
	{
		if (input->isMouseLDown())
		{
			if (Collision::checkBoundingBox(&cursor, &leftButton))
			{
				moveLeft();
				computeMandelbrot();
			}
			else if (Collision::checkBoundingBox(&cursor, &rightButton))
			{
				moveRight();
				computeMandelbrot();
			}
			else if (Collision::checkBoundingBox(&cursor, &upButton))
			{
				moveUp();
				computeMandelbrot();
			}
			else if (Collision::checkBoundingBox(&cursor, &downButton))
			{
				moveDown();
				computeMandelbrot();
			}
			else if (Collision::checkBoundingBox(&cursor, &zoomInButton))
			{
				zoomIn();
				computeMandelbrot();
			}
			else if (Collision::checkBoundingBox(&cursor, &zoomOutButton))
			{
				zoomOut();
				computeMandelbrot();
			}
			else if (Collision::checkBoundingBox(&cursor, &resetButton))
			{
				reset();
				computeMandelbrot();
			}
		}

		if (input->isKeyDown(sf::Keyboard::A) || input->isKeyDown(sf::Keyboard::Left))
		{
			moveLeft();
			computeMandelbrot();
		}
		else if (input->isKeyDown(sf::Keyboard::D) || input->isKeyDown(sf::Keyboard::Right))
		{
			moveRight();
			computeMandelbrot();
		}
		else if (input->isKeyDown(sf::Keyboard::W) || input->isKeyDown(sf::Keyboard::Up))
		{
			moveUp();
			computeMandelbrot();
		}
		else if (input->isKeyDown(sf::Keyboard::S) || input->isKeyDown(sf::Keyboard::Down))
		{
			moveDown();
			computeMandelbrot();
		}
		else if (input->isKeyDown(sf::Keyboard::Q) || input->isKeyDown(sf::Keyboard::Subtract))
		{
			zoomOut();
			computeMandelbrot();
		}
		else if (input->isKeyDown(sf::Keyboard::E) || input->isKeyDown(sf::Keyboard::Add))
		{
			zoomIn();
			computeMandelbrot();
		}
		else if (input->isKeyDown(sf::Keyboard::R))
		{
			reset();
			computeMandelbrot();
		}
	}
	
	// Close window on Escape pressed.
	if (input->isKeyDown(sf::Keyboard::Escape))
	{
		window->close();
	}
	// ----- //
}

// Update game objects
void Level::update(float dt)
{
	// Sets the string to be displayed on the window, containing the controls and other relevant information.
	if (gpuMode)
	{
		text.setString("Controls:\nW/Up - Move Up \nS/Down - Move Down \nA/Left - Move Left \nD/Right - Move Right \nQ/Minus - Zoom Out \nE/Plus - Zoom In \nR - Reset \nSee console for commands\n\nCPU/GPU mode: GPU\nMax iterations: " + std::to_string(mandelbrot->getIterations()) 
			+ "\nZoom increment: " + std::to_string(zoomIncrement) 
			+ "\nMovement increment: " + std::to_string(moveIncrement) 
			+ "\nThread count: " + std::to_string(getThreads()) 
			+ "\nTask count: " + std::to_string(getTasks()) 
			+ "\n\nTime taken to compute last mandelbrot:\n" + std::to_string(mandelbrotTime) + "ms");
	}
	else
	{
		text.setString("Controls:\nW/Up - Move Up \nS/Down - Move Down \nA/Left - Move Left \nD/Right - Move Right \nQ/Minus - Zoom Out \nE/Plus - Zoom In \nR - Reset \nSee console for commands\n\nCPU/GPU mode: CPU\nMax iterations: " + std::to_string(mandelbrot->getIterations()) 
			+ "\nZoom increment: " + std::to_string(zoomIncrement) 
			+ "\nMovement increment: " + std::to_string(moveIncrement) 
			+ "\nThread count: " + std::to_string(getThreads()) 
			+ "\nTask count: " + std::to_string(getTasks()) 
			+ "\n\nTime taken to compute last mandelbrot:\n" + std::to_string(mandelbrotTime) + "ms");
	}
	
}

// Render level
void Level::render()
{
	// SFML - draw the mandelbrot set and UI to the window.
	// ----- //
	beginDraw();

	window->draw(*mandelbrot);
	window->draw(crosshair);
	window->draw(leftButton);
	window->draw(rightButton);
	window->draw(upButton);
	window->draw(downButton);
	window->draw(zoomInButton);
	window->draw(zoomOutButton);
	window->draw(resetButton);
	window->draw(outputWindow);
	window->draw(text);

	endDraw();
	// ----- //
}



// clear back buffer
void Level::beginDraw()
{
	// Sets background colour - grey in this case.
	window->clear(sf::Color(200, 200, 200));
}

// Ends rendering to the back buffer, and swaps buffer to the screen.
void Level::endDraw()
{
	window->display();
}

void Level::computeMandelbrot()
{
	// Set lock while computing mandelbrot.
	std::unique_lock<std::mutex> lock(mandelbrotMutex);
	computingMandelbrot = true;
	mandelbrotCV.notify_all();

	// Start timing
	the_clock::time_point start = the_clock::now();

	// Compute mandelbrot, passing in the required parameters and calling the specific function depending on the current mode.
	if (gpuMode)
	{
		mandelbrot->compute_mandelbrot_gpu(left, right, top, bottom, zoom, xPos, yPos);
	}
	else
	{
		// calculate cpu
		//mandelbrot->compute_mandelbrot_cpu(left, right, top, bottom, zoom, xPos, yPos, cpuThreads);
		mandelbrot->compute_mandelbrot_task_farm(left, right, top, bottom, zoom, xPos, yPos, cpuThreads, taskCount);
	}
	
	// Stop timing
	the_clock::time_point end = the_clock::now();

	// Compute the difference between the two times in milliseconds
	auto time_taken = duration_cast<milliseconds>(end - start).count();

	//cout << "Computing the Mandelbrot set took " << time_taken << " ms." << endl;

	// Capture the time taken to calculate the mandelbrot, write it to the output file and reset the texture.
	mandelbrotTime = time_taken;
	mandelbrot->write_tga("output.tga");
	mandelbrotTexture.loadFromFile("output.tga");

	// Unlock once mandelbrot is finished computing.
	lock.unlock();
	mandelbrotCV.notify_all();
	computingMandelbrot = false;
}

// Functions for controlling the movement of the mandelbrot. Increases or decreases the centre position based on the user's specified increment and the current zoom level.
// ----- //
void Level::moveLeft()
{
	xPos -= moveIncrement * zoom;
}

void Level::moveRight()
{
	xPos += moveIncrement * zoom;
}

void Level::moveUp()
{
	yPos -= moveIncrement * zoom;
}

void Level::moveDown()
{
	yPos += moveIncrement * zoom;
}
// ----- //

// Function for increasing the zoom.
void Level::zoomIn()
{
	zoom *= 1 - zoomIncrement;
}

// Function for decreasing the zoom.
void Level::zoomOut()
{
	zoom *= 1 + zoomIncrement;
}

// Reset the mandelbrot parameters.
void Level::reset()
{
	left = -2.0;
	right = 2.0;
	top = 1.125;
	bottom = -1.125;

	zoom = 1.0;

	xPos = 0.0;
	yPos = 0.0;
}

void Level::processResults(int x)
{
	// Set bool to true for generating results. This is used to prevent unwanted input during the process.
	generatingResults = true;

	// Reset values of the previously computed mandelbrot so that it doesn't affect the results.
	reset();

	// Create a results file to write to.
	std::ofstream resultsFile("results.csv");

	// Save the values for number of iterations, threads and tasks.
	int prevIterations = mandelbrot->getIterations();
	int prevThreads = cpuThreads;
	int prevTasks = taskCount;

	// Write to results file. Writes column headings, adjusts parameters, and generates the mandelbrot set which is then timed and written to file. Repeats for several stages.
	// ----- //
	resultsFile << "(All times in milliseconds)\n\n";
	resultsFile << "Coordinates:,-2.0,1.0,1.125,-1.125\nIterations:,500\n\n";
	mandelbrot->setIterations(500);
	generateResults(&resultsFile, -2.0, 1.0, 1.125, -1.125, x);
	std::cout << "Stage 1/7 complete.\n";
	
	resultsFile << "\nIterations:,1000\n\n";
	mandelbrot->setIterations(1000);
	generateResults(&resultsFile, -2.0, 1.0, 1.125, -1.125, x);
	std::cout << "Stage 2/7 complete.\n";

	resultsFile << "\nIterations:,2000\n\n";
	mandelbrot->setIterations(2000);
	generateResults(&resultsFile, -2.0, 1.0, 1.125, -1.125, x);
	std::cout << "Stage 3/7 complete.\n";

	resultsFile << "\nCoordinates:,-0.751085,-0.734975,0.118378,0.134488\nIterations:,500\n\n";
	mandelbrot->setIterations(500);
	generateResults(&resultsFile, -0.751085, -0.734975, 0.118378, 0.134488, x);
	std::cout << "Stage 4/7 complete.\n";

	resultsFile << "\nIterations:,1000\n\n";
	mandelbrot->setIterations(1000);
	generateResults(&resultsFile, -0.751085, -0.734975, 0.118378, 0.134488, x);
	std::cout << "Stage 5/7 complete.\n";

	resultsFile << "\nIterations:,2000\n\n";
	mandelbrot->setIterations(2000);
	generateResults(&resultsFile, -0.751085, -0.734975, 0.118378, 0.134488, x);
	std::cout << "Stage 6/7 complete.\n";

	the_clock::time_point start = the_clock::now();
	the_clock::time_point end = the_clock::now();
	auto time_taken = duration_cast<milliseconds>(end - start).count();

	resultsFile << "\nWrite to TGA file\n";
	for (int i = 0; i < x; i++)
	{
		the_clock::time_point start = the_clock::now();
		mandelbrot->write_tga("output.tga");
		end = the_clock::now();
		time_taken = duration_cast<milliseconds>(end - start).count();
		resultsFile << time_taken << "\n";
	}
	std::cout << "Stage 7/7 complete.\n";
	// ----- //

	// Reset values and set generating results to false.
	mandelbrot->setIterations(prevIterations);
	cpuThreads = prevThreads;
	taskCount = prevTasks;
	reset();
	generatingResults = false;
}

// Function to generate results and write them to the specified file. Parameters for a pointer to the file, the co-ordinates of the mandelbrot, and the amount of times you want to test each set of values.
void Level::generateResults(std::ofstream* results, double l, double r, double t, double b, int testTimes)
{
	the_clock::time_point start = the_clock::now();
	the_clock::time_point end = the_clock::now();
	auto time_taken = duration_cast<milliseconds>(end - start).count();

	*results << "GPU\n";

	for (int i = 0; i < testTimes; i++)
	{
		start = the_clock::now();
		mandelbrot->compute_mandelbrot_gpu(l, r, t, b, zoom, xPos, yPos);
		end = the_clock::now();
		time_taken = duration_cast<milliseconds>(end - start).count();
		*results << time_taken << "\n";
	}

	*results << "\nCPU - 4 Threads,CPU - 8 Threads,CPU - 16 threads\n";
	for (int i = 0; i < testTimes; i++)
	{
		start = the_clock::now();
		mandelbrot->compute_mandelbrot_cpu(l, r, t, b, zoom, xPos, yPos, 4);
		end = the_clock::now();
		time_taken = duration_cast<milliseconds>(end - start).count();
		*results << time_taken << ",";

		start = the_clock::now();
		mandelbrot->compute_mandelbrot_cpu(l, r, t, b, zoom, xPos, yPos, 8);
		end = the_clock::now();
		time_taken = duration_cast<milliseconds>(end - start).count();
		*results << time_taken << ",";

		start = the_clock::now();
		mandelbrot->compute_mandelbrot_cpu(l, r, t, b, zoom, xPos, yPos, 16);
		end = the_clock::now();
		time_taken = duration_cast<milliseconds>(end - start).count();
		*results << time_taken << "\n";
	}

	*results << "\nCPU - 4 Threads,,,,CPU - 8 Threads,,,,CPU - 16 Threads\n";
	*results << "16 tasks,50 tasks,200 tasks,,16 tasks,50 tasks,200 tasks,,16 tasks,50 tasks,200 tasks,\n";

	for (int i = 0; i < testTimes; i++)
	{
		start = the_clock::now();
		mandelbrot->compute_mandelbrot_task_farm(l, r, t, b, zoom, xPos, yPos, 4, 16);
		end = the_clock::now();
		time_taken = duration_cast<milliseconds>(end - start).count();
		*results << time_taken << ",";

		start = the_clock::now();
		mandelbrot->compute_mandelbrot_task_farm(l, r, t, b, zoom, xPos, yPos, 4, 50);
		end = the_clock::now();
		time_taken = duration_cast<milliseconds>(end - start).count();
		*results << time_taken << ",";

		start = the_clock::now();
		mandelbrot->compute_mandelbrot_task_farm(l, r, t, b, zoom, xPos, yPos, 4, 200);
		end = the_clock::now();
		time_taken = duration_cast<milliseconds>(end - start).count();
		*results << time_taken << ",,";

		start = the_clock::now();
		mandelbrot->compute_mandelbrot_task_farm(l, r, t, b, zoom, xPos, yPos, 8, 16);
		end = the_clock::now();
		time_taken = duration_cast<milliseconds>(end - start).count();
		*results << time_taken << ",";

		start = the_clock::now();
		mandelbrot->compute_mandelbrot_task_farm(l, r, t, b, zoom, xPos, yPos, 8, 50);
		end = the_clock::now();
		time_taken = duration_cast<milliseconds>(end - start).count();
		*results << time_taken << ",";

		start = the_clock::now();
		mandelbrot->compute_mandelbrot_task_farm(l, r, t, b, zoom, xPos, yPos, 8, 200);
		end = the_clock::now();
		time_taken = duration_cast<milliseconds>(end - start).count();
		*results << time_taken << ",,";

		start = the_clock::now();
		mandelbrot->compute_mandelbrot_task_farm(l, r, t, b, zoom, xPos, yPos, 16, 16);
		end = the_clock::now();
		time_taken = duration_cast<milliseconds>(end - start).count();
		*results << time_taken << ",";

		start = the_clock::now();
		mandelbrot->compute_mandelbrot_task_farm(l, r, t, b, zoom, xPos, yPos, 16, 50);
		end = the_clock::now();
		time_taken = duration_cast<milliseconds>(end - start).count();
		*results << time_taken << ",";

		start = the_clock::now();
		mandelbrot->compute_mandelbrot_task_farm(l, r, t, b, zoom, xPos, yPos, 16, 200);
		end = the_clock::now();
		time_taken = duration_cast<milliseconds>(end - start).count();
		*results << time_taken << "\n";
	}
}



