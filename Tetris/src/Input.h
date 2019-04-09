#ifndef _INPUT_H_
#define _INPUT_H_

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
	void AddInputToBuffer(char input);

	// @PopInputFromBuffer : Pop the next input.
	// Shift inputBuffer to the left after pop.
	//
	// @return next input
	// @return '#' if inutBuffer is empty
	char PopInputFromBuffer();
};

#endif