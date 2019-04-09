#include "pch.h"
#include <iostream>
#include <chrono>
#include <Windows.h>
#include "Tetris.h"
#include "Screen.h"

// -----------------------------------------------------------------
// Screen
// -----------------------------------------------------------------

// @Screen constructor
// Create new game screen.
Screen::Screen(Tetris* gs) {
	// Get game state
	gameState = *gs;

	// Save current stdout screen
	hScreenConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// Create overlay buffer
	overlay = new wchar_t[screenWide * screenHigh];

	// Create Screen Buffer
	screen = new wchar_t[screenWide * screenHigh];
	hScreenGame = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	for (int i = 0; i < screenWide * screenHigh - 1; i++)
		screen[i] = ' ';
	screen[screenWide * screenHigh - 1] = '\0';
}

// @EnableGameScreen : Switch to the game screen
void Screen::EnableGameScreen() {
	hScreenCurrent = hScreenGame;
	SetConsoleActiveScreenBuffer(hScreenCurrent);
}

// @EnableConsoleScreen : Switch to the console debug screen
void Screen::EnableConsoleScreen() {
	hScreenCurrent = hScreenConsole;
	SetConsoleActiveScreenBuffer(hScreenCurrent);
}

// @ToggleScreen : Toggle between Console and Game screens
void Screen::ToggleScreen() {
	if (hScreenCurrent == hScreenConsole)
		EnableGameScreen();
	else
		EnableConsoleScreen();
}

// @UpdateBoard : Update current board state and with overlays
void Screen::UpdateBoard() {
	// Update if elapse > 0.032 ms (equal to 30 fps)
	static auto start = std::chrono::high_resolution_clock::now();
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	if (elapsed.count() < 0.016)
		return;
	start = std::chrono::high_resolution_clock::now();

	// Update screen size if was resized
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hScreenGame, &csbi);
	int newWide = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	if (newWide != screenWide) {
		screenWide = newWide;
		overlay = new wchar_t[(screenWide * screenHigh)];
		screen = new wchar_t[(screenWide * screenHigh)];
	}

	// Screen vars
	DWORD dwBytesWritten = 0;
	int screenTotalSize = screenWide * screenHigh;

	// Clean screen and overlay buffer
	for (int i = 0; i < screenTotalSize - 1; i++) {
		overlay[i] = ' ';
		screen[i] = ' ';
	}

	// Add tetromino to the overlay
	for (int i = 0; i < 16; i++) {
		if (gameState.GetTetrominoShape()[i] != '-') {
			int tetrominoLine = i / 4;
			int tetromino_i = gameState.tetromino->x + (i % 4) + (tetrominoLine * screenWide);

			overlay[gameState.tetromino->y * screenWide + tetromino_i] = 'X';
		}
	}

	// Add gameboard to the screen
	wchar_t boardBg = '.';
	for (int i = 0; i < screenTotalSize - 1; i++) {
		int screenLine = i / screenWide;
		int screenColunm = i - screenLine * screenWide;

		// check if line and colunm exist in board
		if (screenLine < gameState.b_high &&
			screenColunm < gameState.b_wide)
		{
			// update overlay content here
			int boardIndex = screenLine * gameState.b_wide + screenColunm;

			if (gameState.board[boardIndex] != ' ') {
				screen[screenLine * screenWide + screenColunm] = gameState.board[boardIndex];
			}
			else {
				screen[screenLine * screenWide + screenColunm] = boardBg;
			}
		}
		// Skip useless screen points to improve performance
		else
		{
			i = i + screenWide - gameState.b_wide - 1;
		}

	}


	// Add overlay to the screen
	for (int i = 0; i < screenTotalSize - 1; i++) {
		if (overlay[i] != ' ') {
			screen[i] = overlay[i];
		}
	}

	WriteConsoleOutputCharacter(
		hScreenGame,
		screen,
		screenTotalSize,
		{ 0, 0 },
		&dwBytesWritten
	);
}

// @Log : Very simple log to the console
void Screen::Log(const char x[]) {
	HANDLE current = hScreenCurrent;
	SetConsoleActiveScreenBuffer(hScreenConsole);
	std::cout << x << std::endl;
	SetConsoleActiveScreenBuffer(current);
}
