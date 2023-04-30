#ifdef RICK
#define LPCWSTR LPCSTR
#endif
#include <cstdio>
#include <ctime>
#include <unistd.h>
#include <conio.h>
#include <windows.h>
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000)?true:false)
#define KEY_CLICKED(VK_NONAME) (event == int(VK_NONAME))
#define isSelect(x,y,id,name) \
            setConsolePos(x,y);\
            if (selectedButton == id) SetConsoleTextAttribute(STDOUT_HANDLE,0xf0);\
            else SetConsoleTextAttribute(STDOUT_HANDLE,0x07);\
            printf(name);
using namespace std;

// 常量
const HANDLE STDOUT_HANDLE = GetStdHandle(STD_OUTPUT_HANDLE);
const HANDLE STDIN_HANDLE = GetStdHandle(STD_INPUT_HANDLE);
const int WIDTH = 90;
const int HEIGHT = 50;
const SMALL_RECT WINDOW_RECT = {0, 0, WIDTH*5, HEIGHT*10};
const COORD BUFFER_SIZE = {WIDTH, HEIGHT};

const char* title_and_version = "Fortress Battle Inside-2000";
const char* title = "Fortress Battle";
const char* version = "Inside-2000";
// 按键(Keys)
const char ESC=0x1B;
const char F1=0x70;
const char F2=0x71;
const char F3=0x72;
const char F4=0x73;
const char F5=0x74;
const char F6=0x75;
const char F7=0x76;
const char F8=0x77;
const char F9=0x78;
const char F10=0x79;
const char F11=0x7A;
const char F12=0x7B;
const char SHIFT = 0x10;
const char CONTROL = 0x11;
const char ENTER = 0x0D;
const char KEY_UP = 0x48;
const char KEY_DOWN = 0x50;

// 类、函数
enum GameStatus {
    LOADING,
    START_SCREEN,
    GAMING,
    GAME_PAUSE,
    HISTORY_BATTLES,
    SETTINGS_0
};
const int buttons[] = {1,3,1,1,1,1};
class Player;
class Item;
class Bullet;
void init();
void draw();
void update();
void handoffGameStatus(GameStatus newStatus);
void keyEvent(const char event);
void mouseEvent();
void setConsolePos(short x,short y);
inline int randint(int _min,int _max);

// 变量
GameStatus status = LOADING;

CONSOLE_CURSOR_INFO cursor = {1,0};
DWORD consoleMode;

bool drawed;
int selectedButton;
bool uped;
// LOADING
int loadnum;

// 函数、类实现
class Player {
    short x,y;
    short speed;
    short buttleCd;
    short buttlespeed;
    bool aicontrol;
};

void init() {
    GetConsoleMode(STDIN_HANDLE,&consoleMode);  
    GetConsoleCursorInfo(STDOUT_HANDLE,&cursor);
    consoleMode &= ~ENABLE_QUICK_EDIT_MODE;  //移除快速编辑模式
    consoleMode &= ~ENABLE_INSERT_MODE;      //移除插入模式
    // consoleMode &= ~ENABLE_MOUSE_INPUT;
    cursor.bVisible = false;
    SetConsoleMode(STDIN_HANDLE,consoleMode);
    SetConsoleCursorInfo(STDOUT_HANDLE,&cursor);

    // 设置控制台窗口大小和位置
    SetConsoleWindowInfo(STDOUT_HANDLE, TRUE, &WINDOW_RECT);
    // 设置控制台输出缓冲区大小
    SetConsoleScreenBufferSize(STDOUT_HANDLE, BUFFER_SIZE);

    SetConsoleTitle((LPCSTR)title_and_version);
}

