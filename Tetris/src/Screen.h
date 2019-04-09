#pragma once

#include <Windows.h>

class Screen
{
	Tetris gameState;

	// Screen properties
	int screenWide = 120;
	int screenHigh = 20;
	HANDLE hScreenGame;
	HANDLE hScreenConsole;
	HANDLE hScreenCurrent;

public:
	wchar_t* screen;
	wchar_t* overlay;
	
	// @Screen constructor
	// Create new game screen.
	Screen() {};
	Screen(Tetris* gs);

	// @EnableGameScreen : Switch to the game screen
	void EnableGameScreen();

	// @EnableConsoleScreen : Switch to the console debug screen
	void EnableConsoleScreen();

	// @ToggleScreen : Toggle between Console and Game screens
	void ToggleScreen();

	// @UpdateBoard : Update current board state and with overlays
	void UpdateBoard();

	// @Log : Very simple log to the console
	void Log(const char x[]);
};
