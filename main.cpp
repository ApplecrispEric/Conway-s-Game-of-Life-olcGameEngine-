#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"


// RULES: https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life

// Game property constants.
const float CYCLE_TIME_NORMAL = 0.5f;
const float CYCLE_TIME_FAST_FORWARD = CYCLE_TIME_NORMAL / 3;
const int32_t BOARD_DIMENSION_CELLS = 30;
const int32_t PIXEL_SIZE = 32;

// Keybindings.
const olc::Key FAST_FORWARD_KEY = olc::Key::SHIFT;
const olc::Key PAUSE_KEY = olc::Key::SPACE;
const olc::Key RESET_KEY = olc::Key::ESCAPE;

// Cell states.
const olc::Pixel ALIVE = olc::WHITE;
const olc::Pixel DEAD = olc::BLACK;


class Life : public olc::PixelGameEngine {
public:
	bool gamePaused = true;
	float timer = CYCLE_TIME_NORMAL;
	std::array < std::array < olc::Pixel, BOARD_DIMENSION_CELLS>, BOARD_DIMENSION_CELLS> cells;

	Life() {
		sAppName = "Conway's Game of Life";
	}

	bool OnUserCreate() override {
		cells = std::array < std::array < olc::Pixel, BOARD_DIMENSION_CELLS>, BOARD_DIMENSION_CELLS>();
		ClearBoard();

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Pausing the game.
		if (GetKey(PAUSE_KEY).bPressed) {
			gamePaused = !gamePaused;
		}

		// Resetting the game.
		if (GetKey(RESET_KEY).bPressed) {
			gamePaused = true;
			ClearBoard();
		}

		if (gamePaused) {
			if (GetMouse(0).bHeld) {  // User is adding cells.
				cells[GetMouseY()][GetMouseX()] = ALIVE;
			}
			else if (GetMouse(1).bHeld) {  // User is removing cells.
				cells[GetMouseY()][GetMouseX()] = DEAD;
			}
		}
		else {
			// Fast forwarding the game.
			float waitTime = GetKey(FAST_FORWARD_KEY).bHeld ? CYCLE_TIME_FAST_FORWARD : CYCLE_TIME_NORMAL;

			if (timer >= waitTime) {
				UpdateBoard();
				timer = 0;
			}
			else {
				timer += fElapsedTime;
			}

		}
		DrawScreen();
		return true;
	}

private:
	void UpdateBoard() {
		auto boardCopy = cells;

		for (int32_t row = 0; row < BOARD_DIMENSION_CELLS; row++) {
			for (int32_t column = 0; column < BOARD_DIMENSION_CELLS; column++) {
				auto cellStatus = DEAD;

				switch (countALiveNeighborCells(row, column)) {
				case 2:
					cellStatus = cells[row][column];
					break;
				case 3:
					cellStatus = ALIVE;
					break;
				}

				boardCopy[row][column] = cellStatus;
			}
		}

		cells = boardCopy;
	}

	void ClearBoard() {
		for (int32_t row = 0; row < BOARD_DIMENSION_CELLS; row++) {
			for (int32_t column = 0; column < BOARD_DIMENSION_CELLS; column++) {
				cells[row][column] = DEAD;
			}
		}
	}

	void DrawScreen() {
		for (int row = 0; row < ScreenWidth(); row++) {
			for (int column = 0; column < ScreenHeight(); column++) {
				auto pixel = cells[column][row];

				if (gamePaused && row == GetMouseX() && column == GetMouseY()) {
					pixel = olc::GREY;
				}
				Draw(row, column, pixel);
			}
		}
	}

	int countALiveNeighborCells(int32_t row, int32_t column) {
		std::array<int, 3> directions = { -1, 0, 1 };
		int count = 0;

		for (int verticalStep : directions) {
			for (int horizontalStep : directions) {
				int currentRow = row + verticalStep;
				int currentColumn = column + horizontalStep;

				// Bounds checking.
				if (currentRow < 0 || currentRow >= BOARD_DIMENSION_CELLS || currentColumn < 0 || currentColumn >= BOARD_DIMENSION_CELLS) {
					continue;
				}

				// Don't count the parent cell.
				if (currentRow == row && currentColumn == column) {
					continue;
				}

				auto cell = cells[currentRow][currentColumn];

				if (cell == ALIVE) {
					count++;
				}
			}
		}
		return count;
	}
};

int main()
{
	Life app;
	bool ready = app.Construct(BOARD_DIMENSION_CELLS, BOARD_DIMENSION_CELLS, PIXEL_SIZE, PIXEL_SIZE);

	if (ready) {
		app.Start();
	}

	return 0;
}