// Tetris.cpp
#include "pch.h"
#include <iostream>
#include <Windows.h>

// Board definitions
const int LINES = 12;
const int COLUMNS = 12;
const int TOTAL_SIZE = LINES * COLUMNS;

// @cleanBoard : Reset all board fields to default state
void cleanBoard(char board[]) {
	int l, c;

	for (l = 0; l < LINES; l++)
		for (c = 0; c < COLUMNS; c++)
			board[l*COLUMNS + c] = '#';

	for (l = 0; l < LINES; l++) {
		for (c = 0; c < COLUMNS; c++) {
			std::cout << board[l*COLUMNS + c];
		}

		std::cout << "\n";
	}
}

// @renderBoard : Update console rendered content
int renderBoard(char board[]) {
	// Documentation at
	// https://docs.microsoft.com/en-us/windows/console/reading-and-writing-blocks-of-characters-and-attributes

	HANDLE hStdout, hNewScreenBuffer;
	SMALL_RECT srctReadRect;
	SMALL_RECT srctWriteRect;
	CHAR_INFO chiBuffer[160]; // [2][80]; 
	COORD coordBufSize;
	COORD coordBufCoord;
	BOOL fSuccess;

	// Create new screen
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	hNewScreenBuffer = CreateConsoleScreenBuffer(
		GENERIC_READ |           // read/write access 
		GENERIC_WRITE,
		FILE_SHARE_READ |
		FILE_SHARE_WRITE,        // shared 
		NULL,                    // default security attributes 
		CONSOLE_TEXTMODE_BUFFER, // must be TEXTMODE 
		NULL);                   // reserved; must be NULL

	// Make the new screen buffer the active screen buffer. 
	if (!SetConsoleActiveScreenBuffer(hNewScreenBuffer)) {
		printf("SetConsoleActiveScreenBuffer failed - (%d)\n", GetLastError());
		return 1;
	}

	// Set the source rectangle. 
	srctReadRect.Top = 0;    // top left: row 0, col 0 
	srctReadRect.Left = 0;
	srctReadRect.Bottom = 1; // bot. right: row 1, col 79 
	srctReadRect.Right = 79;
}

int main() {
	char board[TOTAL_SIZE];
	//cleanBoard(board);

	renderBoard(board);


	return 0;
}
