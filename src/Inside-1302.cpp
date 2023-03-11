#include <bits/stdc++.h>
#include <windows.h>
#define random(x,y) rand()%(y-x)+x
using namespace std;

/*

Friend War Inside-1302 双人对决游戏

Game Description:
    游戏开始有A和B两个玩家,起初他们堆叠在一起，需要任意一个玩家移开。
    A玩家操作方式: WSAD上下左右移动, Z朝玩家面向方向发射子弹, X换弹
    B玩家操作方式: IKJL上下左右移动, M朝玩家面向方向发射子弹, N换弹

Game Data:
    子弹伤害(BULLET_HURT): 5点
    最大生命值(MAX_HEALTH): 100
    子弹CD(MAX_CD): 1/4秒

Update Log:
    1. 设置utf-8编码，准备中文测试
    2. AI随机更改

本作品由天寻(https://space.bilibili.com/627871340)制作
GitHub仓库: https://github.com/GitTianxun163/FriendWar

感谢支持，喜欢给个三连！

*/

// 定义常量

const float MAX_FPS=30; // 最大帧数

const HANDLE STDOUT_HANDLE = GetStdHandle(STD_OUTPUT_HANDLE); // 控制台句柄

const short UP_EDGE=7; // 上边缘
const short LEFT_EDGE=0; // 左边缘
const short DOWN_EDGE=40; // 下边缘
const short RIGHT_EDGE=64; // 右边缘
// 方向
enum Direction {
    LEFT,
    RIGHT,
    UP,
    DOWN
};
enum ItemTypes {
    HEALTHY = '+'
};

const unsigned short MAX_HEALTH=100; // 最大生命值
const short BULLET_HURT=5;  // 子弹伤害
const short MAX_BULLETS=100;  // 一个玩家能拥有的最大子弹数
const short HEALTH_NUM = 3;  // 救治包恢复生命值
const int MAX_CD=MAX_FPS/4;  // 子弹CD
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

// 语言
map<string,const char*> chinesePack = {
    {"Name","Chinese"},
    {"PauseMenu.title","暂停菜单:"},
    {"PauseMenu.1","退出游戏"},
    {"PauseMenu.2","继续游戏"},
    {"PauseMenu.3","开启/关闭Player2的AI"},
    {"PauseMenu.4","开启/关闭Player1的AI"},
    {"PauseMenu.5","开启/关闭中文语言包"},
    {"PauseMenu.6","重启游戏"},
    {"StartMenu.title","选择游戏模式:"},
    {"hud.health.text","血量"},
    {"hud.bullet.text","子弹"},
    {"hud.mode.text","模式"},
    {"hud.PlayerTime.title","计时器"},
    {"hud.SystemTime.title","系统时间"},
    {"over.info","按ESC退出,按空格重启"}
};
map<string,const char*> englishPack = {
    {"Name","English"},
    {"PauseMenu.title","Pause Menu:"},
    {"PauseMenu.1","Exit Game"},
    {"PauseMenu.2","Continue Game"},
    {"PauseMenu.3","Open/Close AI for player2"},
    {"PauseMenu.4","Open/Close AI for player1"},
    {"PauseMenu.5","Open/Close Chinese Language pack"},
    {"PauseMenu.6","Restart Game"},
    {"StartMenu.title","Select Mode:"},
    {"hud.health.text","Health"},
    {"hud.bullet.text","Bullet"},
    {"hud.mode.text","Mode"},
    {"hud.PlayerTime.title","Player Time"},
    {"hud.SystemTime.title","System Time"},
    {"over.info","Press ESC Exit.Press space restart"}
};
map<string,const char*>& lang = englishPack;

// 定义全局变量

bool over; // 是否失败
bool stop; // 暂停
bool aiControl;  // 是否由ai操控玩家2
bool aiControl1; 
bool aifps;
bool starting;
time_t startTime;
time_t t;
time_t timep;
string debugs;

// 定义函数、类

void keyboard();
void update();
void draw();
void init();
void setConsolePos(short x,short y);
void restart();
void developerOptions();
class Player;
class Bullet;
class Item;
void updateAi(Player& player,Player& other);

// 实现函数、类

