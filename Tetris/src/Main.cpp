#include "pch.h"

#include "Tetris.h"
#include "Screen.h"
#include "Input.h"

#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define DEBUG(x) std::cout << x << std::endl;

#if 0
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
	// Create GLFW window and OpenGL context
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Initialize GLEW
	auto err = glewInit();
	if (err != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return 1;
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	printf("        OpenGL %s\n", glGetString(GL_VERSION));


	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		// Triangle
		glBegin(GL_TRIANGLES);
		glVertex2f(-0.15, 0);
		glVertex2f(0, 0.3);
		glVertex2f(0.15, 0);
		glEnd();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();

	DEBUG("FINISHED SANDBOX");
}


#endif