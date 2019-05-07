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


//定义二维数组存放所有数字图片的路径
char numpath[10][20]={
	"/haida/0.bmp",
	"/haida/1.bmp",
	"/haida/2.bmp",
	"/haida/3.bmp",
	"/haida/4.bmp",
	"/haida/5.bmp",
	"/haida/6.bmp",
	"/haida/7.bmp",
	"/haida/8.bmp",
	"/haida/9.bmp"
};

//触摸屏触控事件
struct input_event g_ts_event;

//映射内存
unsigned int *mem_p;

int fd,fd1;//文件描述符
int i,j;  //像素点位置
int m,n;  //蛇头的诞生位置，随机数
int food_m,food_n;//食物的诞生位置，随机数

int flag=1;//标志位

//吃掉食物后尾插法的蛇身位置
int num_x=0;
int num_y=0;

//分数
int num1=0;
int num2=0;
int num3=0;

//蛇身长度
int snake_number=0;

//存储上一个触摸坐标
int t_x,t_y;

//触摸坐标
int ts_x,ts_y;

//线程标志位
int start = 1;
int over = 1;
int touch =1;
int super = 0;
int flag1 = 0;

int d;//方向

extern pthread_cond_t cond;

//加载lcd驱动
int lcd_open()
{
	fd = open("/dev/fb0",O_RDWR);
    if(fd==-1){
        perror("open lcd");
        return -1;
    }
	
	mem_p = mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

    if(mem_p==MAP_FAILED){
        perror("mmap");
        return -1;
    }
	
}

//触摸事件驱动
int ts_open()
{
	fd1 = open("/dev/input/event0",O_RDONLY);
	if(fd1  == -1)
	{
		perror(" Open ts");
		return -1;
	}
}


//获取触摸屏坐标
int	ts_xy_get(int *x,int *y)
{
	static int ts_get_xy_count=0;
	
	int count;
	
	while(1)
	{
		/* 调用read函数,获取触摸屏输入事件报告 */	
		count = read(fd1,&g_ts_event,sizeof(struct input_event));
		
		/* 检查当前读取的事件报告是否读取完整 */
		if(count != sizeof(struct input_event))
		{
			perror("read error");
			return -1;
		}	

		/* 检查当前响应事件是否坐标值事件 */
		if(EV_ABS == g_ts_event.type)
		{
			/* x坐标 */
			if(g_ts_event.code == ABS_X)
			{
				ts_get_xy_count ++;
				
				*x = g_ts_event.value;
				
			}
			
			/* y坐标 */
 			if(g_ts_event.code == ABS_Y)
			{
				ts_get_xy_count ++;
				
				*y = g_ts_event.value;	
							
			}	
			
			if(ts_get_xy_count == 2)
			{
				ts_get_xy_count = 0;
				break;
			}
		}
	}
	return 0;
}

//开始界面相关函数
int game_window()
{
	if(ts_x>285&&ts_x<520&&ts_y>365&&ts_y<480)
	{
		start = 0;
	}
	
	if(ts_x>666&&ts_x<785&&ts_y>62&&ts_y<165)
	{
		exit(0);
	}
}

//游戏结束相关函数
int game_over()
{
	if(ts_x>273&&ts_x<537&&ts_y>372&&ts_y<456)
	{
		over = 0;
		d=0;
		
	}
	if(ts_x>640&&ts_x<790&&ts_y>20&&ts_y<150)
	{
		system("killall -KILL madplay");
		exit(0);
	}
	
	
}

//播放音乐
int play_music()
{
		if(flag==1)
		{
			system("madplay /haida/fade.mp3 &");
			flag=0;
		}
		else
		{
			system("killall -KILL madplay");
			flag=1;
		}
}


//显示分数
int show_score(int num1,int num2,int num3)
{
	//百位
	show_shapebmp(540,90,80,80,numpath[num3]);
	//十位
	show_shapebmp(620,90,80,80,numpath[num2]);
	//个位
	show_shapebmp(700,90,80,80,numpath[num1]);
	
	
}

//蛇移动函数
snake_t snake_move(snake_t head,snake_t tmp)
{
	for(i=800*tmp->y;i<800*(tmp->y+10);)
	{
		for(j=tmp->x;j<tmp->x+10;j++)
			mem_p[i+j] =BLACK;
		i+=800;
	}	
		
	while(tmp!=head->next)
    {
        tmp->y = tmp->prev->y;
        tmp->x = tmp->prev->x;
        tmp = tmp->prev;
    }
	
	return tmp;
}

//设置方向
int direction_snake()
{
	if(ts_x>625&&ts_x<695&&ts_y>300&&ts_y<385)//上
		return 1;
	if(ts_x>625&&ts_x<695&&ts_y>394&&ts_y<480)//下
		return 2;
	if(ts_x>520&&ts_x<620&&ts_y>390&&ts_y<480)//左
		return 3;
	if(ts_x>700&&ts_x<800&&ts_y>390&&ts_y<480)//右
		return 4;
	
}


