#include <bits/stdc++.h>
#include <windows.h>
using namespace std;

/*

Friend War Inside-1000 双人对决游戏

Game Description:
    游戏开始有A和B两个玩家,起初他们堆叠在一起，需要任意一个玩家移开。
    A玩家操作方式: WSAD上下左右移动, Z朝玩家面向方向发射子弹
    B玩家操作方式: IKJL上下左右移动, M朝玩家面向方向发射子弹

Game Data:
    子弹伤害(BULLET_HURT): 5点
    最大生命值(MAX_HEALTH): 100
    子弹CD(MAX_CD): 1/4秒
    
本作品由天寻(https://space.bilibili.com/627871340)制作
GitHub仓库: https://github.com/GitTianxun163/FriendWar

感谢支持，喜欢给个三连！

*/

// 定义常量

const float MAX_FPS=60; // 最大帧数

const HANDLE STDOUT_HANDLE = GetStdHandle(STD_OUTPUT_HANDLE); // 控制台句柄

const short UP_EDGE=4; // 上边缘
const short LEFT_EDGE=0; // 左边缘
const short DOWN_EDGE=47; // 下边缘
const short RIGHT_EDGE=160; // 右边缘
// 方向
const short LEFT=1;
const short RIGHT=2;
const short UP=3;
const short DOWN=4;

const unsigned short MAX_HEALTH=100; // 最大生命值
const short BULLET_HURT=5;  // 子弹伤害

const int MAX_CD=MAX_FPS/4;  // 子弹CD
// 按键(Keys)
const char ESC=0x1B;
const char F8=0x77;
const char SHIFT = 0x10;
const char CONTROL = 0x11;

// 定义全局变量

bool over; // 是否失败
bool stop; // 暂停

// 定义函数、类

void keyboard();
void update();
void draw();
void init();
void setConsolePos(short x,short y);
void SetConsoleWindowSize(short width,short height);
void developerOptions();

// 实现函数、类
class Player{  
    short health;
    short direction;
    bool show; 
    public:
    int CD;
    short x,y;
    Player(){
        this->x=LEFT_EDGE;
        this->y=UP_EDGE;
        this->health = MAX_HEALTH;
        this->direction=RIGHT;
        this->CD = 0;
        this->show = true;
    }

    void hidePlayer() {
        this->show = false;
    }
    void showPlayer() {
        this->show = true;
    }

    void forword(int length=1) {
        this->direction=RIGHT;
        if (this->x<RIGHT_EDGE) this->x+=length;
    }
    void backword(int length=1) {
        this->direction=LEFT;
        if (this->x>LEFT_EDGE) this->x-=length;
    }
    void upword(int length=1) {
        this->direction=UP;
        if (this->y>UP_EDGE) this->y-=length;
    }
    void downword(int length=1) {
        this->direction=DOWN;
        if (this->y<DOWN_EDGE) this->y+=length;
    }

    short getHealth(){
        return this->health;
    }
    void setHealth(short newHealth){
        if (newHealth<=MAX_HEALTH) this->health=newHealth;
    }
    void addHealth(short length){
        if ((this->health+length)<=MAX_HEALTH) this->health += length;
    }
    short getDirection(){
        return this->direction;
    }

    void draw(char c=0){
        if (!this->show) return;
        setConsolePos(x,y);
        if (this->getHealth()>=70) SetConsoleTextAttribute(STDOUT_HANDLE,0x0A);
        else if (this->getHealth()>=40) SetConsoleTextAttribute(STDOUT_HANDLE,0x0E);
        else if (this->getHealth()>=20) SetConsoleTextAttribute(STDOUT_HANDLE,0x06);
        else SetConsoleTextAttribute(STDOUT_HANDLE,0x04);
        printf("%c",c);
        if (this->direction==RIGHT && x!=RIGHT_EDGE) {
            printf("-");
        }
        else if (this->direction==LEFT && x!=LEFT_EDGE) {
            setConsolePos(x-1,y);
            printf("-");
        }
        else if (this->direction==UP && y!=UP_EDGE) {
            setConsolePos(x,y-1);
            printf("|");
        }
        else if (this->direction==DOWN && y!=DOWN_EDGE) {
            setConsolePos(x,y+1);
            printf("|");
        }
    }
};

