// Tetris.cpp
#include "pch.h"
#include "Tetris.h"
#include "Screen.h"
#include "Input.h"
#include <iostream>
#include <chrono>

#define DEBUG(x) std::cout << x << std::endl;

std::vector<std::vector<std::string>> Tetris::GetTetrominoShapes() {
	auto tetrominoShapes = {
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
	
	return tetrominoShapes;
}

// @Tetris constructor
// Create game board and prepare the initial game state.
Tetris::Tetris() {
	// Create board
	board = new wchar_t[b_high * b_wide];
	CleanBoard();
}

Tetris::~Tetris() {
	delete board;
}

// @debugBoard : Print current board state
void Tetris::DebugBoard() {
	// each line
	for (int l = 0; l < b_high; l++) {
		// each column
		for (int c = 0; c < b_wide; c++) {
			std::wcout << board[l * c + c];
		}
	}
}

// @cleanBoard : Reset all board fields to default state
void Tetris::CleanBoard() {
	wchar_t empty = ' ';
	for (int i = 0; i < b_high * b_wide; i++) {
		board[i] = empty;
	}
}

void Tetris::UpdateTetrominoState() {
	// Update if elapse > 200 ms
	static auto start = std::chrono::high_resolution_clock::now();
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	if (elapsed.count() < 0.15)
		return;
	start = std::chrono::high_resolution_clock::now();

	// Reset state if not current activate
	if (!tetromino->active) {
		tetromino->active = 1;
		tetromino->x = 3;
		tetromino->y = 0;
		tetromino->shape = rand() % GetTetrominoShapes().size();
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

void Tetris::CheckFilledLines() {
	int numberOfPoints = 0;

	for (int l = 0; l < b_high; l++) {
		int blocksFound = 0;
		for (int c = 0; c < b_wide; c++) {
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

void Tetris::DeleteLineFromBoard(int lineToDelete) {
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
int Tetris::IsStateValid() {
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
void Tetris::AddTetrominoToBoard() {
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
std::string Tetris::GetTetrominoShape() {
	return GetTetrominoShapes()[tetromino->shape][tetromino->rotation];
}


// Tetrominos movements
void Tetris::Rotate() {
	int initialState = tetromino->rotation;
	if (tetromino->rotation < 3)
		tetromino->rotation++;
	else
		tetromino->rotation = 0;
	if (!IsStateValid())
		tetromino->rotation = initialState;
}

void Tetris::MoveToLeft() {
	if (tetromino->active &&
		tetromino->x != 0) {
		tetromino->x--;
		if (!IsStateValid())
			tetromino->x++;
	}
}

void Tetris::MoveToRight() {
	if (tetromino->active &&
		tetromino->x < b_wide - 1) {
		tetromino->x++;
		if (!IsStateValid())
			tetromino->x--;
	}
}