//蛇初始化和具体移动操作和显示
int snake_init(snake_t head)
{
	
	while(1){
		
		if(super ==0)    //等于0为手动模式，1为歼灭模式
			d =direction_snake(); //手动模式，调用触摸函数
			
		pthread_cond_signal(&cond);	//唤醒触摸屏线程
		
		snake_t tmp = head->prev;//娶蛇的尾结点
		
		if(d==2)//下
		{
			n+=10;//向下移动，行数加10
			
			num_y=-10; //在尾巴后10行插入新结点
			num_x=0;
			
			t_x=ts_x;//记录上一次坐标值
			t_y=ts_y;
			
			tmp = snake_move(head,tmp); //移动函数，每个结点往前移动
			
			tmp->y+=10; //蛇头加10像素点
			
			eat_food(head);//判断是否吃到食物的函数
			travel_snake(head);//遍历蛇函数
			interface();//边框界面
			
			if(crash_sneak_body(head)&&super==0)//判断是否到自己的身体
				return;
			
		}
		
		else if(d==1)//上
		{
			n-=10;//向上移动
			
			num_y=10;
			num_x=0;
			
			t_x=ts_x; 
			t_y=ts_y;
			
			tmp = snake_move(head,tmp);
			
			tmp->y-=10;
			eat_food(head);
			travel_snake(head);
			interface();
			
			if(crash_sneak_body(head)&&super==0)
				return;
		}
		else if(d==3)//左
		{
			m-=10;//向左移动
			num_y=0;
			num_x=10;
			
			t_x=ts_x;
			t_y=ts_y;
			
			tmp = snake_move(head,tmp);
			
			tmp->x-=10;
			eat_food(head);
			travel_snake(head);
			interface();
			
			if(crash_sneak_body(head)&&super==0)
				return;
		
		}
		else if(d==4)//右
		{
			m+=10;//向右移动
			num_y=0;
			num_x=-10;
			
			t_x=ts_x;
			t_y=ts_y;
			
			tmp = snake_move(head,tmp);
			
			tmp->x+=10;
			eat_food(head);
			travel_snake(head);
			interface();
			
			if(crash_sneak_body(head)&&super==0)
				return;

		}
		

		else if(ts_x>620&&ts_x<685&&ts_y>185&&ts_y<250)//播放音乐的坐标，点击播放，且蛇暂停移动
		{
			ts_x=0;
			ts_y=0;
			play_music();
		}
		else if(ts_x==0&&ts_y==0)//让蛇暂停的坐标
		{
			
		}
		else//触碰以上坐标以外，则回到上一次坐标的值
		{
			ts_x=t_x;
			ts_y=t_y;
		}
		
		
		
		if(n>450||n<20||m<20||m>510) //撞墙判断
		{
			printf("GAME OVER!\n");
			return;
			//exit(0);
		}
		if(super==0) //普通模式的蛇移动速度控制
		{
			if(snake_number<11) //蛇身长度少于11，则是低速
				usleep(LOW_SPEED);
			else if(snake_number>=11&&snake_number<31)//长度大于11小于31为中速
				usleep(MIDDLE_SPEED);
			else                          //大于31为高速
				usleep(HIGH_SPEED);
		}	
		if(super==1)             //歼灭模式的移动速度
			usleep(5000);
	}
}