Player player;
Player player2;

class Bullet{
    short x,y;
    short direction;
    Player from;
    public:
    bool isExt;
    Bullet(Player with){
        this->from = with;
        this->direction = from.getDirection();
        this->isExt=true;

        this->x=from.x;
        this->y=from.y;
    }
    void update(){
        // if (this->x!=this->from.x && this->y!=this->from.y) {
        
        // }
        if (this->x<LEFT_EDGE || this->y<UP_EDGE || this->x>=RIGHT_EDGE || this->y>=DOWN_EDGE) {
            this->isExt=false;
            this->x=-1;
            this->y=-1;
        }
        if (this->direction==RIGHT) this->x++;
        else if (this->direction==LEFT) this->x--;
        else if (this->direction==DOWN) this->y++;
        else if (this->direction==UP) this->y--;

        if (this->x==player.x && this->y==player.y) {
            player.addHealth(-BULLET_HURT);
            this->isExt=false;
        }
        else if (this->x==player2.x && this->y==player2.y) {
            player2.addHealth(-BULLET_HURT);
            this->isExt=false;
        }
    }
    void draw(){
        if (this->isExt){
            setConsolePos(x,y);
            printf("*");
        }
    }

};
list<Bullet> bulletList;

void keyboard(){
    player.CD++;
    player2.CD++;
    if (!over && !stop){
        // 玩家1移动
        if (GetAsyncKeyState('W') & 0x8000) player.upword();
        if (GetAsyncKeyState('A') & 0x8000) player.backword();
        if (GetAsyncKeyState('S') & 0x8000) player.downword();
        if (GetAsyncKeyState('D') & 0x8000) player.forword();

        // 玩家2移动
        if (GetAsyncKeyState('I') & 0x8000) player2.upword();
        if (GetAsyncKeyState('J') & 0x8000) player2.backword();
        if (GetAsyncKeyState('K') & 0x8000) player2.downword();
        if (GetAsyncKeyState('L') & 0x8000) player2.forword();

        // 玩家1、2攻击
        if ((GetAsyncKeyState('Z') & 0x8000) && player.CD>=MAX_CD) {
            bulletList.push_back(Bullet(player));
            player.CD = 0;

        } // 玩家1 攻击
        if ((GetAsyncKeyState('M') & 0x8000) && player2.CD>=MAX_CD) {
            bulletList.push_back(Bullet(player2));
            player2.CD=0;
        } // 玩家2 攻击

        // 暂停
        if (GetAsyncKeyState(ESC) & 0x8000) stop++;
    }
    else if (stop){
        if (GetAsyncKeyState('1') & 0x8000) exit(0);
        else if (GetAsyncKeyState('2') & 0x8000) stop=false;
    }
    else {
        // 退出
        if (GetAsyncKeyState(ESC) & 0x8000) exit(0);
    }
}

void update(){
    SetConsoleWindowSize(800,600);
    if (player.getHealth()<=0 || player2.getHealth()<=0){
        over = true;
    }
    list<Bullet>::iterator bullet = bulletList.begin();
    for (bullet;bullet!=bulletList.end();bullet++) {
        if (!((*bullet).isExt)) {
            bulletList.erase(bullet);
            break;
        }
    }
    bullet=bulletList.begin();
    for (bullet;bullet!=bulletList.end();bullet++) {
        if ((*bullet).isExt) {
            (*bullet).update();
        }
    }
}

void showPlayerHealth(Player player,short line=0) {
    int step=20;
    int s=100/step;
    SetConsoleTextAttribute(STDOUT_HANDLE,0x88);
    for (int i=0;i<step;i++) printf(" ");
    if (player.getHealth()>=70) SetConsoleTextAttribute(STDOUT_HANDLE,0xAA);
    else if (player.getHealth()>=40) SetConsoleTextAttribute(STDOUT_HANDLE,0xEE);
    else if (player.getHealth()>=20) SetConsoleTextAttribute(STDOUT_HANDLE,0x66);
    else SetConsoleTextAttribute(STDOUT_HANDLE,0x44);
    setConsolePos(16,line);
    for (int i=0;i<player.getHealth()/s&&i<step;i++) printf(" ");
    if (player.getHealth()>=70) SetConsoleTextAttribute(STDOUT_HANDLE,0x0A);
    else if (player.getHealth()>=40) SetConsoleTextAttribute(STDOUT_HANDLE,0x0E);
    else if (player.getHealth()>=20) SetConsoleTextAttribute(STDOUT_HANDLE,0x06);
    else SetConsoleTextAttribute(STDOUT_HANDLE,0x04);
    setConsolePos(16+step,line);
    printf(" | %3d/%3d\n",player.getHealth(),MAX_HEALTH);

    SetConsoleTextAttribute(STDOUT_HANDLE,0x07);
}

