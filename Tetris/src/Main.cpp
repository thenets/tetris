#include "pch.h"
#include "Tetris.h"
#include "Screen.h"
#include "Input.h"	
#include <iostream>
#include <chrono>
#include <thread>
#include "GLFW/glfw3.h"

#define DEBUG(x) std::cout << x << std::endl;

#if 1
int main() {
	// Main vars
	Tetris gameState;
	Screen screen(&gameState);
	Input input(&gameState, &screen);
	screen.Log("Game first state started.");

	// Main gameloop
	screen.Log("Start gameloop.");
	gameState.DebugBoard();
	screen.SwitchToGameScreen();

	while (1) {
		// Get inputs
		input.WatchInputs();

		// Update game state
		input.ProcessInputs();
		gameState.UpdateTetrominoState();

		// Update game screen
		screen.UpdateBoard();

		// Pause
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}

	// Finished
	DEBUG("FINISHED APP");
}

#else

int main() {
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();

	DEBUG("FINISHED SANDBOX");
}


#endif