class Player{  
    short health;
    Direction direction;
    bool show; 
    public:
    bool newb;
    short bullet_num;
    int CD;
    short x,y;
    Player(short x=LEFT_EDGE,short y=UP_EDGE){
        this->x=x;
        this->y=y;
        this->health = MAX_HEALTH;
        this->direction=RIGHT;
        this->CD = 0;
        this->show = true;
        this->bullet_num=MAX_BULLETS;
        this->newb=false;
    }

    void setNewB(bool is=false) {
        if (this->bullet_num > 5 && this->newb) this->bullet_num -= 5;
        this->newb=is;
    }

    void hidePlayer() {
        this->show = false;
        this->setNewB();
    }
    void showPlayer() {
        this->show = true;
        this->setNewB();
    }

    void forword(int length=1) {
        this->direction=RIGHT;
        if (this->x<RIGHT_EDGE) this->x+=length;
        this->setNewB();
    }
    void backword(int length=1) {
        this->direction=LEFT;
        if (this->x>LEFT_EDGE) this->x-=length;
        this->setNewB();
    }
    void upword(int length=1) {
        this->direction=UP;
        if (this->y>UP_EDGE) this->y-=length;
        this->setNewB();
    }
    void downword(int length=1) {
        this->direction=DOWN;
        if (this->y<DOWN_EDGE) this->y+=length;
        this->setNewB();
    }

    short getHealth(){
        return this->health;
    }
    void setHealth(short newHealth){
        if (newHealth<=MAX_HEALTH) this->health=newHealth;
    }
    void addHealth(const short length){
        this->health += min(length,(short)5);
        if ((this->health)>MAX_HEALTH) this->health = MAX_HEALTH;
        this->setNewB();
        debugs = "Player? addHealth "+to_string(length);
    }
    Direction getDirection(){
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
        if (this->newb && (this->bullet_num<MAX_BULLETS)) {
            this->bullet_num++;
            this->newb = (this->bullet_num<MAX_BULLETS);
        }
        // 绘制边缘
        SetConsoleTextAttribute(STDOUT_HANDLE,0x07);
        if (x == RIGHT_EDGE) {
            setConsolePos(x+1,y);
            printf("|");
        }
        if (y == UP_EDGE) {
            setConsolePos(x,y-1);
            printf("-");
        }
        else if (y == DOWN_EDGE) {
            setConsolePos(x,y+1);
            printf("-");
        }
    }
    void clear(){
        setConsolePos(this->x,this->y);
        printf(" ");
        if (x!=RIGHT_EDGE) {
            printf(" ");
        }
        if (x!=LEFT_EDGE) {
            setConsolePos(x-1,y);
            printf(" ");
        }
        if (y!=UP_EDGE) {
            setConsolePos(x,y-1);
            printf(" ");
        }
        if (y!=DOWN_EDGE) {
            setConsolePos(x,y+1);
            printf(" ");
        }
    }
};

Player player;
Player player2 = Player(RIGHT_EDGE,DOWN_EDGE);

class Bullet{
    Direction direction;
    Player *from;
    public:
    short x,y;
    bool isExt;
    Bullet(Player *with){
        this->from = with;
        this->direction = from->getDirection();
        this->isExt=true;

        this->x=from->x;
        this->y=from->y;
        if (this->from->newb) this->isExt=false;
        if (this->from == &player) {
            debugs = "player1 bulleting";
        }
        if (this->from == &player2) {
            debugs = "player2 bulleting";
        }
    }
    void update(){
        // if (this->x!=this->from.x && this->y!=this->from.y) {
        
        // }
        if (this->x<LEFT_EDGE || this->y<UP_EDGE || this->x>RIGHT_EDGE || this->y>DOWN_EDGE) {
            this->isExt=false;
            this->x=-1;
            this->y=-1;
        }

        if (this->x==player.x && this->y==player.y && &player != this->from) {
            player.addHealth(-BULLET_HURT);
            this->isExt=false;
            from->addHealth(1);
        }
        else if (this->x==player2.x && this->y==player2.y && &player2 != this->from) {
            player2.addHealth(-BULLET_HURT);
            this->isExt=false;
            from->addHealth(1);
        }
        if (this->direction==RIGHT) this->x++;
        else if (this->direction==LEFT) this->x--;
        else if (this->direction==DOWN) this->y++;
        else if (this->direction==UP) this->y--;
    }
    void draw(){
        if (this->isExt){
            setConsolePos(x,y);
            printf("*");
        }
    }

};
list<Bullet> bulletList;
list<Item> itemList;


