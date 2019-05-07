#ifndef _LINKLIST_H
#define _LINKLIST_H

typedef struct node
{
	int x;
	int y;
	struct node *prev; //前置指针
	struct node *next; //后置指针
	
}snake,*snake_t;

snake_t create_snake();
snake_t destroy_snake(snake_t *phead);
snake_t insert_snake(snake_t p,int x,int y);
snake_t travel_snake(snake_t head);

#endif