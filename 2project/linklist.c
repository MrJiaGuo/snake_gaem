#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "linklist.h"
#include "init.h"

#define RED 0xff0000 //红色
#define GREEN 0xff00 //绿色
#define BLUE 0xff //蓝色
#define BLACK 0x000000 //黑色
#define YELLOW 0xffff00 //黄色
#define Magenta 0xff00ff //紫红
#define Cyan 0x00ffff //青色
#define WHITE 0xffffff //白色
#define Medium 0x00F5FF
#define COLOR1 0x00C5CD
#define COLOR2 0x76EEC6
#define COLOR3 0X7A67EE
#define COLOR4 0xCD00CD
#define COLOR5 0XFF3E96

//全局变量声明
extern int m,n;
extern int num1;
extern int num2;
extern int num3;
extern int snake_number;

int c=0;//颜色的下标

//13种颜色，让蛇循环变换颜色
int color[13]={YELLOW,Medium,COLOR1,RED,GREEN,COLOR2,Magenta,COLOR5,WHITE,COLOR3,Cyan,COLOR4,BLUE};

//创建链表
snake_t create_snake()
{
	
	snake_t head = (snake_t)malloc(sizeof(snake));
	if(head)
	{
		head->x=0;
		head->y=0;
		head->prev = head;
		head->next = head;
		
	}
	
	return head;
}
//销毁链表
snake_t destroy_snake(snake_t *phead)
{
	snake_t head = (*phead)->next,p=NULL;
	
	while(head!=*phead)
	{
		p=head;
		head=head->next;
		free(p);
		
	}
	free(head);
	*phead = NULL;
	p=NULL;
	
}

//插入链表函数
snake_t insert_snake(snake_t p,int x,int y)
{
	if(!p)
		return NULL;
	snake_t newnode = (snake_t)malloc(sizeof(snake));
	if(newnode)
	{
		newnode->x=x;
		newnode->y=y;
		//将新节点的前置指向p,后置指向p的下一个节点
		newnode->prev = p;
		newnode->next = p->next;
		//将p的后置指向新节点，将P的下一个节点的前置指向新节点
		p->next->prev = newnode;
		p->next = newnode;
		
	}
	snake_number++;
}

//遍历蛇身函数
snake_t travel_snake(snake_t head)
{
	int i,j;
	int k=0;
	int p=0;
	//略过无效头节点
	snake_t tmp = head->next;
	while(tmp!=head){
			//在lcd屏幕上，让蛇身显示位为棱形，并且颜色循环变换
			for(i=800*tmp->y;i<800*(tmp->y+5);i+=800)
			{
				k++;
				for(j=tmp->x;j<tmp->x+2*k;j++)
					mem_p[i+j+5-k] =color[c];
			}	
			for(i=800*(tmp->y+5);i<800*(tmp->y+10);i+=800)
			{
				
				for(j=tmp->x;j<tmp->x+2*k;j++)
					mem_p[i+j+p] =color[c];
				k--;
				p++;
			}	
			c++;
			k=0;
			p=0;
			if(c==13)
				c=0;
		tmp = tmp->next;
	}
	
	
}




