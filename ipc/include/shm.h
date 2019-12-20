/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_thread.h
  Version       : Initial Draft
  Author        : 
  Created       : 
  Last Modified :
  Description   : hi_thread.c header file
  Function List :
  History       :
  1.Date        : 
    Author      : Monster
    Modification: Created file

******************************************************************************/


#ifndef __VS_PROCESS_COM_H__
#define __VS_PROCESS_COM_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <sys/sem.h> 
#include <sys/ipc.h> 

#include "vs_config.h"


#define ID_CGI_LOGIN_ACTION                 0
#define ID_CGI_ADVC                         1
#define ID_CGI_DETECT_ACTION  	            2
#define ID_CGI_ADD_AREA    			        3

#define ID_CGI_SD_CARD  	                4
#define ID_CGI_PARA_STORECONF               5
#define ID_CGI_PARA_SDCRTL  	            6
#define ID_CGI_PARA_SDCONF  	            7

#define ID_CGI_MISC  	                    8
#define ID_CGI_PARA_NETWORK  	            9             
#define ID_CGI_PARA_CONNECT                 10  
#define ID_CGI_PARA_OSD                     11                 
#define ID_CGI_PARA_VENC                    12                 
#define ID_CGI_PARA_RESTORE                 13                  
#define ID_CGI_PARA_REROOT                  14

#define ID_CGI_PHOTO_SEARCH                 15                  
#define ID_CGI_PHOTO_DELETE                 16  
#define ID_CGI_PHOTO                        17
#define ID_CGI_PARA_UPDATE                  18

#define SEGSIZE 1024        // ע�⣬��ֵҪ��������ҳ���
#define READTIME 1 

/*******************************����CGI�����ﶨ����Ҫ��web server��õ����ݵ����ݽṹ **********************************/
typedef struct tag_cgi_login_action   // ��¼ҳ�����ݽṹ
{
    char username[32];
    char password[32];
} s_cgi_login_action;

typedef struct tag_cgi_advc   // ��vs���advcҳ����Ҫ����: ����ͷ������IP, port
{
    int num_camera;
    int video_port;
    char device_IP[_LEN_NORMAL];
}s_cgi_advc;

typedef struct tag_cgi_detect_action_step1   // ��һ��: ��������������
{
    int alarminA;                                      // �豸 A
    int alarminB;                                      // �豸 B
    int movedetect;                                    // �ƶ����
} s_cgi_detect_action_step1;


typedef struct tag_cgi_detect_action   // �ƶ����
{
    s_cgi_detect_action_step1 cgi_detect_action_step1;  // ��һ��: �������������� 
    
    int Capture_enable;                                 // ץ����Ƭʹ��
    int recording;                                      // ¼��ʹ��
    int alarmbell;                                      // �쾯��ʹ��

    int Capture_style;                                  // 1. ץ�ĵ�����  2. ץ�ĵ�ftp server
    int recordingtime_r;                                // ¼������
    int alarmbelltime_a;                                // �쾯���� ��ʱ
    int Cap_count;                                      // ����ʱ����ץ������
    int Cap_Time;                                       // ���ץ������

    char Detect_Ftp[_LEN_NORMAL];                                // ftp server IP
    char Detect_Ftp_user[_LEN_NORMAL];                           // ftp username
    char Detect_Ftp_pwd[_LEN_NORMAL];                            // ftp password
    char Detect_Ftp_Dir[_LEN_NORMAL];                            // ftp �洢Ŀ¼·��

    char week_plan[_LEN_WEEK];                               // �ƻ������
} s_cgi_detect_action;


typedef struct tag_cgi_add_area                         // �ƶ������������
{
    char dete_areas[_LEN_DETECK_AREAS];                 // �ƶ��������           
    int  difference;                                    // �ƶ����������
} s_cgi_add_area;


/***********************************  sd�� ***********************************/
typedef struct tag_cgi_para_storeconf  // SD���洢����  
{
    int sdStore_w;                                             // SD���洢���� 0,���洢 1,�洢
} s_cgi_para_storeconf;


