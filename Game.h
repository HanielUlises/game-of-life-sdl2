#pragma once

#include "iostream"
#include "vector"
#include "SDL.h"


//Structure for defining a cell, the cell might be dead on alive, depending on the 
//amount of alive neighbours
struct cell {
	cell(bool alive = false) {
		isAlive = alive;
	}

	void Die() {
		isAlive = false;
	}
	void newCell() {
		isAlive = true;
	}
	//Permutation of states, between dead and alive cell
	void flip() {
		isAlive = !isAlive;
	}

	bool isAlive = false;		
	int aliveNeighbours = 0;
};

//Structure for the grid that's actually going to be displayed
struct Grid {
	Grid() {

	}
	int rows = 0;
	int columns = 0;
	//Grid position in the window
	int posX = 0;
	int posY = 0;

	//Grid size within the window
	int width = 0;
	int height = 0;

	//size of every cell
	float cellWidth = 0;
	float cellHeight = 0;

	//Gap between the window and the grid
	int borderSize = 10;
};

class Game
{
public:
	//Class constructor & destructor
	Game();
	~Game();

	//Function for initializing cells 
	void initializeCells(int rows, int columns);
	//Function for drawing the cells
	void Draw();
	//Adjusting grid at the window
	void adjustGrid();
	//Handling the mouse and other window actions
	void handleEvents();
	//Running the simulation
	void runSimulation();

	//Publicly accessible functions for the behavior
	//Loop for keeping the windows opened
	void Run();

private:
	//Reserve the game variables
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	//Flag for ending the program (useful within the while loop)
	bool isRunning = false;

	//The game uses the grid structure 
	Grid grid;
	//2D grid
	std::vector<std::vector<cell>> cells;

	//Delimeter for <<possible>> invalid indexes
	int indexX(int i) {
		if (i < 0) return 0;
		else if (i > grid.rows - 1) return grid.rows-1;
		else return i;
	}
	int indexY(int i) {
		if (i < 0) return 0;
		else if (i > grid.columns - 1) return grid.columns-1;
		else return i;
	}

	//Mouse current location
	int mouseX = 0;
	int mouseY = 0;
	bool mouseDown = 0;
	int mouseCellX = 0;
	int mouseCellY = 0;

	//Handling the simulation variables
	bool gameRunning = false; //The game is or not running
	int simTick = 0;		 //Current simulation timer tick
	int simDelay = 40;		 //Amount of ticks until the next iteration
};

