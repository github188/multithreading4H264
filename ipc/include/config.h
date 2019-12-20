#ifndef __VS_CONFIG_H_
#define __VS_CONFIG_H_


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define _LEN_NORMAL 48
#define _LEN_EXTEND 64
#define _LEN_WEEK 172               // ����ʱ��ר��
#define _LEN_DETECK_AREAS 255       // �ƶ��������
#define _TICKET_LENGHT 6
#define _MAX_OSD_TIME_LENGTH_ 28
#define DETECT_USLEEP_MS 1000
#define _MAX_LEN_SESSION_ID                     64                  // ��ҳsession_id����
#define _MAX_LEN_LOGIN_USERNAME                 32                  // ��ҳ��¼�û�������
#define _MAX_LEN_LOGIN_PASSWORD                 32                  // ��ҳ��¼���볤��
#define _MAX_NUM_SESSION_ID                 8                       // ����session_id��������ͬʱ��������web�����û���


// ֱ����Ƶ���� SessionID = "DIRECT", ������
#define SESSION_ID_DIRECT                 "DIRECT"

// CIF ��QCIF ��2CIF��4CIF/D1���ֱ���0,1,2,3����
#define RESOLUTION_D1                3
#define RESOLUTION_HD1               2
#define RESOLUTION_QCIF              1
#define RESOLUTION_CIF               0

// ��������
typedef struct tagD_string
{
    char name[_LEN_NORMAL];
    char var[_LEN_NORMAL];                                          // pgh ����ط�������ƶ���⣬��Ҫ����Ϊ_LEN_DETECK_AREAS
    char note[_LEN_EXTEND];
}D_string;

typedef struct tagD_string_detect
{
    char name[_LEN_NORMAL];
    char var[_LEN_DETECK_AREAS];                                          // pgh ����ط�������ƶ���⣬��Ҫ����Ϊ_LEN_DETECK_AREAS
    char note[_LEN_EXTEND];
}D_string_detect;

typedef struct tagD_week_string
{
    char name[_LEN_NORMAL];
    char var[_LEN_WEEK];        // ר���ڲ���ʱ��Ŀռ�. ÿ��168��Сʱ
    char note[_LEN_EXTEND];
}D_week_string;

typedef struct tagD_int
{
    char name[_LEN_NORMAL];
    int var;
    char note[_LEN_EXTEND];
}D_int;

typedef struct tagD_long
{
    char name[_LEN_NORMAL];
    long var;
    char note[_LEN_EXTEND];
}D_long;
// .... ����

// sx added auto
#include "vs_config_auto.h"
// sx added auto end

// �����ú궨��
//#define DEBUG_SEND_MESSAGE                                    //  tian 2080122 add for debug the reveived and sending message package
//#define debug_printf_buf                                      // buf����ģ���ӡ���
//#define debug_printf_socket                                   // socketģ���ӡ���
//#define   debug_cap_print                                     //ͼƬץ��ģ����Դ�ӡ
//#define   DEBUG_VIDEO_PRINT					 //video print for saving 
//#define  debug_printf_signal                                    // ��ӡ�ź�ʱ��
//#define debug_printf_msg_rev                                    // ��ӡ���յ���Ϣ
//#define debug_printf_mainrountine                               // mainrountineģ���ӡ���
#define debug_printf_mainrountine_1                             // mainrountineģ���ӡ���
//#define debug_printf_mainrountine_2                             // mainrountineģ���ӡ���
//#define debug_printf_mainrountine_RTV                           // mainrountineģ���ӡ���
//#define debug_printf_timer                                    // timerģ���ӡ���
//#define debug_printf_timer_queue                              // timerģ���ӡtimer����
#define debug_printf_parser                                   // parserģ���ӡ���
//#define debug_printf_detect                                   // detectģ���ӡ���
//#define debug_printf_main                                     // mainģ���ӡ���
//#define debug_printf_save                                     // saveģ���ӡ���
//#define debug_printf_venc                                     // ר�õ���Ƶ����ȴ�ӡ���
//#define debug_printf_crypt                                    // ����ģ���ӡ���
//#define debug_printf_crypt_package                            // ����ģ�飬zdq�޸������������
//#define debug_printf_crypt_MD5                                // MD5ģ���ӡ���
//#define debug_printf_mainrountine_register_res                // ע�ᷴ����ӡ���
//#define debug_printf_mainrountine_register_req
//#define debug_printf_mainrountine_store_register_res
//#define debug_printf_mainrountine_keepAlive_req               //������
//#define debug_printf_thread_create                            // �����̴߳�ӡ���
//#define debug_osd_print                                       //osd�������
//#define debug_printf_http_context                             // hc->http_context�ļ����ݽ���
//#define debug_printf_restart                                  // ֱ����ʽ����
//#define detect_cap
////#define debug_printf_ftp_struct2string                        // ftp��ӡ
//#define debug_photo_searh_print                               //��Ƭ������Դ�ӡ
 // #define debug_sd_state_print                                //SD��״̬��Ϣ
 // #define debug_sd_detect_print                               //SD��״̬���
// #define   debug_ftp_print
//#define debug_handler_cgi_detect_action                      
// �����ú궨�� end 


#define RTV_AV_FLAG             0                   // ʵʱ����Ƶ��ǣ�0:��������Ҫ00dc(4�ֽ�) + ��Ƶ������(4�ֽ�) + ʱ���(8�ֽ�)
                                                    // 1: �����в���Ҫ00dc(4�ֽ�) + ��Ƶ������(4�ֽ�) + ʱ���(8�ֽ�)

#define SEND_AV_SLICE           1              // 1:��Ƶ����Ƭ����  0:��Ƶ����֡����

#define   FAILTIME_DIRECT         10                        // ֱ��ģʽʵʱ��ƵʧЧ����
#define   FAILTIME_ACCESS         10                        // ����ģʽʵʱ��ƵʧЧ����

int vs_config_put_var(char *name, char *var, char *note);

int vs_config_weekplan2schedule();
int vs_config_schedule2weekplan();

// ��ȡ�����ļ�
int config_read();

// ��д�����ļ�
int config_write();

// ����ȫ�ֲ���������һ���Ķ�ʱ������
int vs_config_init_timer();
AllConfig g_config;         // ȫ�ֱ���,���õ����в���

    
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __VS_CONFIG_H_ */





