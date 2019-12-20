/***********************************************************************************
*              Copyright 2007 - 2010, Megaeyes. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: vs_sd.c
* Description: SD������ز���:��ȡ��������ʣ��������SD��״̬������ȫ�ֱ���g_config.para_store�е��������и�ֵ
*
* History:
* Version   Date              Author  WXW      DefectNum    Description
* 1.1       2008-01-12   q60002125     NULL            Create this file.
***********************************************************************************/



#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include <stdio.h>
#include <semaphore.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>                  
#include <sys/vfs.h>        /* for statfs() */
#include <sys/stat.h>        /* for stat() */
#include <unistd.h>
#include <fcntl.h>

#include "vs_def_videoProtocol_3.h"
#include "vs_config.h"
#include "vs_sd.h"
#include "vs_save.h"

#include <asm/io.h>
#include <linux/ioctl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/mman.h>

pthread_mutex_t sd_format_lock;

unsigned int*  gpio_map=0;
unsigned int gpio_mapaddr;
int fd;

//#define GPIO_1_BASE     0x101E5000
//#define GPIO_1_DIR (gpio_mapaddr + 0x400)
//#define GPIO_1_2_DATA (gpio_mapaddr + 0x010) /* GPIO1_2 is sd card insert detect */
//#define GPIO_1_3_DATA (gpio_mapaddr + 0x020) /* GPIO1_3 is reset usb to sd chip 817 */

#define GPIO_3_BASE     0x101E7000
#define GPIO_3_DIR (gpio_mapaddr + 0x400)
#define GPIO_3_3_DATA (gpio_mapaddr + 0x020)

#define MMAP_LENGTH        0x1000

#define HW_REG(reg) *((volatile unsigned int *)(reg))


static int  sd_detect_init(void)
{
    fd=open("/dev/mem",O_RDWR|O_SYNC,00777);
    
    if(fd<0)
    {
        return -1;
    }
    gpio_map = (unsigned int *) mmap( NULL,MMAP_LENGTH,PROT_READ|PROT_WRITE,MAP_SHARED,fd,GPIO_3_BASE);

    if(gpio_map==0)
    {
        return -1;
    }
    gpio_mapaddr = (unsigned int)gpio_map;
    
    return 0;
}


static int sd_detect_exit(void)
{
    printf("sd_detect_exit\n");
    if(-1== munmap( gpio_map, MMAP_LENGTH))
    {
        printf("mumap failed");
        return -1;
    }
    if(-1==close( fd ))
    {
        printf("close the file operator failed");
        return -1;
    }
    return 0;
}

static int sd_detect_operate(void)
{
    unsigned char regvalue;
    
    regvalue = HW_REG(GPIO_3_DIR);
    regvalue &= 0xf7;                        // ����GPIO3�ļĴ���GPIO_DIR����ʾGPIO3[3]Ϊ����                                    
    HW_REG(GPIO_3_DIR) = regvalue;
    regvalue = HW_REG(GPIO_3_3_DATA);
    if((regvalue &0x08) == 0x08)
        return 1;
    else 
        return 0;   
}

/*������:sd_detect_wait
  ����:��� SD����״̬����ȫ�ֱ�����ֵ�����ж��Ƿ��в����γ�SD���Ķ���������о�����Ӧ�Ĵ���
*/
void sd_detect_wait(void)
{
    unsigned char tempvalue;
    int m;
    m=g_config.para_store.SD_status.var;
    while(1)
    {
        /* when gpio_3_3 is low, sd card is insert. */
        if(sd_detect_operate() == 0)                    //SD������
        {
            usleep(100);
             g_config.para_store.SD_status.var=1;
            if(m!=g_config.para_store.SD_status.var)  //������������SD�����ڻ���������й�������SD������
            {  
               m=g_config.para_store.SD_status.var;
               system("insmod /dvs/extdrv/sd.ko");
               sleep(3);
               system("mount -t vfat /dev/sd/0/part1 ./SD");
               vs_sd_init();
               vs_set_save_base_folder();
               printf("detect sd card insert!\n");
            }
            
            #ifdef debug_sd_detect_print 
            printf("sd card is power on.\n");
            #endif
        }
        else                                          //SD��������
        {
             g_config.para_store.SD_status.var=0;
            if(m!=g_config.para_store.SD_status.var)  //���������й�����SD�����γ�
            { 
                m=g_config.para_store.SD_status.var;
                vs_sd_init();
                vs_set_save_base_folder();
                printf("SD�����γ�\n");
            }
            #ifdef debug_sd_detect_print 
            printf("sd card is power off.\n");
            #endif
        }
        sleep(1);
            
    }    
}