typedef struct tag_cgi_para_sdinfo  // SD���洢��Ϣ
{
    int sd_status;                                                  // SD��״̬
    int sd_capb;                                                    // SD��������
    int sd_apb;                                                     // SD��ʣ������
    int alarm_mes;                                                  // SD������Ϣ
} s_cgi_para_sdinfo;

typedef struct tag_cgi_para_sdconf  // SD������
{
    int sdOverlay_w;                                                // �Ƿ񸲸� 0,�� 1. Ҫ
    int sdBackup_w;                                                 // �Ƿ񱸷� 0,�� 1. Ҫ;                                               
    int sdBackupclear_w;                                            // ���ݺ��Ƿ���� 0,�� 1. Ҫ
} s_cgi_para_sdconf;

typedef struct tag_cgi_para_sdcrtl  // SD����Ϣ��ѯ����ʽ��
{  
    int sdhandle_w;                                                 // 1: ˢ�� 2:�����Ƭ 3. SD����ʽ��
} s_cgi_para_sdcrtl;

typedef struct tag_cgi_sd_card      // SD��ҳ��
{
    s_cgi_para_storeconf    cgi_para_storeconf;                     // SD���洢����,�Ƿ�SD���洢  0: ��ѡ��SD���洢  1: ѡ��SD���洢
    s_cgi_para_sdinfo       cgi_para_sdinfo;                        // SD���洢��Ϣ��ѯ
    s_cgi_para_sdconf       cgi_para_sdconf;                        // SD������                                          
} s_cgi_sd_card;


/***********************************  �������� ***********************************/
typedef struct tag_cgi_para_network   // �������
{
    char ip_local[_LEN_NORMAL];                         // ����IP��ַ
    char ip_mask[_LEN_NORMAL];                          // ��������
    char ip_gateway[_LEN_NORMAL];                       // ����
    char ip_DNS[_LEN_NORMAL];                           // DNS
    char ip_server_access[_LEN_NORMAL];                 // ���������IP��ַ
    int port_server_access;                             // ����������˿�
} s_cgi_para_network;


typedef struct tag_cgi_para_connect   // ������Ϣ
{
    char ftp_username[_LEN_NORMAL];                     // FTP�û���
    char ftp_password[_LEN_NORMAL];                     // FTP����
    char dvs_id[_LEN_NORMAL];                           // �豸id,���ڽ���
    int  dev_max_con;                                   // �豸�����������������������ͬʱ��ȡ��Ƶ���û���������ֱ��ģʽ������ģʽ
} s_cgi_para_connect;

typedef struct tag_cgi_para_osd   // ��ʾ���Ƽ�ͨ��Э��--�ַ����� 
{
    int osd_enable;                                     // �����Ļ
    char text[_LEN_NORMAL];                                           // ��������
    int tpox;
    int tpoy;
     
    int osd_time_enable;                                // ��ʾʱ��
    char sTfmt[_LEN_NORMAL];                                         // ʱ���ʽ
    int cpox;                                       
    int cpoy;
} s_cgi_para_osd;


typedef struct tag_cgi_para_venc   // ͼ��Ʒ��--������� 
{
    int videoPara;                                      // ������
    
    int birate;                                         // ����
    int maxquant;                                       // �������
    int quant1;                                         // �������
    int fps;                                            // ֡�ʿ���
    int videoformat;                                    // ͼ��ߴ�
    int bitype;                                         // ��������, 1:��Ƶ�� 2:������
    int gjfra;                                          // �ؼ�֡���
} s_cgi_para_venc;

/****************************************************************************************
�����ļ�http�ϴ�������������:
1. ���˾��ã��ļ�ͨ��http�ϴ�������CGIֱ�Ӵ�ŵ�ָ����λ�þ����ˣ�����Ҫcgi���������ٷ��͸�vs������
2. �����Ҫ���͸�vs�����򣬻�����������:
   (1) filesize��С��2M���ң���ô�����ڴ�ͻ��˷Ѻܶ�
   (2) Ϊ�˽�Լ�����ڴ濪����ֻ�ж�����ļ��ϴ����е�������
   
typedef struct tag_cgi_para_update   // �汾����  pgh �����ݽṹ��Ա��Ҫ��һ����֤
{
    char filename[_LEN_NORMAL];                         
    char filepath[_LEN_NORMAL];
    char fileUID[_LEN_NORMAL];                          // �ļ�Ȩ��
    char file_other_info[_LEN_NORMAL];                  // �ļ�������Ϣ
    char file_content[???]
    int filesize;
} s_cgi_para_update;
***************************************************************************************/