void draw() {
    setConsolePos(0,0);
    SetConsoleTextAttribute(STDOUT_HANDLE,0x07);
    if (status == LOADING) {
        if (drawed) {
            setConsolePos(0,HEIGHT-2);
            SetConsoleTextAttribute(STDOUT_HANDLE,0xff);
            for (int i=0;i<loadnum;i++) printf(" ");
        }
        else {
            setConsolePos(WIDTH/2-5,0);
            printf("Loading...");
        }
    }
    else if (status == START_SCREEN) {
        if (drawed) {
            isSelect(WIDTH/2-5,HEIGHT/5*2,0,"Start Game")
            isSelect(WIDTH/2-7,HEIGHT/5*3,1,"History Battle")
            isSelect(WIDTH/2-4,HEIGHT/5*4,2,"Settings")
        }
        else {
            setConsolePos(WIDTH/2-7,HEIGHT/5-3);
            printf(title);
            setConsolePos(WIDTH/2-11,HEIGHT/5*2-2);
            printf("----------------------");setConsolePos(WIDTH/2-11,HEIGHT/5*2-1);
            printf("|                    |");setConsolePos(WIDTH/2-11,HEIGHT/5*2);
            printf("|     Start Game     |");setConsolePos(WIDTH/2-11,HEIGHT/5*2+1);
            printf("|                    |");setConsolePos(WIDTH/2-11,HEIGHT/5*2+2);
            printf("----------------------");
            setConsolePos(WIDTH/2-11,HEIGHT/5*3-2);
            printf("----------------------");setConsolePos(WIDTH/2-11,HEIGHT/5*3-1);
            printf("|                    |");setConsolePos(WIDTH/2-11,HEIGHT/5*3);
            printf("|   History Battle   |");setConsolePos(WIDTH/2-11,HEIGHT/5*3+1);
            printf("|                    |");setConsolePos(WIDTH/2-11,HEIGHT/5*3+2);
            printf("----------------------");
            setConsolePos(WIDTH/2-11,HEIGHT/5*4-2);
            printf("----------------------");setConsolePos(WIDTH/2-11,HEIGHT/5*4-1);
            printf("|                    |");setConsolePos(WIDTH/2-11,HEIGHT/5*4);
            printf("|      Settings      |");setConsolePos(WIDTH/2-11,HEIGHT/5*4+1);
            printf("|                    |");setConsolePos(WIDTH/2-11,HEIGHT/5*4+2);
            printf("----------------------");
            setConsolePos(2,HEIGHT-2);
            printf("Version: %s",version);
        }
    }
    drawed = true;
}

void update() {
    if (status == LOADING) {
        if (rand()%5==1) loadnum += randint(0,2);
        if (loadnum >WIDTH) {
            handoffGameStatus(START_SCREEN);
        }    
    }
}

void handoffGameStatus(GameStatus newStatus) {
    status = newStatus;
    setConsolePos(0,0);
    SetConsoleTextAttribute(STDOUT_HANDLE,0x07);
    printf("\a");
    system("cls");
    drawed = false;
    draw();
}

void keyEvent(const char event='\0') {
    if (KEY_CLICKED(KEY_UP)) selectedButton--;
    if (KEY_CLICKED(KEY_DOWN)) selectedButton++;
    if (selectedButton >= 0) selectedButton %= buttons[status];
    else selectedButton += buttons[status];
    if (status == START_SCREEN) {
        if (KEY_DOWN(ENTER)) {
            if (selectedButton == 0) handoffGameStatus(GAMING);
            if (selectedButton == 1) handoffGameStatus(HISTORY_BATTLES);
            if (selectedButton == 2) handoffGameStatus(SETTINGS_0);
        }
    }
}

void mouseEvent() {

}

void setConsolePos(short x,short y){
    HANDLE hd;
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(STDOUT_HANDLE, pos);		/*设置控制台光标输出的位置*/
}

inline int randint(int _min,int _max) {
    srand(time(NULL)+rand()*rand()%3);
    return rand()%(_max-_min)+_min;
}

int main() {
    init();
    while(true) {
        draw();
        update();
        if (_kbhit()) keyEvent(_getch());
        else keyEvent();
        sleep(1.0/30);
    }
    return 0;
}