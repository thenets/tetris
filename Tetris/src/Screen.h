#pragma once

#include <iostream>
#include <chrono>
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
	
	Screen() {};
	Screen(Tetris* gs);

	~Screen();

	void SwitchToGameScreen();

	void SwitchToConsoleScreen();

	// @ToggleScreen : Toggle between Console and Game screens
	void ToggleScreen();

	// @UpdateBoard : Update current board state including the overlays
	void UpdateBoard();

	void Log(const char x[]);
};
