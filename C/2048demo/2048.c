/**
 * @file 2048.c
 * @author Celiachu
 * @brief 
 * @version 0.1
 * @date 2023-03-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include<stdio.h>  //标准输入输出
#include<stdlib.h>  //基本工具函数
#include <unistd.h>
#include <termios.h>
#include <time.h>// 时间
#include <pthread.h>//线程
#define bool int  //C里边没有布尔类型,就自己造
#define true 1   //bool的两种值
#define false 0
int MAP[4][4]= {{0}};  //地图,默认0认为是空位
typedef enum {   //定义一个方向类型的枚举变量
    UNKNOW,
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;
int getch();//读取上下左右键
void printMap();  //绘制图形
Direction getNextDirection(); //从键盘读入下一个用户操作
bool canMove(Direction direction); //判断是否可以进行指定方向的操作
void doAction();   //游戏事件
void move(Direction direction);  //移动数字
void putNew();    //放入一个新的数字
int main() {    //主函数
    Direction nextStep;   //下一步
    //int i=0,j=0;
    srand(time(0));
    putNew();    //游戏开始默认放两个数字
    putNew();
    printMap();    //打印格子
    while(1) {
        if(!canMove(UP)&&!canMove(LEFT)&&!canMove(DOWN)&&!canMove(RIGHT)) break; //任意方向都不能移动,那么终止游戏
        nextStep=getNextDirection();  //获取下一个用户操作
        if(nextStep==UNKNOW) continue;  //如果不知道用户按了个什么键或者用户胡乱按的,那么进入新的循环
        if(!canMove(nextStep)) continue; //如果下一步不可继续操作,进入新的循环
        system("clear");    //对于Windows来说,执行命令行命令cls清屏
        doAction(nextStep);   //执行操作
        putNew();    //放新的数字
        printMap();    //打印格子
    }
    printf("You Died!");   //提示游戏结束
    while(1);     //等待游戏结束
}

void printMap() {
    int i,j;
    printf("*-------*-------*-------*-------*\n");
    for(i=0; i<4; i++) {
        printf("|");
        for(j=0; j<4; j++) {
            MAP[i][j]?printf("%d",MAP[i][j]):printf(" ");
            printf("\t|");
            if(j>2)
                printf("\n");
        }
        printf("*-------*-------*-------*-------*\n");
    }
}
void doAction(Direction direction){
    int i,j;//,k;
    /**
     * 为了方便处理问题,将每个方向的运动操作简化为三步
     * 1.将数字归并到一个方向
     * 2.处理相同数字可消,并将消掉的数据定为0
     * 3.再次将数字归并到一个方向
     */
    //1.移动数字,取消数字之间的空位
    move(direction);
    //2.按照方向处理相同数字
    switch(direction){
        case UP:
            //按列枚举
            for(i=0;i<4;i++){
                //对于每一行的每一个元素
                for(j=0;j<3;j++){
                    //如果元素非零,并且当前和下一个相同,当前的翻倍,下一个置零
                    if(MAP[j][i]&&MAP[j][i]==MAP[j+1][i]){
                        MAP[j][i]+=MAP[j+1][i];
                        MAP[j+1][i]=0;
                    }
                }
            }
            break;
        case LEFT://同上
            for(i=0;i<4;i++)
                for(j=0;j<3;j++)
                    if(MAP[i][j]&&MAP[i][j]==MAP[i][j+1]){
                        MAP[i][j]+=MAP[i][j+1];
                        MAP[i][j+1]=0;
                    }
            break;
        case DOWN://同上
            for(i=0;i<4;i++)
                for(j=3;j>0;j--)
                    if(MAP[j][i]&&MAP[j][i]==MAP[j-1][i]){
                        MAP[j][i]+=MAP[j-1][i];
                        MAP[j-1][i]=0;
                    }
            break;
        case RIGHT://同上
            for(i=0;i<4;i++)
                for(j=3;j>0;j--)
                    if(MAP[i][j]&&MAP[i][j]==MAP[i][j-1]){
                        MAP[i][j]+=MAP[i][j-1];
                        MAP[i][j-1]=0;
                    }
            break;
        case UNKNOW:
            printf("按键错误\n");
            break;
    }
    //3.移动数字,取消因为上一步置零过程中新产生的空位
    move(direction);
}
void move(Direction direction) { //移动数字
    int i,j,k;
    switch(direction) {
        case UP:
            //按列枚举
            for(i=0;i<4;i++)
                //对于每一行的每一个元素
                for(j=0;j<4;j++)
                    //如果非零,那么应当取消当前位置,后边元素向前移动
                    if(!MAP[j][i]){
                        for(k=j;k<3;k++){
                            MAP[k][i]=MAP[k+1][i];
                        }
                        //新产生的空位置零
                        MAP[k][i]=0;
                    }
            break;
        case LEFT://同上
            for(i=0;i<4;i++)
                for(j=0;j<4;j++)
                    if(!MAP[i][j]){
                        for(k=j;k<3;k++){
                            MAP[i][k]=MAP[i][k+1];
                        }
                        MAP[i][k]=0;
                    }
            break;
        case DOWN://同上
            for(i=0;i<4;i++)
                for(j=3;j>=0;j--)
                    if(!MAP[j][i]){
                        for(k=j;k>0;k--){
                            MAP[k][i]=MAP[k-1][i];
                        }
                        MAP[k][i]=0;
                    }
            break;
        case RIGHT://同上
            for(i=0;i<4;i++)
                for(j=3;j>=0;j--)
                    if(!MAP[i][j]){
                        for(k=j;k>0;k--){
                            MAP[i][k]=MAP[i][k-1];
                        }
                        MAP[i][k]=0;
                    }
            break;
        case UNKNOW:
            printf("按键错误\n");
            break;
    }
}
bool canMove(Direction direction) { //判断是否可以进行指定方向的操作
    int i,j;
    switch(direction) {
        case UP:
            //依次检查每一列
            for(i=0;i<4;i++){
                //首先排除在远端的一串空位,直接将j指向第一个非零元素
                for(j=3;j>=0;j--)
                    if(MAP[j][i])
                        break;
                //j>0代表这一列并非全部为0
                if(j>0)
                    //依次检查每一个剩余元素,遇见空位直接返回true
                    for(;j>=0;j--)
                        if(!MAP[j][i])
                            return true;
                //依次检查相邻的元素是否存在相同的非零数字
                for(j=3;j>0;j--)
                    if(MAP[j][i]&&MAP[j][i]==MAP[j-1][i])
                        return true;
            }
            break;
        case DOWN://同上
            for(i=0;i<4;i++){
                for(j=0;j<4;j++)
                    if(MAP[j][i]) break;
                if(j<4)
                    for(;j<4;j++)
                        if(!MAP[j][i]) return true;
                for(j=0;j<3;j++)
                    if(MAP[j][i]&&MAP[j][i]==MAP[j+1][i])
                        return true;

            }
            break;
        case LEFT://同上
            for(i=0; i<4; i++){
                for(j=3;j>=0;j--)
                    if(MAP[i][j])
                        break;
                if(j>=0)
                    for(;j>=0;j--)
                        if(!MAP[i][j])
                            return true;
                for(j=0;j<3;j++)
                    if(MAP[i][j]&&MAP[i][j]==MAP[i][j+1])
                        return true;
            }
            break;
        case RIGHT://同上
            for(i=0; i<4; i++){
                for(j=0;j<4;j++)
                    if(MAP[i][j])
                        break;
                if(j<4)
                    for(;j<4;j++)
                        if(!MAP[i][j])
                            return true;
                for(j=0;j<3;j++){
                    if(MAP[i][j]&&MAP[i][j]==MAP[i][j+1])
                        return true;
                }
            }
            break;
        case UNKNOW:
            printf("按键错误\n");
            break;
    }
    //当允许条件都被检查过后,返回不可执行的结果
    return false;
}
Direction getNextDirection() {
//  printf("getch():%d\n",getch());
    //第一个字节必须是224,否则判定输入的不是功能键
//  if(getch()!=224) return UNKNOW;
    //根据第二字节对应出来用户的操作
    switch(getch()) {
        case 183:
        //  printf("UP\n");
            return UP;
        case 184:
        //  printf("DOWN\n");
            return DOWN;
        case 186:
        //  printf("LEFT\n");
            return LEFT;
        case 185:
        //  printf("RIGHT\n");
            return RIGHT;
        case 3:
            exit(EXIT_SUCCESS);
            break;
            //return 0;
        default:
            return UNKNOW;
    }
}
void putNew(){
    //为了方便操作,临时存储一下所有空闲格子的指针,这样可以用一个线性的内存随机访问实现对所有空位中任一空位的随机访问.
    int* boxes[16]={NULL};
    //用来临时保存目标格子的地址
    int* target;
    //统计一共有多少个有效空格
    int count=0;
    int i,j;
    //统计空位,发现空位即保存地址并累加计数器
    for(i=0;i<4;i++)
        for(j=0;j<4;j++)
            if(!MAP[i][j]){
                boxes[count]=&MAP[i][j];
                count++;
            }
    if(count){
        //如果有空位,那么对这一位进行随机赋值操作,对于每一位可能性是相同的
        target=boxes[rand()%count];
        //50%可能出现2 50% 可能出现4
        *target=rand()%2?2:4;
    }
}
#ifndef _PY_GETKER_H
#define _PY_GETKEY_H
int input;
pthread_t thread[1];
pthread_mutex_t mut;
char c[193]="";
void *memset();
void *regetch()
{
    int n = 0;
    n = fgetc( stdin );
    input += n;
    pthread_exit(NULL);
}
int getch( void )
{ 
    struct termios tm, tm_old; 
    int fd = STDIN_FILENO,c;

    if(tcgetattr(fd, &tm) < 0) 
        return -1; 

    tm_old = tm; 

    cfmakeraw(&tm); 

    if(tcsetattr(fd, TCSANOW, &tm) < 0) 
        return -1; 

    unsigned int k;
    int temp;
    input = fgetc( stdin );
    k = input;
    if(tcsetattr(fd,TCSANOW,&tm_old)<0) return -1;
    if(k > 32){
        c = k;
    }
    else{
        while(1)
        {
            memset(&thread,0,sizeof(thread));
            if((temp = pthread_create(&thread[0],NULL,regetch,NULL))!=0)
                printf("用于获取按键输入的线程创建失败!\n");
            usleep(1000);
            pthread_cancel(thread[0]);
            pthread_join(thread[0],NULL);
            if(k == input) {
                c=input; 
                break;
            }
            else{ 
                k = input;
                c=c+k;
            }
        }
    }
    return c; 
} 
#if 0
char *getkey(void)
{
    int n,i;
    char a;
    c[0]='"';
    n=getch();
    switch(n)
    {
        case 171:
            return "UP";
        case 172:
            return "DOWN";
        case 173:
            return "RIGHT";
        case 174:
            return "LEFT";
        case 27:
            return "ESC";
        case 13:
            return "OK";
        case 342:
            return "AA";
        case 343:
            return "BB";
        case 127:
            return "DEL";
        case 32:
            return "SPACE";
        case 39:
            return c;
    }
    if(n>96)if(n<123)
    {
        a='a';
        for(i=97;i<n;i++)a++;
        c[0]=a;
        return c;
    }
    if(n>64)if(n<91)
    {
        a='A';
        for(i=65;i<n;i++)a++;
        c[0]=a;
        return c;
    }
}
#endif
#endif