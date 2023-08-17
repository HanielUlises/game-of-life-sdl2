#include "Game.h"

int main(int argc, char* []) {
	std::cout << "Started program" << std::endl;
	std::cout << "Press escape for closing the program" << std::endl;
	Game newGame;
	newGame.Run();
	std::cout << "Closing program... ";


	return 0;
}