typedef struct tag_cgi_para_restore   // �ָ��������ã�ֱ�Ӳ�������������
{  
} s_cgi_para_restore;

typedef struct tag_cgi_para_reboot  // ϵͳ������ֱ�Ӳ�������������
{  
} s_cgi_para_reboot;


typedef struct tag_cgi_misc      // ��������ҳ��
{
    s_cgi_para_network       cgi_para_network;                     // �������
    s_cgi_para_connect       cgi_para_connect;                     // ������Ϣ
    s_cgi_para_osd           cgi_para_osd;                         // ��ʾ���Ƽ�ͨ��Э��--�ַ�����
    s_cgi_para_venc          cgi_para_venc;                        // ͼ��Ʒ��--������� 
} s_cgi_misc;




/************************************* ��Ƭ���&���� **************************************************/

typedef struct tag_cgi_photo_search   // ��Ƭ���&����
{
    char begindate_w[_LEN_NORMAL];                                       // ��ʼ����
    int beginhour_w;                                            // ��ʼСʱ
    int beginminute_w;                                          // ��ʼ����
    
    char enddate_w[_LEN_NORMAL];                                         // ��������
    int endhour_w;                                              // ����Сʱ
    int endminute_w;                                            // ��������
    
    int pageno_w;                                               // ҳ��
    int searchtype_w;                                           // 1.����������Ƭ 0. ����ȫ����Ƭ

    int bphoto_search;                                          //�Ƿ���������ť(�������ַ�ҳ��������)
    int photoSize_r;                                            //��Ƭ����
    int pageSize_r;                                             //��ҳ��
    int pageNum_r;                                              //��ǰҳ��
    
    char photoUrl1_r[_LEN_NORMAL];                                    //��1����Ƭ�ľ���url
    char photoUrl2_r[_LEN_NORMAL];                                     //��2����Ƭ�ľ���url 
    char photoUrl3_r[_LEN_NORMAL];                                     //��3����Ƭ�ľ���url
    char photoUrl4_r[_LEN_NORMAL];                                     //��4����Ƭ�ľ���url
    char photoUrl5_r[_LEN_NORMAL];                                     //��5����Ƭ�ľ���url
    char photoUrl6_r[_LEN_NORMAL];                                     //��6����Ƭ�ľ���url
    char photoUrl7_r[_LEN_NORMAL];                                     //��7����Ƭ�ľ���url
    char photoUrl8_r[_LEN_NORMAL];                                     //��8����Ƭ�ľ���url
    char photoUrl9_r[_LEN_NORMAL];                                     //��9����Ƭ�ľ���url
} s_cgi_photo_search;

typedef struct tag_cgi_photo_delete   // ��Ƭɾ��
{
    char del_img1[_LEN_NORMAL];                                               // ɾ����Ƭ1
    char del_img2[_LEN_NORMAL];                                               // ɾ����Ƭ2
    char del_img3[_LEN_NORMAL];                                               // ɾ����Ƭ3
    char del_img4[_LEN_NORMAL];                                               // ɾ����Ƭ4
    char del_img5[_LEN_NORMAL];                                               // ɾ����Ƭ5
    char del_img6[_LEN_NORMAL];                                               // ɾ����Ƭ6
    char del_img7[_LEN_NORMAL];                                               // ɾ����Ƭ7
    char del_img8[_LEN_NORMAL];                                               // ɾ����Ƭ8
    char del_img9[_LEN_NORMAL];                                               // ɾ����Ƭ9
    int del_img_all;                                                        // ɾ��������Ƭ   
} s_cgi_photo_delete;


