#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <signal.h>
#include <sys/time.h>
#define NUM 60

struct direct                //�p���\�������I
{
    int cx;
    int cy;
};
typedef struct node            //?�\�I?�_
{
    int cx;
    int cy;
    struct node *back;
    struct node *next;
}node;

void initGame();            //���n����?
int setTicker(int);            //?�u??��
void show();                //?���������
void showInformation();        //?����?�M���i�O?�s�j
void showSnake();            //?���֓I�g��
void getOrder();            //��??��?�施��
void over(int i);            //������??���@�I�񎦐M��

void creatLink();                //�i??��?�_�j�o��?�\�ȋy���I����
void insertNode(int x, int y);   
void deleteNode();
void deleteLink();

int ch;                                //?���I����
int hour, minute, second;            //?���b
int length, tTime, level;            //�i�֓I�j?�x�C??��C�i��?�j��?
struct direct dir, food;            //�֓I�O?�����C�H���I�ʒu
node *head, *tail;                    //?�\�I?��?�_

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
    cbreak();                    //�c?�[�ICBREAK�͎���?
    noecho();                    //??��?
    curs_set(0);                //�c��?�u?�s��?
    keypad(stdscr, true);        //�g�p�p??�[�I??��I��??
    srand(time(0));                //?�u������?�q
    //���n���e?����
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

//?�u??��i?��������?�{��I��q�C�L��?�j
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
    //?��??
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
    //?��?�x�C��?
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

//�֓I�\�����p�꘢??��?�_�I�o��?�\���\���I�C
//�֓I?�ꎟ�O?�C�s����?�\�I?�������꘢?�_�C�ݔ���?���꘢?�_
//�@�ʎ֋h���꘢�H���C�ߏA�s�p?��?�_��
void showSnake()
{
    if(1 != tTime % (30-level))
        return;
    //���f�֓I?�x�L�v�L��?
    bool lenChange = false;
    //?���H��
    move(food.cy, food.cx);
    printw("@");
    //�@�ʎ�?��?�C?��??��
    if((COLS-1==head->next->cx && 1==dir.cx)
        || (0==head->next->cx && -1==dir.cx)
        || (LINES-1==head->next->cy && 1==dir.cy)
        || (2==head->next->cy && -1==dir.cy))
    {
        over(1);
        return;
    }
    //�@�ʎ�??�����ȓI�g�́C?��??��
    if('*' == mvinch(head->next->cy+dir.cy, head->next->cx+dir.cx) )
    {
        over(2);
        return;
    }
    insertNode(head->next->cx+dir.cx, head->next->cy+dir.cy);
    //�֋h���꘢�g�H���h
    if(head->next->cx==food.cx && head->next->cy==food.cy)
    {
        lenChange = true;
        length++;
        //����?�C��?��
        if(length >= 50)
        {
            over(3);
            return;
        }
        //�d�V?�u�H���I�ʒu
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
    signal(SIGALRM, show);        //?�u���f�M��
    showInformation();
    showSnake();
    refresh();                    //���V�^?����
}

void getOrder()
{
    //�����꘢���z?�C��?�旈��??�I����
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
    //?��?������
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
    setTicker(0);                //????��
    deleteLink();                //?��?�\�I��?
}

//?���꘢�o��?�\
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

//��?�\�I?���i��??�_�j�����꘢?�_
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

//?��?�\�I�i���?�_�I�j�ō@�꘢?�_
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

//?������?�\
void deleteLink()
{
    while(head->next != tail)
        deleteNode();
    head->next = tail->back = NULL;
    free(head);
    free(tail);
}