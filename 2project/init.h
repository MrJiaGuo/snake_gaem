#ifndef _INIT_H
#define _INIT_H
#include "linklist.h"

#define RED 0xff0000 //红色
#define GREEN 0xff00 //绿色
#define BLUE 0xff //蓝色
#define BLACK 0x000000 //黑色
#define YELLOW 0xffff00 //黄色
#define Magenta 0xff00ff //紫红
#define Cyan 0x00ffff //青色
#define WHITE 0xffffff //白色

#define LOW_SPEED 150000
#define MIDDLE_SPEED 100000
#define HIGH_SPEED 70000

unsigned int *mem_p;
int fd;
int fd1;
int ts_x;
int ts_y;



int lcd_open();
int ts_open();
int ts_close();

int	ts_xy_get(int *x,int *y);

int snake_init(snake_t head);
snake_t snake_move(snake_t head,snake_t tmp);
int food_init(snake_t head);
int eat_foot(snake_t head);
int crash_sneak_body(snake_t head);

int game_window();//游戏开始界面
int game_over();//游戏结束界面

int direction_snake();
int smart_snake(snake_t head);

int play_music();
int show_score(int num1,int num2,int num3);

int interface();
int snake_map();



#endif