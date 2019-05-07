#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <linux/input.h>

#define RED 0xff0000 //红色
#define GREEN 0xff00 //绿色
#define BLUE 0xff //蓝色
#define BLACK 0x000000 //黑色
#define YELLOW 0xffff00 //黄色
#define Magenta 0xff00ff //紫红
#define Cyan 0x00ffff //青色
#define WHITE 0xffffff //白色

//定义一个事件结构体
struct input_event ts_evt;

int main()
{
    int x = 0,y = 0;

    int fd = open("/dev/input/event0",O_RDWR);
    if(fd==-1){
        perror("open");
        exit(-1);
    }


    int fd1 = open("/dev/fb0",O_RDWR);
    if(fd1==-1){
        perror("open");
        exit(-1);
    }

    //映射显存
    unsigned int *mem_p = mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,fd1,0);

    if(mem_p==MAP_FAILED){
        perror("mmap");
        exit(-2);
    }

    int i,num=0;
    while(1)
    {
         while(1)
         {
            //阻塞
            read(fd,&ts_evt,sizeof(struct input_event));
            //判断是否是坐标事件
            if(ts_evt.type==EV_ABS){
                //X坐标
				usleep(100000);
                if(ts_evt.code==ABS_X){
                    x = ts_evt.value;
					
                }
                //Y坐标
                if(ts_evt.code==ABS_Y){
                    y = ts_evt.value;
					usleep(100000);
                }
            }
				usleep(100000);
            //同步事件
            if(ts_evt.type==EV_SYN)
                break;
        }
		
		usleep(100000);
		
        if(x>=400)
        {
			if(num==0)
			{
				  for(i=0;i<800*480;i++)
				  {
					mem_p[i] = RED;
				  }
				  
				  num++;
				  usleep(100000);
			} 

			else if(num==1)
			{
				 for(i=0;i<800*480;i++)
				  {
					mem_p[i] = GREEN;
				  }
				  
				  num++;
				 usleep(100000);
				
			}
			
			else if(num==2)
			{
				 for(i=0;i<800*480;i++)
				  {
					mem_p[i] = BLACK;
				  }
				  
				  num++;
				 
				usleep(100000);
			}
			
			else if(num==3)
			{
				 for(i=0;i<800*480;i++)
				  {
					mem_p[i] = YELLOW;
				  }
				  
				  num=0;
				  
				usleep(100000);
			}

			
        }
        else
        {
			if(num>0)
				num--;
			else
				num=3;
        }

    }
   


    munmap(mem_p,800*480*4);
    close(fd);
    close(fd1);

    return 0;
}