class Item{
    Direction direction;
    ItemTypes type;
    public:
    short x,y;
    bool isExt;
    Item(ItemTypes type,short x,short y,Direction direction=RIGHT){
        this->isExt=true;
        this->type =type;
        this->x=x;
        this->y=y;
        debugs = string("Item ")+char(type)+string(" in ")+to_string(x)+","+to_string(y);
    }
    void update(){
        if (!this->isExt) return;
        if (this->x==player.x && this->y==player.y) {
            switch (this->type) {
                case HEALTHY:player.addHealth(HEALTH_NUM);break;
                default:break;
            }
            this->isExt=false;
            this->x=-1;
            this->y=-1;
        }
        else if (this->x==player2.x && this->y==player2.y) {
            switch (this->type) {
                case HEALTHY:player2.addHealth(HEALTH_NUM);break;
                default:break;
            }
            this->isExt=false;
            this->x=-1;
            this->y=-1;
        }
        if (!bulletList.empty()) {
            if (this->x==bulletList.begin()->x && this->y==bulletList.begin()->y) {
                this->isExt = false;
                bulletList.begin()->isExt = false;
                debugs = "A Bullet -> item";
            }
        }
    }
    void draw(){
        if (this->isExt){
            setConsolePos(x,y);
            printf("%c",char(this->type));
        }
    }
};


void keyboard(){
    player.CD++;
    player2.CD++;
    if (!over && !stop && !starting){
        // 玩家1移动
        if (!aiControl1){
            if (GetAsyncKeyState('W') & 0x8000) player.upword();
            if (GetAsyncKeyState('A') & 0x8000) player.backword();
            if (GetAsyncKeyState('S') & 0x8000) player.downword();
            if (GetAsyncKeyState('D') & 0x8000) player.forword();
        }

        // 玩家2移动
        if (!aiControl) {
            if (GetAsyncKeyState('I') & 0x8000) player2.upword();
            if (GetAsyncKeyState('J') & 0x8000) player2.backword();
            if (GetAsyncKeyState('K') & 0x8000) player2.downword();
            if (GetAsyncKeyState('L') & 0x8000) player2.forword();
        }

        // 玩家1、2攻击
        if ((GetAsyncKeyState('Z') & 0x8000) && player.CD>=MAX_CD && player.bullet_num>0 && !aiControl1) {
            bulletList.push_back(Bullet(&player));
            player.setNewB(false);
            player.bullet_num--;
            player.CD = 0;

        } // 玩家1 攻击
        if ((GetAsyncKeyState('M') & 0x8000) && player2.CD>=MAX_CD && player2.bullet_num>0 && !aiControl) {
            bulletList.push_back(Bullet(&player2));
            player2.setNewB(false);
            player2.bullet_num--;
            player2.CD=0;
        } // 玩家2 攻击

        if (GetAsyncKeyState('X') & 0x8000) player.setNewB(true);
        if (GetAsyncKeyState('N') & 0x8000) player2.setNewB(true);

        // 暂停
        if (GetAsyncKeyState(ESC) & 0x8000) stop = !stop;
    }
    else if (starting){
        if (GetAsyncKeyState('1') & 0x8000) starting = false;
        else if (GetAsyncKeyState('2') & 0x8000) {
            starting = false;
            aiControl = true;
        }
        else if (GetAsyncKeyState('3') & 0x8000) {
            starting = false;
            aiControl = true;
          
            aiControl1 = true;
        }
        time(&startTime);
    }
    else if (stop){
        if (GetAsyncKeyState('1') & 0x8000) exit(0);
        else if (GetAsyncKeyState('2') & 0x8000) stop=false;
        else if (GetAsyncKeyState('3') & 0x8000) {
            aiControl = !aiControl;
            stop=false;
            debugs = string("AI for player2: ")+((aiControl)?"ON":"OFF");
        }
        else if (GetAsyncKeyState('4') & 0x8000) {
            aiControl1 = !aiControl1;
            stop=false;
            debugs = string("AI for player1: ")+((aiControl1)?"ON":"OFF");
        }
        else if (GetAsyncKeyState('5') & 0x8000) {
            if (lang["Name"][0] == 'C') lang = englishPack;
            else if (lang["Name"][0] == 'E') lang = chinesePack;
        }
        else if (GetAsyncKeyState('6') & 0x8000) restart();
    }
    else {
        // 退出
        if (GetAsyncKeyState(ESC) & 0x8000) exit(0);
        else if (GetAsyncKeyState(' ') & 0x8000) restart();
    }
}

