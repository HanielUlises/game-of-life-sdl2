#include "Game.h"

#define TICK_INTERVAL 10
static Uint32 nextTime;

Uint32 timeLeft() {
	Uint32 currentTime = SDL_GetTicks();
	if (nextTime <= currentTime) return 0;
	else return nextTime - currentTime;
}

Game::Game(){
	window = SDL_CreateWindow(
		"Conway's Game of Life",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		800,
		800,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);
	if (window == nullptr) {
		std::cout << "Failed to create a window <<SDL_Window>>" << std::endl;
		return;
	}

	renderer = SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_PRESENTVSYNC
	);
	if (renderer == nullptr) {
		std::cout << "Failed to create a renderer <<SDL_Renderer>> " << std::endl;
		return;
	}

	isRunning = true;
}

Game::~Game(){
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void Game::initializeCells(int rows, int columns){

	cells.clear();
	grid.rows = rows;
	grid.columns = columns;
	
	//Creating the rows and columns
	for (int x = 0; x < grid.rows; x++) {
		//Creating a column for every row
		std::vector<cell> column;
		for (int y = 0; y < grid.columns; y++) {
			column.push_back(cell()); //Adding cells to the column
		}
		//Adding the column to the grid
		cells.push_back(column);
	}
	
}

void Game::Draw(){
	//Iterating throught the x and y grid cells
	for (int x = 0; x < grid.rows; x++) {
		for (int y = 0; y < grid.columns; y++) {

			/*Creating an SDL rectangle in order to draw the cells
			defining the structure SDL_FRect determines the 
			rectangle for each cell, defines its postion and size*/

			SDL_FRect cellRect = {
				1.0f + (float)grid.posX + ((float)x * grid.cellWidth),
				1.0f + (float)grid.posY + ((float)y * grid.cellHeight),
				grid.cellWidth,
				grid.cellHeight
			};
			//Colors on the cells status, black for alive cell, white (empty) for dead cell
			if (cells[x][y].isAlive) {
				SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
			}
			else {
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			}
			//Drawing with the mouse
			SDL_RenderFillRectF(renderer, &cellRect);
			if (mouseX > cellRect.x && mouseX < cellRect.x + cellRect.w) {
				if (mouseY > cellRect.y && mouseY < cellRect.y + cellRect.h) {
					mouseCellX = x;
					mouseCellY = y;
					SDL_SetRenderDrawColor(renderer, 150, 150, 150, 15);
					SDL_RenderFillRectF(renderer, &cellRect);
				}
			}
		}
	}
}


void Game::adjustGrid(){

	int windowWidth = 0;
	int windowHeight = 0;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	int maxLength = 0;
	if (windowWidth > windowHeight) {
		maxLength = windowHeight;
	}
	else {
		maxLength = windowWidth;
	}
	//Setting a border from the window to the grid, previously defined border size
	grid.posX = grid.borderSize;
	grid.posY = grid.borderSize;
	
	int botRightX = maxLength - grid.borderSize;
	int botRightY = maxLength - grid.borderSize;

	grid.width = botRightX - grid.posX;
	grid.height = botRightY - grid.posY;

	grid.cellWidth = (float)grid.width / (float)grid.rows;
	grid.cellHeight = (float)grid.height / (float)grid.columns;

	//Centering the window based on the size of the current opened window
	//Keeps the grid squared and centered
	if (windowWidth > windowHeight) {
		grid.posX = ((float)windowWidth / 2.0f) - ((float)grid.width / 2.0f);
	}
	else {
		grid.posY = ((float)windowHeight / 2.0f) - ((float)grid.height / 2.0f);
	}
}

void Game::Run(){
	initializeCells(50, 50);
	adjustGrid();

	nextTime = SDL_GetTicks() + TICK_INTERVAL;

	while (isRunning) {
		
		
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		//Renders everything
		Draw();
		//This flips the renderer buffer
		SDL_RenderPresent(renderer);
		//Handling window events such like mouse clicks, escape, space
		handleEvents();
		//Runnning the game
		if (gameRunning && !mouseDown) {
			runSimulation();
		}

		

		//Framerate limiter
		SDL_Delay(timeLeft());
		nextTime += TICK_INTERVAL;
	}
}

void Game::handleEvents() {
	SDL_Event ev;
	SDL_WaitEvent(&ev);
	
	while (SDL_PollEvent(&ev)) {

		if (ev.type == SDL_MOUSEMOTION) {
			SDL_GetMouseState(&mouseX, &mouseY);
		}

		if (ev.type == SDL_MOUSEBUTTONDOWN) {
			if (ev.button.button == SDL_BUTTON_LEFT)
				mouseDown = true;
		}

		if (ev.type == SDL_MOUSEBUTTONUP) {
			if (ev.button.button == SDL_BUTTON_LEFT)
				if (mouseDown)
					mouseDown = false;
		}

		if (ev.type == SDL_WINDOWEVENT) {
			//Closing the window actually and shutting down the program
			if (ev.window.event == SDL_WINDOWEVENT_CLOSE) {
				isRunning = false;
			}
			//I think this is actually pretty clever
			if (ev.window.event == SDL_WINDOWEVENT_RESIZED) {
				adjustGrid();
			}
			//Also this one 
			if (ev.window.event == SDL_WINDOWEVENT_RESTORED) {
				adjustGrid();
			}

			if (ev.window.event == SDL_WINDOW_FULLSCREEN) {
				adjustGrid();
			}

		}

		if (ev.type == SDL_KEYDOWN) {

			if (ev.key.keysym.sym == SDLK_ESCAPE)
				isRunning = false;

			if (ev.key.keysym.sym == SDLK_SPACE) {
				gameRunning = !gameRunning;
				if (gameRunning)		SDL_SetWindowTitle(window, "Conway's Game of Life --- (running)");
				else if (!gameRunning)	SDL_SetWindowTitle(window, "Conway's Game of Life --- (paused)");
			}
		}
	}
	//Clicking changes the state of aw cell
	if (mouseDown) cells[mouseCellX][mouseCellY].flip();
}

void Game::runSimulation(){
	for (int x = 0; x < cells.size(); x++) {
		for (int y = 0; y < cells[x].size(); y++) {
			cells[x][y].aliveNeighbours = 0;//Deleting the current cell's neighbour count
			//The next lines increment the neighbours 
			// if the cell at an actual position
			// Is alive 
			//Defining the top left
			if (cells[indexX(x - 1)][indexY(y - 1)].isAlive) {
				cells[x][y].aliveNeighbours++;
			}
			//Defining the top
			if (cells[indexX(x)][indexY(y - 1)].isAlive) {
				cells[x][y].aliveNeighbours++;
			}
			//Defining the top right
			if (cells[indexX(x+1)][indexY(y - 1)].isAlive) {
				cells[x][y].aliveNeighbours++;
			}
			//Defining the left
			if (cells[indexX(x - 1)][indexY(y)].isAlive) {
				cells[x][y].aliveNeighbours++;
			}
			//Defining the right
			if (cells[indexX(x + 1)][indexY(y)].isAlive) {
				cells[x][y].aliveNeighbours++;
			}
			//Defining bottom left
			if (cells[indexX(x - 1)][indexY(y + 1)].isAlive) {
				cells[x][y].aliveNeighbours++;
			}
			//Defining bottom 
			if (cells[indexX(x)][indexY(y + 1)].isAlive) {
				cells[x][y].aliveNeighbours++;
			}
			//Defining bottom right
			if (cells[indexX(x + 1)][indexY(y + 1)].isAlive) {
				cells[x][y].aliveNeighbours++;
			}
		}
	}
	while (simTick < simDelay) {
		simTick++;
		SDL_Delay(1);
	}
	//Defining the actual Conway's game of life rules (logic)
	for (int x = 0; x < grid.rows; x++) {
		for (int y = 0; y < grid.columns; y++) {
			if (cells[x][y].aliveNeighbours == 3) {
				cells[x][y].newCell();
			}
			//Underpopulation 
			if (cells[x][y].aliveNeighbours < 2) {
				cells[x][y].Die();
			}
			//Overpopulation
			if (cells[x][y].aliveNeighbours > 3) {
				cells[x][y].Die();
			}
		}
		simTick = 0;
	}
}

