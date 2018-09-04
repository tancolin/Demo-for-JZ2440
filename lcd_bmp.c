#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/mman.h>
 
 
//short * fb_mem;  //这块的数据类型一定要注意，如果使用就要跟一个像素匹配两个字节（这里跟LCD一个像素由多少个字节组成有关）
int lcd;
int init_lcd()
{
	lcd =  open("/dev/fb0",O_RDWR);  //1.打开LCD设备
	if(lcd == -1)
	{
		perror("open LCD  Err");
		return -1;
	}
	printf("Picture Loading......\n");
	
}
 
int uninit_lcd()
{
	close(lcd);		//关闭LCD
	/*取消映射*/
	/* munmap(fb_mem,      //映射后的操作地址
        480*272*2);  //映射的大小 */
}
 
int show_bmp(char *file)  //显示一张图片 思路
{
	int ret,x,y,i;
	short lcdbuf[480*272*2]; //注意：因为此LCD是一个像素包含两个字节，所以要用short，用int类型则会出错
	char temp[3] = {0};
	char bmpbuf[480*272*3];
	
	int bmp = open(file,O_RDONLY);	//2.打开bmp图片
	if(bmp == -1)
	{
		perror("open BMP  Err");
		return -1;
	}	
	
	lseek(bmp, 54, SEEK_SET);  //偏移54个字节
	
	ret = read(bmp,bmpbuf,480*272*3); //3.读取bmp信息
	if(ret ==-1)
	{
		perror("read BMP  Err");
		return -1;
	}
 
	//4.将window下24位的bmp图像转化为ARM S3C2440下的16位的图片(存储RGB格式有变化)
	//change  24bit RGB  to  16bit 思路：r:g:b = 5:6:5 取r的高五位，g的高6位，b的高5位，或运算，重新组成一个新的16位  
    for(y=0,i=0;y<272;y++)
	{
		for(x=0;x<480;x++,i+=3)
        {	
                            //        B              G              R
			lcdbuf[(271-y)*480+x] = (bmpbuf[i])>>3 | (bmpbuf[i+1]>>2)<<5 | (bmpbuf[i+2]>>3) <<11;//将24位转化为16位算法
	    }
	}
	
	//5.往LCD写数据
	ret = write(lcd,lcdbuf,480*272*2);
	if(ret ==-1)
	{
		perror("write LCD  Err");
		return -1;
	}	
 
	//close(lcd);     //6.关闭lcd,有专门的uninit_lcd函数关闭lcd 
	close(bmp);		//7.关闭图片
	
	return 0;
}
 
int main(int argc, char* argv[])
{
	init_lcd(); //LCD 初始化
	show_bmp(argv[1]);
	sleep(1);
	uninit_lcd();//关闭LCD
	return 0;
}
 
 
 
 
 
 
 
