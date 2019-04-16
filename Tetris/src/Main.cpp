#include "pch.h"

#define DEBUG(x) std::cout << x << std::endl;

#include "Renderer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

// OPENGL PLAYGROUND
#define RUN_PLAYGROUND 1

#if RUN_PLAYGROUND
int main() {
	// Create GLFW window and OpenGL context
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	// Set OpenGL version 3.3.0 core
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(640, 480, "Tetris Exodus", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Syncronize refresh rate with monitor (v-sync)
	glfwSwapInterval(1);

	// Initialize GLEW
	auto err = glewInit();
	if (err != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return 1;
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	printf("        OpenGL %s\n", glGetString(GL_VERSION));


	{
		// vb : Vertex Buffer
		float positions[] = {
			-0.5f,  -0.5f, // 0
			 0.5f,  -0.5f, // 1
			 0.5f,   0.5f, // 2
			-0.5f,   0.5f  // 3
		};

		VertexArray va;
		VertexBuffer vb(positions, 4 * 2 * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);
		
		//GLCall(glEnableVertexAttribArray(0));
		//GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

		// ib : Index Buffer Object
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};
		IndexBuffer ib(indices, 6);

		// Shader
		Shader shader("res/shaders/Basic.shader");		
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

		va.Unbind();
		shader.Unbind();
		vb.Unbind();
		ib.Unbind();	

		Renderer renderer;

		float r = 0.0f;
		float increment = 0.20f;
		// Loop until the user closes the window
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			renderer.Clear();

			// Draw the triangle from indices
			shader.Bind();
			shader.SetUniform4f("u_Color", r, r - 1.0f, 0.8f, 1.0f);

			va.Bind();
			ib.Bind();

			renderer.Draw(va, ib, shader);

			if (r > 1.0f)
				increment = -0.05f;
			else if (r < 0.0f)
				increment = 0.18f;
			r += increment;

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}

	glfwTerminate();

	DEBUG("FINISHED PLAYGROUND");
}





// TETRIS MAIN CODE
#else
#include "Tetris.h"
#include "Screen.h"
#include "Input.h"


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

#endif