typedef union tag_cgi_data_DS   
{
    s_cgi_login_action                      cgi_login_action;                // ���: 0 , ��¼ҳ��(username, password)���ݽṹ
    s_cgi_advc                              cgi_advc;                        // ���: 1 , advcҳ�����: ����ͷ������IP, port
    s_cgi_detect_action                     cgi_detect_action;               // ���: 2 , alertҳ�����
    s_cgi_add_area                          cgi_add_area;                    // ���: 3 , alert_moveҳ�����

    s_cgi_sd_card                           cgi_sd_card;                     // ���: 4 , sd��ҳ��
    s_cgi_para_storeconf                    cgi_para_storeconf;              // ���: 5 , alert_moveҳ�����
    s_cgi_para_sdcrtl                       cgi_para_sdcrtl;                 // ���: 6 , alert_moveҳ�����
    s_cgi_para_sdconf                       cgi_para_sdconf;                 // ���: 7 , alert_moveҳ�����

    s_cgi_para_network                      cgi_para_network;                // ���: 8  , �������
    s_cgi_para_connect                      cgi_para_connect;                // ���: 9  , ������Ϣ
    s_cgi_para_osd                          cgi_para_osd;                    // ���: 10 , ��ʾ���Ƽ�ͨ��Э��--�ַ�����
    s_cgi_para_venc                         cgi_para_venc;                   // ���: 11 , ͼ��Ʒ��--������� 
    s_cgi_para_restore                      cgi_para_restore;                // ���: 12 , �ָ���������
    s_cgi_para_reboot                       cgi_para_reboot;                 // ���: 13 , ϵͳ����

    s_cgi_photo_search                      cgi_photo_search;                // ���: 14 , ��Ƭ���&����
    s_cgi_photo_delete                      cgi_photo_delete;                // ���: 15 , ��Ƭɾ��                   
} s_cgi_data_DS;

typedef struct tag_cgi_data
{
    int result;					                // �ɹ���־0, �ɹ� >0 ʧ�ܡ������ڴ�ĶԷ�������Ϻ���д��ֵ
    int id_cgi_data_DS;                         // ������Ӧ��������ݽṹ
    int send_rev;					            // ��ʾ�����ڴ�ĶԷ� ���ͻ��ǽ�������0.���� 1. ����  2.����&����
    s_cgi_data_DS cgi_data_DS;                  // ҳ�����ݽṹ    
} s_cgi_data;

// ��ҳ��ÿһ���ύ����Ӧһ��CGI,��Ӧһ��vs��CGI����������ǵ����ӹ�ϵ:
// cgi_data_DS -- id_cgi_data_DS    id_cgi_handler -- cgi_handler
// ֻҪ�����ڴ��˫����id_cgi_data_DS/id_cgi_handlerʹ����ͬ�ı�ţ�����ʵ���ύ��vs�������Ķ�Ӧ
typedef int(*cgi_CallBack_operation)(char *p_shm);
typedef struct tag_map_CGI_flag2Handler
{

	int id_cgi_handler;
	cgi_CallBack_operation cgi_handler;               // �����ӦCGI��handler
} s_map_CGI_flag2Handler;

#define ID_CGI_LOGIN_ACTION                 0
#define ID_CGI_ADVC                         1
#define ID_CGI_DETECT_ACTION  	            2
#define ID_CGI_ADD_AREA    			        3

#define ID_CGI_SD_CARD  	                4
#define ID_CGI_PARA_STORECONF               5
#define ID_CGI_PARA_SDCRTL  	            6
#define ID_CGI_PARA_SDCONF  	            7

#define ID_CGI_MISC  	                    8
#define ID_CGI_PARA_NETWORK  	            9             
#define ID_CGI_PARA_CONNECT                 10  
#define ID_CGI_PARA_OSD                     11                 
#define ID_CGI_PARA_VENC                    12                 
#define ID_CGI_PARA_RESTORE                 13                  
#define ID_CGI_PARA_REROOT                  14

#define ID_CGI_PHOTO_SEARCH                 15                  
#define ID_CGI_PHOTO_DELETE                 16  
#define ID_CGI_PHOTO                        17
#define ID_CGI_PARA_UPDATE                  18



#define CGI_REQ_SUC                     0               // cgi��vs������ɹ�
#define CGI_REQ_ERR                     1               // cgi��vs������ʧ��



