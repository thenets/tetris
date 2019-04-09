#ifndef _TETRIS_H_
#define _TETRIS_H_

#include <vector>
#include <string>

// -----------------------------------------------------------------
// Tetrominos
// -----------------------------------------------------------------
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
	bool debug = true;
	int cursor_x = 0;
	int cursor_y = 0;

	// @Tetris constructor
	// Create game board and prepare the initial game state.
	Tetris();

	// @debugBoard : Print current board state
	void DebugBoard();

	// @cleanBoard : Reset all board fields to default state
	void CleanBoard();

	// Get tetromino shapes
	static std::vector<std::vector<std::string>> GetTetrominoShapes();

	void UpdateTetrominoState();

	void CheckFilledLines();

	void DeleteLineFromBoard(int lineToDelete);


	// @IsStateValid : Test active tetromino state
	//
	// @return 1; if current state is valid
	// @return 0; if current state is invalid
	int IsStateValid();

	// @AddTetrominoToBoard : Add current tetromino to the board
	void AddTetrominoToBoard();

	// @GetTetrominoShape : Return current tetromino shape
	// return std::string
	std::string GetTetrominoShape();


	// Tetrominos movements
	void Rotate();
	void MoveToLeft();
	void MoveToRight();

};

#endif