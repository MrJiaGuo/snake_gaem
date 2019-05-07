/*
方法1：使用输出子系统模型来实现
注意y坐标是颠倒的，所以在计算的时候需要修正
比如： 45<y<109  --->修正成  479-109 < y < 479-45
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <linux/input.h>



struct input_event g_ts_event;

int lcd_open(int *fd,unsigned int **mem_p)
{
	fd = open("/dev/fb0",O_RDWR);
    if(fd1==-1){
        perror("open lcd");
        exit(-1);
    }
	
	*mem_p = mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,fd1,0);

    if(mem_p==MAP_FAILED){
        perror("mmap");
        exit(-2);
    }
	
}

int ts_open(int *fd1)
{
	fd1 = open("/dev/input/event0",O_RDONLY);
	if(fd1  == -1)
	{
		perror(" Open ts");
		return -1;
	}
}

int	ts_xy_get(int *x,int *y)
{
	static int ts_get_xy_count=0;
	
	int count;
	
	while(1)
	{
		/* 调用read函数,获取触摸屏输入事件报告 */	
		count = read(g_ts_fd,&g_ts_event,sizeof(struct input_event));
		
		/* 检查当前读取的事件报告是否读取完整 */
		if(count != sizeof(struct input_event))
		{
			perror("read error");
			return -1;
		}	

		/* 检查当前响应事件是否坐标值事件 */
		if(EV_ABS == g_ts_event.type)
		{
			/* 打印x坐标 */
			if(g_ts_event.code == ABS_X)
			{
				ts_get_xy_count ++;
				
				*x = g_ts_event.value;
				
				//printf("x = : %d\n",g_ts_event.value);				
			}

			
			/* 打印y坐标 */
 			if(g_ts_event.code == ABS_Y)
			{
				ts_get_xy_count ++;
				
				*y = g_ts_event.value;	
				
				//printf("y = : %d\n",g_ts_event.value);					
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

int ts_close()
{
	close(g_ts_fd);
}

int ts_is_released(void)
{
	int count;

	int i=5;
	
	while(i--)
	{
		/* 调用read函数,获取触摸屏输入事件报告 */	
		count = read(g_ts_fd,&g_ts_event,sizeof(struct input_event));
		
		/* 检查当前读取的事件报告是否读取完整 */
		if(count != sizeof(struct input_event))
		{
			perror("read error");
			
			return -1;
		}	

		if(EV_KEY == g_ts_event.type)
		{
			if(g_ts_event.code == BTN_TOUCH)
			{
				if(g_ts_event.value == 1)
				{
					printf("touch is pressed\n");
					
					return 0;
					
				}	
				else
				{
					printf("touch is released\n");
					
					return 1;
					
				}
					
			}

		} 		
		
	}
	return 2;
}