void draw(){
    printf("Players:2\n");

    printf("Player1 Health: ");
    showPlayerHealth(player,1);

    printf("Player2 Health: ");
    showPlayerHealth(player2,2);

    player.draw('A');
    player2.draw('B');

    SetConsoleTextAttribute(STDOUT_HANDLE,0x07);

    list<Bullet>::iterator bullet = bulletList.begin();
    for (bullet;bullet!=bulletList.end();bullet++) {
        if ((*bullet).isExt) {
            (*bullet).draw();
        }
    }

    if (over) {
        setConsolePos(45,10);
        SetConsoleTextAttribute(STDOUT_HANDLE,0x0F);
        printf("Game Over!");
    }
    if (stop) {
        setConsolePos(45,15);
        printf("Pause Menu:");
        setConsolePos(47,17);
        printf("1 : Exit Game");
        setConsolePos(47,18);
        printf("2 : Continue Game");
    }
}

void init(){
    over=false;
    stop=false;
    SetConsoleTitle("Friend War Inside-1000");
    SetConsoleTextAttribute(STDOUT_HANDLE,0x07);
    printf(" ----------------------------------\n");Sleep(11);
    printf(" |                                |\n");Sleep(10);
    printf(" |                                |\n");Sleep(10);
    printf(" |            Friend War          |\n");Sleep(11);
    printf(" |                                |\n");Sleep(10);
    printf(" |                                |\n");Sleep(10);
    printf(" ----------------------------------\n");Sleep(11);
    printf(" Version Inside-1000    Made in Tianxun\n\n\n");Sleep(10);
    printf(" Loading: ");
    SetConsoleTextAttribute(STDOUT_HANDLE,0x88);
    printf("                                                  \n");Sleep(10);
    SetConsoleTextAttribute(STDOUT_HANDLE,0x07);
    printf(" Press F8 to Developer Options");Sleep(10);
    setConsolePos(10,10);
    SetConsoleTextAttribute(STDOUT_HANDLE,0x77);
    for (int i=0;i<50;i++){
        if (GetAsyncKeyState(F8) & 0x8000) {
            developerOptions();
            return;
        }
        srand(time(NULL));
        Sleep(rand()%100);
        printf("=");
    }
    system("cls");
}

void developerOptions(){
    system("cls");
    SetConsoleTextAttribute(STDOUT_HANDLE,0x10);
    printf("                      Developer Options                      \n");Sleep(10);SetConsoleTextAttribute(STDOUT_HANDLE,0x70);
    printf("                         No Options                          \n");Sleep(10);
    system("pause");
    system("color 0f");
    system("cls");
    init();
}

void setConsolePos(short x,short y){
    HANDLE hd;
	COORD pos;
	
	pos.X = x;
	pos.Y = y;
	hd = STDOUT_HANDLE;	/*获取标准输出的句柄*/ 
	SetConsoleCursorPosition(hd, pos);		/*设置控制台光标输出的位置*/
}

void SetConsoleWindowSize(short width,short height){
    HANDLE hStdOutput = STDOUT_HANDLE;
    COORD coord = { width/8, height/28 };
    SetConsoleScreenBufferSize( hStdOutput, coord ); // 设置缓冲尺寸
}

int main(){
    init();
    while (true){
        update();
        draw();
        keyboard();
        SetConsoleTextAttribute(STDOUT_HANDLE,0x07);
        setConsolePos(0,0);
        Sleep(1000/MAX_FPS);
        system("cls");
    }

    return 0;
}