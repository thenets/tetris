#include "pch.h"
#include "Input.h"
#include "Tetris.h"
#include "Screen.h"
#include <iostream>
#include <chrono>


Input::Input(Tetris* gs, Screen* ss) {
	gameState = *gs;
	screenState = *ss;
}

void Input::DebugCleanInputBuffer() {
	// Pop each input from buffer
	char out = PopInput();
	while (out != '#') {
		std::cout << out;
		out = PopInput();
	}
	std::cout << std::endl;
}

void Input::WatchInputs() {
	// Update if elapse > 0.032 ms (equal to 30 fps)
	static auto start = std::chrono::high_resolution_clock::now();
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	if (elapsed.count() < 0.120)
		return;
	start = std::chrono::high_resolution_clock::now();

	if (GetAsyncKeyState(VK_LEFT))
		AddInput('l');

	if (GetAsyncKeyState(VK_RIGHT))
		AddInput('r');

	if (GetAsyncKeyState(VK_UP))
		AddInput('u');

	if (GetAsyncKeyState(VK_DOWN))
		AddInput('d');

	if (GetAsyncKeyState(VK_SPACE))
		AddInput('s');

	if (GetAsyncKeyState(VK_RETURN))
		AddInput('t');

	if (GetAsyncKeyState(VK_ESCAPE)) {
		AddInput('e');
		exit(0);
	}
}

void Input::ProcessInputs() {
	char input = PopInput();
	while (input != '#') {
		switch (input) {

		case 'l': // Move to the left
			gameState.MoveToLeft();
			break;

		case 'r': // Move to the right
			gameState.MoveToRight();
			break;

		case 'u': // Rotate
			gameState.Rotate();
			break;

		case 'd': // Move down
			screenState.screen[3]++;
			break;

		case 's': // Toggle console and game screens
			screenState.ToggleScreen();
			break;

		case 't': // DEBUG change tetromino shape
			gameState.tetromino->shape = rand() % Tetris::GetTetrominoShapes().size();
			break;

		default:
			break;
		}

		// Pop next input
		input = PopInput();
	}
}

// @AddInput : Add the input to the 'inputBuffer'
// This buffer will be consumed during the next gameloop.
void Input::AddInput(char input) {
	if (inputBufferCursor < 9) {
		inputBuffer[inputBufferCursor] = input;
		inputBufferCursor++;
	}
}

// @PopInput : Pop the next input.
// Shift inputBuffer to the leaf after pop.
//
// @return next input
// @return '#' if inutBuffer is empty
char Input::PopInput() {
	char out;
	if (inputBufferCursor > 0) {
		out = inputBuffer[0];

		for (int i = 0; i < inputBufferCursor; i++)
			inputBuffer[i] = inputBuffer[i + 1];

		inputBufferCursor--;
		return out;
	}
	else {
		return '#';
	}
}