void update(){
    if (stop || over || starting) return;
    aifps = !aifps;
    if (player.getHealth()<=0 || player2.getHealth()<=0){
        over = true;
    }
    if (aiControl && aifps) updateAi(player2,player);
    if (aiControl1 && aifps) updateAi(player,player2);

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

    list<Item>::iterator item = itemList.begin();
    for (item;item!=itemList.end();item++) {
        if (!((*item).isExt)) {
            itemList.erase(item);
            break;
        }
    }
    item=itemList.begin();
    for (item;item!=itemList.end();item++) {
        if ((*item).isExt) {
            (*item).update();
        }
    }


    srand(time(NULL));
    if (rand()%25 == 5 && itemList.size() < 24) {
        itemList.push_back(Item(HEALTHY,random(LEFT_EDGE,RIGHT_EDGE),random(UP_EDGE,DOWN_EDGE)));
    }
}

void showPlayerHealth(Player player,short line=0) {
    int step=20;
    int s=MAX_HEALTH/step;
    int startx = 0;
    SetConsoleTextAttribute(STDOUT_HANDLE,0x88);
    setConsolePos(16,line);
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
    printf(" | %3hd/%3u\n",player.getHealth(),MAX_HEALTH);

    SetConsoleTextAttribute(STDOUT_HANDLE,0x07);
}

void draw(){
    printf("Players:2\n");

    printf("Player1 %s: ",lang["hud.health.text"]);
    showPlayerHealth(player,1);

    printf("Player2 %s: ",lang["hud.health.text"]);
    showPlayerHealth(player2,2);

    printf("Player1 %s: %3hd/%3hd\n",lang["hud.bullet.text"],player.bullet_num,MAX_BULLETS);
    printf("Player2 %s: %3hd/%3hd\n",lang["hud.bullet.text"],player2.bullet_num,MAX_BULLETS);
    printf("%s: %s vs %s\n",lang["hud.mode.text"],(aiControl1)?"AI":"Player",(aiControl)?"AI":"Player");

    player.draw('A');
    player2.draw('B');

    SetConsoleTextAttribute(STDOUT_HANDLE,0x07);

    list<Bullet>::iterator bullet = bulletList.begin();
    for (bullet;bullet!=bulletList.end();bullet++) {
        if ((*bullet).isExt) {
            (*bullet).draw();
        }
    }

    list<Item>::iterator item = itemList.begin();
    for (item;item!=itemList.end();item++) {
        if ((*item).isExt) {
            (*item).draw();
        }
    }

    if (over) {
        setConsolePos(45,10);
        SetConsoleTextAttribute(STDOUT_HANDLE,0x0F);
        printf("Game Over!");
        setConsolePos(45,11);
        printf("Winner: Player%s",(player.getHealth() <= 0)?"2":"1");
        setConsolePos(47,13);
        printf(lang["over.info"]);
    }
    if (stop) {
        setConsolePos(45,15);
        printf(lang["PauseMenu.title"]);
        setConsolePos(47,17);
        printf("1 : %s",lang["PauseMenu.1"]);
        setConsolePos(47,18);
        printf("2 : %s",lang["PauseMenu.2"]);
        setConsolePos(47,19);
        printf("3 : %s",lang["PauseMenu.3"]);
        setConsolePos(47,20);
        printf("4 : %s",lang["PauseMenu.4"]);
        setConsolePos(47,21);
        printf("5 : %s",lang["PauseMenu.5"]);
        setConsolePos(47,22);
        printf("6 : %s",lang["PauseMenu.6"]);
    }
    if (starting) {
        setConsolePos(45,15);
        printf(lang["StartMenu.title"]);
        setConsolePos(47,17);
        printf("1 : Player vs Player");
        setConsolePos(47,18);
        printf("2 : Player vs AI");
        setConsolePos(47,19);
        printf("3 : AI vs AI");
    }

    SetConsoleTextAttribute(STDOUT_HANDLE,0x0f);
    setConsolePos(0,DOWN_EDGE+2);
    
    time(&timep);
    if (!over && !starting) t = timep-startTime;

    printf("%s: %02lld:%02lld\n",lang["hud.PlayerTime.title"],t/60,t%60);
    printf("%s: %s\n",lang["hud.SystemTime.title"],ctime(&timep));
    if (__argc >= 2) if (__argv[1][1] == 'D') {
        printf("[Debug] %s",debugs.c_str());
    }
}

