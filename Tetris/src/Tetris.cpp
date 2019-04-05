// Tetris.cpp
#include "pch.h"
#include "Tetris.h"
#include <Windows.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <chrono>

#define DEBUG(x) std::cout << x << std::endl;

// -----------------------------------------------------------------
// Tetrominos
// -----------------------------------------------------------------
std::vector<std::vector<std::string>> TetrominoShapes = {
	// bar
	std::vector<std::string> {
			"x---x---x---x---",
			"xxxx------------",
			"x---x---x---x---",
			"xxxx------------"},

	// cube
	std::vector<std::string> {
			"xx--xx----------",
			"xx--xx----------",
			"xx--xx----------",
			"xx--xx----------"},

	// J
	std::vector<std::string> {
			"-x---x---x--xx--",
			"x---xxxx--------",
			"xx--x---x---x---",
			"xxxx---x--------"},

	// L
	std::vector<std::string> {
			"x---x---x---xx--",
			"xxxxx-----------",
			"xx---x---x---x--",
			"---xxxxx--------"},

	// S
	std::vector<std::string> {
			"-xx-xx----------",
			"x---xx---x------",
			"-xx-xx----------",
			"x---xx---x------"},

	// Z
	std::vector<std::string> {
			"xx---xx---------",
			"-x--xx--x-------",
			"xx---xx---------",
			"-x--xx--x-------"},

	// T
	std::vector<std::string> {
			"xxx--x----------",
			"-x--xx---x------",
			"-x--xxx---------",
			"x---xx--x-------"}
};

struct Tetromino {
	int x = 0;
	int y = 2;

	// Possible values from 'TetrominoShapes' vector
	int shape = 0;

	// 0: default; 1: 90o; 2:180o; 3:270o
	int rotation = 0;

	// 0 or 1
	int active = 0;
};


// -----------------------------------------------------------------
// Tetris
// -----------------------------------------------------------------
class Tetris {

public:
	// Board properties
	int b_high = 20;
	int b_wide = 10;

	// Main game board
	wchar_t* board;

	// Does exist only one Tetramino at time
	Tetromino* tetromino = new Tetromino();

	// Helpers
	BOOL debug = true;
	int cursor_x = 0;
	int cursor_y = 0;

	// @Tetris constructor
	// Create game board and prepare the initial game state.
	Tetris() {
		// Create board
		board = new wchar_t[b_high * b_wide];

		CleanBoard();
	}

	// @debugBoard : Print current board state
	void DebugBoard() {
		// each line
		for (int l = 0; l < b_high; l++) {
			// each column
			for (int c = 0; c < b_wide; c++) {
				std::wcout << board[l * c + c];
			}
		}
	}

	// @cleanBoard : Reset all board fields to default state
	void CleanBoard() {
		wchar_t empty = ' ';
		for (int i = 0; i < b_high * b_wide; i++) {
			board[i] = empty;
		}
	}

	void UpdateTetrominoState() {
		// Update if elapse > 200 ms
		static auto start = std::chrono::high_resolution_clock::now();
		auto finish = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = finish - start;
		if (elapsed.count() < 0.2)
			return;
		start = std::chrono::high_resolution_clock::now();

		// Reset state if not current activate
		if (!tetromino->active) {
			tetromino->active = 1;
			tetromino->x = 3;
			tetromino->y = 0;
			tetromino->shape = rand() % TetrominoShapes.size();
			tetromino->rotation = 0;
		}

		// Move one block down
		tetromino->y++;

		// If state is invalid, undo movement 
		// and add tetromino to the board.
		if (!IsStateValid()) {
			tetromino->y--;
			AddTetrominoToBoard();
		}

		// Check if one or more lines are completed
		CheckFilledLines();

	}

	void CheckFilledLines() {
		int numberOfPoints = 0;

		for (int l=0; l < b_high; l++) {
			int blocksFound = 0;
			for (int c=0; c < b_wide; c++) {
				if (board[(l * b_wide) + c] != ' ') {
					blocksFound++;
				}
			}

			// If line is completely filled
			if (blocksFound == b_wide) {
				numberOfPoints++;

				DeleteLineFromBoard(l);
			}
		}
	}

	void DeleteLineFromBoard(int lineToDelete) {
		// Move lines down
		if (lineToDelete > 0) {
			for (int l = lineToDelete; l != 0; l--) {
				for (int c = 0; c < b_wide; c++) {
					int from = ((l - 1) * b_wide) + c;
					int to = (l * b_wide) + c;
 					board[to] = board[from];
				}
			}
		}

		// Clean first line
		for (int c = 0; c < b_wide; c++)
			board[c] = ' ';


	}


