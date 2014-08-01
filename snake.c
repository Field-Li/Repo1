#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <signal.h>
#include <sys/time.h>
#define NUM 60

struct direct                //用来表示方向的
{
    int cx;
    int cy;
};
typedef struct node            //?表的?点
{
    int cx;
    int cy;
    struct node *back;
    struct node *next;
}node;

void initGame();            //初始化游?
int setTicker(int);            //?置??器
void show();                //?示整个画面
void showInformation();        //?示游?信息（前?行）
void showSnake();            //?示蛇的身体
void getOrder();            //从??中?取命令
void over(int i);            //完成游??束后的提示信息

void creatLink();                //（??尾?点）双向?表以及它的操作
void insertNode(int x, int y);   
void deleteNode();
void deleteLink();

int ch;                                //?入的命令
int hour, minute, second;            //?分秒
int length, tTime, level;            //（蛇的）?度，??器，（游?）等?
struct direct dir, food;            //蛇的前?方向，食物的位置
node *head, *tail;                    //?表的?尾?点

int main()
{
    initscr();
    initGame();
    signal(SIGALRM, show);
    getOrder();
    endwin();
    return 0;
}

void initGame()
{
    cbreak();                    //把?端的CBREAK模式打?
    noecho();                    //??回?
    curs_set(0);                //把光?置?不可?
    keypad(stdscr, true);        //使用用??端的??上的小??
    srand(time(0));                //?置随机数?子
    //初始化各?数据
    hour = minute = second = tTime = 0;
    length = 1;
    dir.cx = 1;
    dir.cy = 0;
    ch = 'A';
    food.cx = rand() % COLS;
    food.cy = rand() % (LINES-2) + 2;
    creatLink();
    setTicker(20);
}

//?置??器（?个函数是?本上的例子，有改?）
int setTicker(int n_msecs)
{
    struct itimerval new_timeset;
    long    n_sec, n_usecs;

    n_sec = n_msecs / 1000 ;
    n_usecs = ( n_msecs % 1000 ) * 1000L ;
    new_timeset.it_interval.tv_sec  = n_sec;        
    new_timeset.it_interval.tv_usec = n_usecs;      
    n_msecs = 1;
    n_sec = n_msecs / 1000 ;
    n_usecs = ( n_msecs % 1000 ) * 1000L ;
    new_timeset.it_value.tv_sec     = n_sec  ;      
    new_timeset.it_value.tv_usec    = n_usecs ;     
    return setitimer(ITIMER_REAL, &new_timeset, NULL);
}

void showInformation()
{
    tTime++;
    if(tTime >= 1000000)                //
        tTime = 0;
    if(1 != tTime % 50)
        return;
    move(0, 3);   
    //?示??
    printw("time: %d:%d:%d %c", hour, minute, second);
    second++;
    if(second > NUM)
    {
        second = 0;
        minute++;
    }
    if(minute > NUM)
    {
        minute = 0;
        hour++;
    }
    //?示?度，等?
    move(1, 0);
    int i;
    for(i=0;i<COLS;i++)
        addstr("-");
    move(0, COLS/2-5);
    printw("length: %d", length);
    move(0, COLS-10);
    level = length / 3 + 1;
    printw("level: %d", level);
}

//蛇的表示是用一个??尾?点的双向?表来表示的，
//蛇的?一次前?，都是在?表的?部增加一个?点，在尾部?除一个?点
//如果蛇吃了一个食物，那就不用?除?点了
void showSnake()
{
    if(1 != tTime % (30-level))
        return;
    //判断蛇的?度有没有改?
    bool lenChange = false;
    //?示食物
    move(food.cy, food.cx);
    printw("@");
    //如果蛇?到?，?游??束
    if((COLS-1==head->next->cx && 1==dir.cx)
        || (0==head->next->cx && -1==dir.cx)
        || (LINES-1==head->next->cy && 1==dir.cy)
        || (2==head->next->cy && -1==dir.cy))
    {
        over(1);
        return;
    }
    //如果蛇??到自己的身体，?游??束
    if('*' == mvinch(head->next->cy+dir.cy, head->next->cx+dir.cx) )
    {
        over(2);
        return;
    }
    insertNode(head->next->cx+dir.cx, head->next->cy+dir.cy);
    //蛇吃了一个“食物”
    if(head->next->cx==food.cx && head->next->cy==food.cy)
    {
        lenChange = true;
        length++;
        //恭喜?，通?了
        if(length >= 50)
        {
            over(3);
            return;
        }
        //重新?置食物的位置
        food.cx = rand() % COLS;
        food.cy = rand() % (LINES-2) + 2;
    }
    if(!lenChange)
    {
        move(tail->back->cy, tail->back->cx);
        printw(" ");
        deleteNode();
    }
    move(head->next->cy, head->next->cx);
    printw("*");
}

void show()
{
    signal(SIGALRM, show);        //?置中断信号
    showInformation();
    showSnake();
    refresh();                    //刷新真?屏幕
}

void getOrder()
{
    //建立一个死循?，来?取来自??的命令
    while(1)
    {
        ch = getch();
        if(KEY_LEFT == ch)
        {
            dir.cx = -1;
            dir.cy = 0;
        }
        else if(KEY_UP == ch)
        {
            dir.cx = 0;
            dir.cy = -1;
        }
        else if(KEY_RIGHT == ch)
        {
            dir.cx = 1;
            dir.cy = 0;
        }
        else if(KEY_DOWN == ch)
        {
            dir.cx = 0;
            dir.cy = 1;
        }
        setTicker(20);
    }
}

void over(int i)
{
    //?示?束原因
    move(0, 0);
    int j;
    for(j=0;j<COLS;j++)
        addstr(" ");
    move(0, 2);
    if(1 == i)
        addstr("Crash the wall. Game over");
    else if(2 == i)
        addstr("Crash itself. Game over");
    else if(3 == i)
        addstr("Mission Complete");
    setTicker(0);                //????器
    deleteLink();                //?放?表的空?
}

//?建一个双向?表
void creatLink()
{
    node *temp = (node *)malloc( sizeof(node) );
    head = (node *)malloc( sizeof(node) );
    tail = (node *)malloc( sizeof(node) );
    temp->cx = 5;
    temp->cy = 10;
    head->back = tail->next = NULL;
    head->next = temp;
    temp->next = tail;
    tail->back = temp;
    temp->back = head;
}

//在?表的?部（非??点）插入一个?点
void insertNode(int x, int y)
{
    node *temp = (node *)malloc( sizeof(node) );
    temp->cx = x;
    temp->cy = y;
    temp->next = head->next;
    head->next = temp;
    temp->back = head;
    temp->next->back = temp;
}

//?除?表的（非尾?点的）最后一个?点
void deleteNode()
{
    node *temp = tail->back;
    node *bTemp = temp->back;
    bTemp->next = tail;
    tail->back = bTemp;
    temp->next = temp->back = NULL;
    free(temp);
    temp = NULL;
}

//?除整个?表
void deleteLink()
{
    while(head->next != tail)
        deleteNode();
    head->next = tail->back = NULL;
    free(head);
    free(tail);
}