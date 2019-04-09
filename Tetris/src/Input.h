#pragma once

#include "Tetris.h"
#include "Screen.h"

class Input {
	Tetris gameState;
	Screen screenState;

	char inputBuffer[10];
	int  inputBufferCursor = 0;

public:
	Input(Tetris* gs, Screen* ss);

	void DebugCleanInputBuffer();

	void WatchInputs();

	void ProcessInputs();

private:
	// @AddInput : Add the input to the 'inputBuffer'
	// This buffer will be consumed during the next gameloop.
	void AddInput(char input);

	// @PopInput : Pop the next input.
	// Shift inputBuffer to the leaf after pop.
	//
	// @return next input
	// @return '#' if inutBuffer is empty
	char PopInput();

};
