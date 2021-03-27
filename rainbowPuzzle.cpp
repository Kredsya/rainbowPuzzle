#include <bangtal.h>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>
using namespace std;

const double tick = 0.0005;

SceneID scene;
ObjectID board[36], original_board[36];
ObjectID start_button;
TimerID timer, timer_game;
int blank = 35, cnt = 1000;
bool start_flag = false;
clock_t start_time;

int locateX(int n) { return 300 + 100 * (n % 6); }
int locateY(int n) { return 520 - 100 * (n / 6); }

int extract_index(ObjectID object) {
	for (int i = 0; i < 36; i++) if (board[i] == object) return i;
	return -1;
}

void move(int i) {
	swap(board[i], board[blank]);
	locateObject(board[i], scene, locateX(i), locateY(i));
	locateObject(board[blank], scene, locateX(blank), locateY(blank));
	blank = i;
}

bool check(int i) {
	if (blank == i - 1 || blank == i + 1 || blank == i - 6 || blank == i + 6) return true;
	return false;
}

void doShuffle() {
	setTimer(timer, tick);
	startTimer(timer);
}

bool check_over() {
	for (int i = 0; i < 36; i++) if (board[i] != original_board[i]) return false;
	return true;
}

void mouseCallback(ObjectID object, int x, int y, MouseAction action) {
	if (object == start_button) {
		hideObject(board[blank]);
		hideObject(start_button);
		doShuffle();
	}
	if (start_flag) {
		int index = extract_index(object);
		if (index != -1) {
			if (check(index)) {
				move(index);
			}
		}

		if (blank == 35) {
			if (check_over()) {
				showObject(start_button);
				showObject(board[blank]);
				start_flag = false;
				string ending = "축하합니다! ";
				ending += to_string((int)((clock() - start_time) / CLOCKS_PER_SEC));
				ending += "초 걸리셨습니다!";
				showMessage(ending.c_str());
				cnt = 1000;
			}
		}
	}
}

void timerCallback(TimerID t) {
	int i = -1;
	while (i == -1) {
		switch (rand() % 4) {
		case 0: if (blank / 6 < 5) i = blank + 6;
			break;
		case 1: if (blank / 6 > 0) i = blank - 6;
			break;
		case 2: if (blank % 6 < 5) i = blank + 1;
			break;
		case 3: if (blank % 6 > 0) i = blank - 1;
			break;
		}
	}
	move(i);

	if (cnt--) {
		setTimer(timer, tick);
		startTimer(timer);
		start_time = clock();
	}
	else start_flag = true;
}

SceneID init() {
	scene = createScene("background", "Images/background.png");

	string front = "Images/", back = ".png";
	for (int i = 0; i < 36; i++) {
		string path = front + to_string(i + 1) + back;
		board[i] = createObject(path.c_str());
		locateObject(board[i], scene, locateX(i), locateY(i));
		showObject(board[i]);

		original_board[i] = board[i];
	}

	start_button = createObject("Images/start.png");
	locateObject(start_button, scene, 590, 100);
	showObject(start_button);

	timer = createTimer(tick);
	timer_game = createTimer(0);

	return scene;
}

void set_game() {
	srand(time(0));
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
	setMouseCallback(mouseCallback);
	setTimerCallback(timerCallback);
}

int main()
{
	set_game();
	startGame(init());
}