#include "game.h"
#include <stdexcept>
#include <stdio.h>

int main(int argc, char* argv[]) {
	Game game;

	try {
		game.run();
	}
	catch (std::runtime_error e) {
		fprintf(stderr, "%s\n", e.what());
	}
	return 0;
}