void init(){
    over=false;
    stop=false;
    starting = true;
    SetConsoleTitle("Friend War Inside-1302");
    SetConsoleTextAttribute(STDOUT_HANDLE,0x07);
    printf(" ----------------------------------\n");Sleep(11);
    printf(" |                                |\n");Sleep(10);
    printf(" |                                |\n");Sleep(10);
    printf(" |            Friend War          |\n");Sleep(11);
    printf(" |                                |\n");Sleep(10);
    printf(" |                                |\n");Sleep(10);
    printf(" ----------------------------------\n");Sleep(11);
    printf(" Version Inside-1302    Made in Tianxun\n\n\n");Sleep(10);
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
    printf("                      Developer Options                      \n");Sleep(10);
    SetConsoleTextAttribute(STDOUT_HANDLE,0x70);
    printf("                       Regstiger error                       \n");
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

void updateAi(Player& player,Player& other) {
    /*
    if (player2.CD>=MAX_CD && player2.bullet_num>0) {
        bulletList.push_back(Bullet(&player2));
        player2.setNewB(false);
        player2.bullet_num--;
        player2.CD=0;
    } // 玩家2 攻击
    */
    if (player.x < other.x) {
        player.forword();
        if (player.CD>=MAX_CD && player.bullet_num>0 && player.y == other.y && !player.newb) {
            bulletList.push_back(Bullet(&player));
            player.setNewB(false);
            player.bullet_num--;
            player.CD=0;
        } 
    }
    else if (player.x > other.x) {
        player.backword();
        if (player.CD>=MAX_CD && player.bullet_num>0 && player.y == other.y && !player.newb) {
            bulletList.push_back(Bullet(&player));
            player.setNewB(false);
            player.bullet_num--;
            player.CD=0;
        } 
    }
    else if (player.y < other.y) {
        player.downword();
        if (player.CD>=MAX_CD && player.bullet_num>0 && !player.newb) {
            bulletList.push_back(Bullet(&player));
            player.setNewB(false);
            player.bullet_num--;
            player.CD=0;
        } 
    }
    else if (player.y > other.y) {
        player.upword();
        if (player.CD>=MAX_CD && player.bullet_num>0 && !player.newb) {
            bulletList.push_back(Bullet(&player));
            player.setNewB(false);
            player.bullet_num--;
            player.CD=0;
        } 
    }
    if (player.bullet_num < 5) {
        player.setNewB(true);
    }
    if (player.x == other.x && player.y == other.y) {
        debugs = "AI Fleing";
        srand(time(NULL)+rand());
        switch (rand()%4){
            case 0:player.forword();
            case 1:player.upword();
            case 2:player.downword();
            case 3:player.backword();
        
            default:break;
        }
    }
    else if (player.getHealth() < 90){
        list<Item>::iterator item = itemList.begin();
        for (item;item != itemList.end();item++) {
            short yd = player.y-item->y;   
            short xd = player.x-item->x;
            bool f = true;
            if (xd == 1 && yd == 0) player.forword();   
            else if (xd == -1 && yd == 0) player.backword();   
            else if (yd == 1 && xd == 0) player.upword();   
            else if (yd == -1 && xd == 0) player.downword(); 
            else f = false;
            if (f) {
                debugs = "AI trying eat HEALTH";
                break; 
            } 
        }
    }
}

void restart() {
    debugs = "Restart";
    over = false;
    over=false;
    stop=false;
    starting = true;
    player = Player();
    player2 = Player(RIGHT_EDGE,DOWN_EDGE);
    bulletList.clear();
    itemList.clear();
    system("color ff");
    t = 0;
    system("cls");
    Sleep(1000);
    system("color 7");
}

int main(){
    system("chcp 65001");  // 设置Utf-8,Dev-Cpp请删除这一行
    system("cls");
    init();
    SetConsoleTextAttribute(STDOUT_HANDLE,0x07);
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