#include "pch.h"
#include "Tetris.h"
#include "Screen.h"
#include "Input.h"
#include <iostream>
#include <chrono>
#include <thread>

#define DEBUG(x) std::cout << x << std::endl;

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
	DEBUG("FINISHED");

	return 0;
}