//歼灭模式的自动移动算法
int smart_snake(snake_t head)
{
	snake_t tmp ;
	if(super == 1)
	{ 
		tmp = head->next; //获取蛇头
	}
	int l = food_m;  //保存食物的坐标
	int h = food_n;

	while(1)
	{
		if(super == 1)
		{
			printf("x:%d  y:%d  f_m:%d  f_n:%d|\n",tmp->x,tmp->y,food_m,food_n);
			if(tmp->x>food_m&&tmp->y>food_n)  //判断食物是不是在蛇头左上方
			{
				while(tmp->x!=food_m)  //判断移动后的蛇的头街的x坐标是否等于食物的x坐标
				{
					d=1;            //向上移动
					if(tmp->y==food_n)   //移动到同一行
					{
						while(tmp->x!=food_m)   
						{
							d=3;          //往左移动
							usleep(5000); 
							if(tmp->x==l)  //吃到食物返回
							{
								d=0;
								return;
							}
						}
					}
					usleep(5000);
				}
			}
			
			else if(tmp->x<food_m&&tmp->y<food_n) //判断食物是不是在蛇头右下方
			{
				
				while(tmp->x!=food_m)
				{
					d=2;//往下移动
					if(tmp->y==food_n)
					{
						while(tmp->x!=food_m)
						{
							d=4;//往右移动
							usleep(5000);
							if(tmp->x==l)
							{
								d=0;
								return;
							}
						}
					}
					usleep(5000);
				}
			}
			
			else if(tmp->x<food_m&&tmp->y>food_n)//判断食物是不是在蛇头右上方
			{
				while(tmp->x!=food_m)
				{
					d=1;
					if(tmp->y==food_n)
					{
						while(tmp->x!=food_m)
						{
							d=4;
							usleep(5000);
							if(tmp->x==l)
							{
								d=0;
								return;
							}
						}
					}
					usleep(5000);
				}
			}
			
			else if(tmp->x>food_m&&tmp->y<food_n)//判断食物是不是在蛇头左下方
			{
				while(tmp->x!=food_m)
				{
					d=2;
					if(tmp->y==food_n)
					{
						while(tmp->x!=food_m)
						{
							d=3;
							usleep(5000);
							if(tmp->x==l)
							{
								d=0;
								return;
							}
						}
					}
					usleep(5000);
				}
				
			}
			
			if(tmp->x==l&&tmp->y>h)//判断食物是不是在蛇头上方
			{
				while(tmp->y!=h)
				{
					d=1;
					if(tmp->y==h)
					{
						d=0;
						return;
					}
					usleep(5000);
				}
				
			}
			
			if(tmp->x==l&&tmp->y<h)//判断食物是不是在蛇头下方
			{
				while(tmp->y!=h)
				{
					d=2;
					if(tmp->y==h)
					{
						d=0;
						return;
					}
					usleep(5000);
				}
				
			}
			
			if(tmp->x>l&&tmp->y==h)//判断食物是不是在蛇头左方
			{
				while(tmp->x!=l)
				{
					d=3;
					if(tmp->x==l)
					{
						d=0;
						return;
					}
					usleep(5000);
				}
				
			}
			
			if(tmp->x<l&&tmp->y==n)//判断食物是不是在蛇头右方
			{
				while(tmp->x!=l)
				{
					d=4;
					if(tmp->x==l)
					{
						d=0;
						return;
					}
					usleep(5000);
				}
				
			}
		}
	break;
	}
	return ;
}

//食物初始化
int food_init(snake_t head)
{
	snake_t tmp = head->prev;	

	food_m=(rand()%50)*10+20;
	food_n=(rand()%44)*10+20;
	
	
	//判断食物是否在蛇身出现
	while(tmp!=head)
	{
		
		if(food_n==tmp->x&&food_m==tmp->y)
		{
			food_init(head);
		}
		tmp = tmp->prev;
	}
	//打印食物
	for(i=800*food_n;i<800*(food_n+10);)
	{
		for(j=food_m;j<food_m+10;j++)
			mem_p[i+j] = GREEN;
		i+=800;
	}
	
}

//吃到食物函数判断
int eat_food(snake_t head)
{
	snake_t tmp = head->next;
	
	//当前食物消失
	if(tmp->x==food_m&&tmp->y==food_n)
	{	
		for(i=800*food_n;i<800*(food_n+10);)
		{
			for(j=food_m;j<food_m+10;j++)
				mem_p[i+j] = BLACK;
			i+=800;
		}
		
		insert_snake(head->prev,head->prev->x+num_x,head->prev->y+num_y);//吃到食物就在尾巴插入新结点
		food_init(head);
		
		//分数判断
		num1++;
		if(num1==10)
		{
			num2++;
			num1=0;
		}
		if(num2==10)
		{
			num3++;
			num2=0;
	    }
	
	}
	//显示分数
	show_score(num1,num2,num3);
	
}
//判断是否撞到蛇身函数
int crash_sneak_body(snake_t head)
{
	snake_t tmp = head->prev;
	while(tmp!=head->next)
	{

		if(tmp->x==head->next->x&&tmp->y==head->next->y&&super==0)
		{
			printf("GAME OVER! crash self body!\n");
			return 1;
		}
		
		tmp = tmp->prev;
	}

	return 0;
}

//打印蛇移动地图
int snake_map()
{
	//产生随机坐标
	srand(time(0));
	m=(rand()%50)*10+20;
	n=(rand()%44)*10+20;
	
	for(i=800*20;i<800*460;)
	{
		for(j=20;j<520;j++)
			mem_p[i+j] =BLACK;
		i+=800;
	}
	
}
//打印地图边框
int interface()
{	

	for(i=0;i<800*480;)
	{
		for(j=0;j<20;j++)
			mem_p[i+j] =WHITE;
		i+=800;
	}
	
	for(i=0;i<800*480;)
	{
		for(j=780;j<800;j++)
			mem_p[i+j] =WHITE;
		i+=800;
	}
	
	for(i=0;i<800*20;)
	{
		for(j=20;j<800;j++)
			mem_p[i+j] =WHITE;
		i+=800;
	}
	
	for(i=0;i<800*480;)
	{
		for(j=520;j<540;j++)
			mem_p[i+j] =WHITE;
		i+=800;
	}
	
	for(i=800*460;i<800*480;)
	{
		for(j=20;j<800;j++)
			mem_p[i+j] =WHITE;
		i+=800;
	}
	
}

//关闭文件
int ts_close()
{
	close(fd);
	close(fd1);
}