	// @IsStateValid : Test active tetromino state
	//
	// @return 1; if current state is valid
	// @return 0; if current state is invalid
	int IsStateValid() {
		std::string t = GetTetrominoShape();
		int t_high = 0;
		int t_wide = 0;

		// Calculate tetromino high
		for (int l = 0; l < 4; l++) {
			bool foundInLine = false;
			for (int c = 0; c < 4; c++) {
				if (t[l * 4 + c] == 'x')
					foundInLine = true;
			}
			if (foundInLine)
				t_high++;
		}

		// Check tetromino high
		// return 0 if tetromino is equal or above the board
		if (t_high + tetromino->y > b_high)
			return 0;

		// Calculate tetromino wide
		for (int c = 0; c < 4; c++) {
			bool foundInColumn = false;
			for (int l = 0; l < 4; l++) {
				if (t[l * 4 + c] == 'x')
					foundInColumn = true;
			}
			if (foundInColumn)
				t_wide++;
		}

		// Check tetromino wide
		// return 0 if tetromino is beyond the board
		if (t_wide + tetromino->x > b_wide)
			return 0;


		// Check if tetromino reach some block in the board
		for (int l = 0; l < 4; l++) {
			for (int c = 0; c < 4; c++) {
				if (t[l * 4 + c] == 'x' && board[(tetromino->y + l) * b_wide + (tetromino->x + c)] != ' ')
					return 0;
			}
		}

		return 1;
	}

	// @AddTetrominoToBoard : Add current tetromino to the board
	void AddTetrominoToBoard() {
		std::string t = GetTetrominoShape();

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (t[i * 4 + j] == 'x')
					//board[(tetromino->y + i)*b_wide + (tetromino->x + j)] = tetromino->shape;
					board[(tetromino->y + i) * b_wide + (tetromino->x + j)] = '#';
			}
		}

		// Disable current tetromino
		tetromino->active = 0;
	}

	// @GetTetrominoShape : Return current tetromino shape
	// return std::string
	std::string GetTetrominoShape() {
		return TetrominoShapes[tetromino->shape][tetromino->rotation];
	}


	// Tetrominos movements
	void Rotate() {
		int initialState = tetromino->rotation;
		if (tetromino->rotation < 3)
			tetromino->rotation++;
		else
			tetromino->rotation = 0;
		if (!IsStateValid())
			tetromino->rotation = initialState;
	}

	void MoveToLeft() {
		if (tetromino->active &&
			tetromino->x != 0) {
			tetromino->x--;
			if (!IsStateValid())
				tetromino->x++;
		}
	}

	void MoveToRight() {
		if (tetromino->active &&
			tetromino->x < b_wide - 1) {
			tetromino->x++;
			if (!IsStateValid())
				tetromino->x--;
		}
	}

};

// -----------------------------------------------------------------
// Screen
// -----------------------------------------------------------------
class Screen {
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
	Screen(Tetris* gs) {
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
	void EnableGameScreen() {
		hScreenCurrent = hScreenGame;
		SetConsoleActiveScreenBuffer(hScreenCurrent);
	}

	// @EnableConsoleScreen : Switch to the console debug screen
	void EnableConsoleScreen() {
		hScreenCurrent = hScreenConsole;
		SetConsoleActiveScreenBuffer(hScreenCurrent);
	}

	// @ToggleScreen : Toggle between Console and Game screens
	void ToggleScreen() {
		if (hScreenCurrent == hScreenConsole)
			EnableGameScreen();
		else
			EnableConsoleScreen();
	}

	// @UpdateBoard : Update current board state and with overlays
	void UpdateBoard() {
		// Update if elapse > 0.032 ms (equal to 30 fps)
		static auto start = std::chrono::high_resolution_clock::now();
		auto finish = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = finish - start;
		if (elapsed.count() < 0.032)
			return;
		start = std::chrono::high_resolution_clock::now();

		// Update screen size if was resized
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(hScreenGame, &csbi);
		int newWide = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		if (newWide != screenWide) {
			screenWide = newWide;
			overlay = new wchar_t[screenWide * screenHigh];
			screen = new wchar_t[screenWide * screenHigh];
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
		wchar_t boardBg = '|';
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
	void Log(const char x[]) {
		HANDLE current = hScreenCurrent;
		SetConsoleActiveScreenBuffer(hScreenConsole);
		std::cout << x << std::endl;
		SetConsoleActiveScreenBuffer(current);
	}

};



// -----------------------------------------------------------------
// Input
// -----------------------------------------------------------------
class Input {
	Tetris gameState;
	Screen screenState;

	char inputBuffer[10];
	int  inputBufferCursor = 0;

public:
	Input(Tetris* gs, Screen* ss) {
		gameState = *gs;
		screenState = *ss;
	}

	void DebugCleanInputBuffer() {
		// Pop each input from buffer
		char out = PopInput();
		while (out != '#') {
			std::cout << out;
			out = PopInput();
		}
		std::cout << std::endl;
	}

	void WatchInputs() {
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

	void ProcessInputs() {
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
				gameState.tetromino->shape = rand() % TetrominoShapes.size();
				break;

			default:
				break;
			}

			// Pop next input
			input = PopInput();
		}
	}

private:

	// @AddInput : Add the input to the 'inputBuffer'
	// This buffer will be consumed during the next gameloop.
	void AddInput(char input) {
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
	char PopInput() {
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


};


int main() {
	// Main vars
	Tetris gameState;
	Screen screen(&gameState);
	Input input(&gameState, &screen);
	screen.Log("Game first state started.");



	// Record start time
	DEBUG(" ");
	auto start = std::chrono::high_resolution_clock::now();
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	if (elapsed.count() > 0.2) { DEBUG("Cafe"); }


	// Main gameloop
	screen.Log("Start gameloop.");
	gameState.DebugBoard();
	screen.EnableGameScreen();


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