/*������:get_sd_capacity
  ����:��ȡSD����������С��ʣ��ռ��С������ȫ�ֱ���g_config.para_store�е�����ֵ
 */
void get_sd_capacity()
{

    long size ;
    long allsize ;
    int percent;
    
    struct statfs stat;
    statfs("/remote/vs/SD",&stat);
    size = stat.f_bsize * stat.f_bavail;
    allsize = stat.f_bsize * stat.f_blocks;
    if(g_config.para_store.SD_status.var==0)
    { 
      g_config.para_store.SD_size_amount.var = 0;
      g_config.para_store.SD_size_useable.var =0;
      g_config.para_store.SD_alarm.var=0; 
    }
    else
    {
       if ( stat.f_blocks > 0 )
       {
           percent = stat.f_bavail * 100 / stat.f_blocks;
       }
    
       g_config.para_store.SD_size_amount.var = allsize/(1024*1024);  //SD��������
       g_config.para_store.SD_size_useable.var = size/(1024*1024);    //SD��ʣ������

       if (percent < 10)
       {
           g_config.para_store.SD_alarm.var=0;     //�ռ䲻��
       }
       else
       {
          g_config.para_store.SD_alarm.var=1;     //�ռ乻��
       }
    }
    
//#ifdef debug_sd_state_print 
      printf("SD�������� = %d M\n", g_config.para_store.SD_size_amount.var);
      printf("SD��ʣ������ = %d M\n", g_config.para_store.SD_size_useable.var);
//#endif

    return;
}


int sd_format()
{
    pthread_mutex_lock (&sd_format_lock);
    system("mkfs.vfat /dev/sd/0/part1");
    pthread_mutex_unlock (&sd_format_lock);

    sleep(10);                  // pgh ����ʹ��sleep���ܻ������⣬�Ժ�Ľ�
    return 0;

}

int sd_umount()
{
    pthread_mutex_lock (&sd_format_lock);
    system("umount /remote/vs/SD");
    sleep(5);
    system("rmmod sd.ko");
    pthread_mutex_unlock (&sd_format_lock);

    sleep(1);                   // pgh ����ʹ��sleep���ܻ������⣬�Ժ�Ľ�
    return 0;
}




/*  ������:vs_sd_init
    ����:��ʼ��ʱ���ô˺�������SD�������ȫ�ֱ���(g_config.para_store)��ֵ
*/
void vs_sd_init()
{
    get_sd_capacity();  

}


/*  ���߳����������SD����״̬*/

void thread_detectsd_main()
{
   if(sd_detect_init() != 0)
        printf("sd card detect init error!\n");
    sd_detect_wait();
    sd_detect_exit();
}


/*������:sd_test
  ����:����SD���Ķ�д
*/
 
int sd_test()
{
    char buf1[100],buf2[100];
    FILE * fp;
    //int bytesread,byteswrite;
    memset(buf1,'a',sizeof(buf1));
    memset(buf2,'b',sizeof(buf2));
    fp=fopen("/mnt/1.txt","r+");
    fwrite(buf1,sizeof(buf1),1,fp);
    fclose(fp);
return 0;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


