#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include<semaphore.h>
#include <linux/input.h>
#include "init.h"
#include "linklist.h"
#include "aboutbmp.h"


//相关全局变量声明
extern unsigned int *mem_p;

extern int fd;
extern int fd1;

extern int ts_x;
extern int ts_y;

extern int m,n;
extern int i,j;

extern int start;
extern int over;
extern int touch;
extern int super;
extern int flag1;
extern int d;

extern int num1,num2,num3;

extern int snake_number;

pthread_t id,id1,id2,id3;//线程id

pthread_mutex_t lock;
pthread_cond_t cond;

snake_t head; //创建头节点


//触摸屏控制线程函数
void *ts_touch(void *arg)
{
		while(touch)
		{
			pthread_mutex_lock(&lock);
			//线程等待唤醒
			pthread_cond_wait(&cond,&lock);
			
			ts_xy_get(&ts_x,&ts_y);	
			printf("x:%d   y:%d\n",ts_x,ts_y);
			printf("flag1:%d  touch:%d super:%d\n",flag1,touch,super);		
					
			if(ts_x>565&&ts_x<750&&ts_y>280&&ts_y<310)  //歼灭模式的坐标，点击则开启歼灭模式
			{
				if(super==0)
				{
					super = 1;
		
				}
				else
				{
					super = 0;
				}
			}
			pthread_mutex_unlock(&lock);
		}
}

//智能蛇控制线程函数
void *super_snake(void *arg)
{
	while(1)
	{
		while(super)
		{
			smart_snake(head);
		}	
	}
}

//游戏开始界面线程函数
void *game_start(void *arg)
{
	while(start)
	{
		show_shapebmp(0,0,800,480,"/haida/jiemian.bmp");
		system("madplay /haida/beijing.mp3 &");
		ts_xy_get(&ts_x,&ts_y);	
		game_window();
		system("killall -KILL madplay");
	}
	
}

//游戏结束线程函数
void *game_over_show(void *arg)
{
	while(over)
	{
		
		show_shapebmp(0,0,800,480,"/haida/gameover.bmp");
		ts_xy_get(&ts_x,&ts_y);	
		game_over();
	}
	
}
//点击继续游戏后，重新初始化所有变量函数等等
void re_init()
{
	/********  回收资源  *********/	
	touch = 0;
	pthread_join(id,0);  //回收触摸屏线程资源

	pthread_cancel(id3);  

	pthread_create(&id2,0,game_over_show,0);  //结束游戏的线程创建
	pthread_join(id2,0);     //等待回收其资源
		
	destroy_snake(&head);  //销毁链表
	

	/********  资源重新初始化  *********/
	d = 0;
	super = 0;
	num1=0;
	num2=0;
	num3=0;
	snake_number=0;
	
	snake_map();
	interface();
	
	show_shapebmp(540,20,240,440,"/haida/window5.bmp");
	head = create_snake();
		
	over = 1;
	touch = 1;
	ts_x=0,ts_y=0;
	
	pthread_create(&id,0,ts_touch,0);
	pthread_create(&id3,0,super_snake,0);
	
	insert_snake(head->prev,m,n);
	
	food_init(head);
	
	travel_snake(head);	
}



int main()
{
	lcd_open();
	ts_open();

	pthread_create(&id1,0,game_start,0); //开始界面线程创建
	pthread_join(id1,0);    //点击开始游戏后，回收线程资源
	
	pthread_mutex_init(&lock,0);
    pthread_cond_init(&cond,0);
	
	
	pthread_create(&id3,0,super_snake,0); //创建歼灭模式线程
	
	snake_map();
	interface();
	show_shapebmp(540,20,240,440,"/haida/window5.bmp");

	head = create_snake();  //创建链表

    pthread_create(&id,0,ts_touch,0);  //创建触摸屏线程
	
	insert_snake(head->prev,m,n);  //插入蛇头
	
	food_init(head);   //食物初始化
	
	travel_snake(head);  //遍历蛇头 

	while(1)
	{
		//pthread_cond_broadcast(&cond);
		snake_init(head);   //移动函数
		re_init();    //继续游戏，资源初始化函数
			
	}
	// 解除内存映射
	munmap(mem_p,800*480*4);
	
	destroy_snake(&head);
	pthread_join(id,0); 
	pthread_join(id1,0);
	pthread_join(id2,0); 
	pthread_join(id3,0); 			
    close(fd);
    close(fd1);
	return 0;
}