int sem_creat(key_t key) ;//�����ź��� 
void del_sem(int semid) ;//ɾ���ź��� 
int p(int semid) ;//p ����
int v(int semid) ;//v ����
int handler_cgi_login_action(char *p_shm);
int handler_cgi_advc(char *p_shm);// ����CGI�ĵ�¼�ɹ���advcҳ������
int handler_cgi_detect_action_r(s_cgi_detect_action *p_cgi_detect_action);// �������������ݣ�ר����handler_cgi_detect_action()����
int handler_cgi_detect_action_w(s_cgi_detect_action *p_cgi_detect_action);// д�����������ݣ�ר����handler_cgi_detect_action()����
int handler_cgi_detect_action(char *p_shm);// ����CGI��alertҳ������
int handler_cgi_add_area_r(s_cgi_add_area *p_cgi_add_area);// ����alert_moveҳ������--������������
int handler_cgi_add_area_w(s_cgi_add_area *p_cgi_add_area);// ����alert_moveҳ������--д����������
int handler_cgi_add_area(char *p_shm);// ����alert_moveҳ������
int handler_cgi_sd_card_w(s_cgi_sd_card *p_cgi_sd_card);// ����sd_cardҳ������--д����������
int handler_cgi_sd_card(char *p_shm);// ����sd_cardҳ������
int handler_cgi_para_storeconf_r(s_cgi_para_storeconf *p_cgi_para_storeconf);// ����SD�洢��������--������������
int handler_cgi_para_storeconf(char *p_shm);// ����SD�洢��������
int handler_cgi_para_sdcrtl_refresh(s_cgi_para_sdcrtl *p_cgi_para_sdcrtl);
int handler_cgi_para_sdcrtl_clear(s_cgi_para_sdcrtl *p_cgi_para_sdcrtl);
int handler_cgi_para_sdcrtl_format();
int handler_cgi_para_sdcrtl_umount();
int handler_cgi_para_sdcrtl(char *p_shm);// ����SD������: 1: ˢ�� 2:�����Ƭ 3. SD����ʽ�� 4.ж��SD��
int handler_cgi_para_sdconf_r(s_cgi_para_sdconf *p_cgi_para_sdconf);// ����SD���������ã��� ��
int handler_cgi_para_sdconf(char *p_shm);// ����SD����������
int handler_cgi_misc_w(s_cgi_misc *p_cgi_misc);
int handler_cgi_para_update(char *p_shm);               // ������������ҳ�� -- misc -- �汾����
int handler_cgi_misc(char *p_shm);// ������������ҳ�� -- misc
int handler_cgi_para_network_w(s_cgi_para_network *p_cgi_para_network);
int handler_cgi_para_network(char *p_shm);// ������������ҳ�� -- misc -- �������
int handler_cgi_para_connect_w(s_cgi_para_connect *p_cgi_para_connect);
int handler_cgi_para_connect(char *p_shm);// ������������ҳ�� -- misc -- ������Ϣ
int handler_cgi_para_osd_w(s_cgi_para_osd *p_cgi_para_osd);
int handler_cgi_para_osd(char *p_shm);// ������������ҳ�� -- misc -- ��ʾ���Ƽ�ͨ��Э��--�ַ�����
int handler_cgi_para_venc_w(s_cgi_para_venc *p_cgi_para_venc);
int handler_cgi_para_venc(char *p_shm);// ������������ҳ�� -- misc -- ͼ��Ʒ��--������� 
int handler_cgi_para_restore(char *p_shm);// ������������ҳ�� -- misc -- �ָ���������
int handler_cgi_para_reboot(char *p_shm);// ������������ҳ�� -- misc -- ��������
int handler_cgi_photo_search_w(s_cgi_photo_search *p_cgi_photo_search);
int handler_cgi_photo_search(char *p_shm);// ������Ƭ���&����
int handler_cgi_photo_delete_w(s_cgi_photo_delete *p_cgi_photo_delete);
int handler_cgi_photo_delete(char *p_shm);// ������Ƭɾ��
int cgi_getHandlerMatch(int id_cgi_handler); /* ��handler��Ӧ������ƥ�����鵥Ԫ��  */
int thread_process_com_main() ;                 // ���̼�ͨ����Ϣ�����̣߳�Ŀǰ���ù����ڴ淽ʽ



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __VS_SOCKET_H__ */
