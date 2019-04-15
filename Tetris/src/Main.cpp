#include "pch.h"

#include "Tetris.h"
#include "Screen.h"
#include "Input.h"

#include "Renderer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"

#include <iostream>
#include <fstream>
#include <sstream>

// #include <GL/glew.h>
#include <GLFW/glfw3.h>

#define DEBUG(x) std::cout << x << std::endl;

#define RUN_PLAYGROUND 1



// OpenGL get shaders from files
struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};
static ShaderProgramSource ParseShader(const std::string& filepath) {
	std::ifstream stream(filepath);

	if (!stream.is_open())
	{
		std::cout << "[Error : ParseShader]: Shader '" << filepath << "' not found: " << std::endl;
	}

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else 
		{
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

// OpenGL shader functions
static unsigned int CompileShader(unsigned int type, const std::string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " <<
			(type == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< " shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}


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

	// Create the 'vao' : Vertex Array Object
	// compatibility with opengl 3.3.0 core
	unsigned int vao;
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao));

	{
		// vb : Vertex Buffer
		float positions[] = {
			-0.5f,  -0.5f, // 0
			 0.5f,  -0.5f, // 1
			 0.5f,   0.5f, // 2
			-0.5f,   0.5f  // 3
		};
		VertexBuffer vb(positions, 4 * 2 * sizeof(float));

		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

		// ib : Index Buffer Object
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};
		IndexBuffer ib(indices, 6);

		ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
		unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
		GLCall(glUseProgram(shader));

		int location = glGetUniformLocation(shader, "u_Color");
		ASSERT(location != -1);
		GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f));

		float r = 0.0f;
		float increment = 0.05f;
		// Loop until the user closes the window
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			glClear(GL_COLOR_BUFFER_BIT);

			// Draw the triangle from indices
			GLCall(glUseProgram(shader));
			GLCall(glUniform4f(location, r, r - 1.0f, 0.8f, 1.0f));
			ib.Bind();
			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
			if (r > 1.0f)
				increment = -0.05f;
			else if (r < 0.0f)
				increment = 0.05f;
			r += increment;

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}

		glDeleteProgram(shader); 
	}

	glfwTerminate();

	DEBUG("FINISHED PLAYGROUND");
}





#else
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
