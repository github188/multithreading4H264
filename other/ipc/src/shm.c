/***********************************************************************************
*              Copyright 2007 - 2010, Megaeyes. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: vs_process_com.c
* Description:  .
*
* History:
* Version   Date              Author        DefectNum    Description

***********************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "vs_process_com.h"
#include "vs_main_rountin.h"
#include "vs_sd.h"
#include "vs_factory_set.h"
#include "vs_photo.h"
//added by ww
#include "vs_video_manage.h"

union semun { 
    int val; 
    struct semid_ds *buf; 
    unsigned short *array; 
} arg; 


s_map_CGI_flag2Handler  map_CGI_flag2Handler[]=
{
		{	ID_CGI_LOGIN_ACTION      , 			handler_cgi_login_action	    },
		{	ID_CGI_ADVC              , 			handler_cgi_advc	            },
		{	ID_CGI_DETECT_ACTION     , 			handler_cgi_detect_action	    },
		{	ID_CGI_ADD_AREA          , 			handler_cgi_add_area	        },

		{	ID_CGI_SD_CARD           , 			handler_cgi_sd_card	            },
		{	ID_CGI_PARA_STORECONF    , 			handler_cgi_para_storeconf	    },
		{	ID_CGI_PARA_SDCRTL       , 			handler_cgi_para_sdcrtl	        },
		{	ID_CGI_PARA_SDCONF       , 			handler_cgi_para_sdconf	        },
 
        {	ID_CGI_MISC             , 			handler_cgi_misc	            },
		{	ID_CGI_PARA_NETWORK     , 			handler_cgi_para_network	    },
		{	ID_CGI_PARA_CONNECT     , 		    handler_cgi_para_connect	    },
		{	ID_CGI_PARA_OSD         , 			handler_cgi_para_osd	        },
		{	ID_CGI_PARA_VENC        , 			handler_cgi_para_venc	        },
		{	ID_CGI_PARA_RESTORE     , 			handler_cgi_para_restore	    },
		{	ID_CGI_PARA_REROOT      , 	        handler_cgi_para_reboot	        },

		{	ID_CGI_PHOTO_SEARCH     , 			handler_cgi_photo_search	    },
		{	ID_CGI_PHOTO_DELETE     , 	        handler_cgi_photo_delete	    },
		{	ID_CGI_PARA_UPDATE      , 	        handler_cgi_para_update	        }
		
};


//�����ź��� 
int sem_creat(key_t key) 
{ 
    union semun sem; 
    int semid; 
    sem.val = 0; 
    semid = semget(key,1,IPC_CREAT |0777); 

    if (-1 == semid){ 
        printf("create semaphore error\n"); 
        return -1; 
    } 
    semctl(semid,0,SETVAL,sem); 
    return semid; 
} 

//ɾ���ź��� 
void del_sem(int semid) 
{ 
    union semun sem; 
    sem.val = 0; 
    semctl(semid,0,IPC_RMID,sem); 
} 

//p ����
int p(int semid) 
{ 
    struct sembuf sops={0, -1,0}; 
    return (semop(semid,&sops,1)); 
} 

//v ����
int v(int semid) 
{ 
    struct sembuf sops={0,1,0}; 
    return (semop(semid,&sops,1)); 
} 


/********************************** ���ﴦ�����CGI******************************/

/* ֱ��ģʽ��¼:
    1. ����û���������

    ���������Ѿ�ɾ��
    2. ����ticket,���������ҳ���л�ø�ticket,���ڽ�����еĽ���(��ҳ����Ϳؼ���Ƶ)�У����ݸ�ticket������֤��
      ticket��ʧЧʱ��Ϊ1800�룬ÿ��������Ľ��������ø�ʧЧʱ��
    3. ����advc.asp.htmҳ��
*/
int handler_cgi_login_action(char *p_shm)
{
    int ret;
    char name[_MAX_LEN_LOGIN_USERNAME];
    char pwd[_MAX_LEN_LOGIN_PASSWORD];
    s_cgi_login_action *cgi_login_action;

    // �õ��û���������
    cgi_login_action = (s_cgi_login_action *)(p_shm + sizeof(int)*3);
    memcpy(name, cgi_login_action->username, _MAX_LEN_LOGIN_USERNAME);
    memcpy(pwd, cgi_login_action->password, _MAX_LEN_LOGIN_PASSWORD);
    printf("name=%s\n", name);
    printf("password=%s\n", pwd);

    // ����û����������Ƿ�Ϸ�,���������д�������ڴ���Ӧλ��
    ret = login_check(name, pwd);
    *(int *)(p_shm) =  ret;

    // ��¼�Ƿ������ش���
    if(0 != ret)
    {
        printf("login failed.\n");
        return -1;
    }
    
    return 0;
    
}  

// ����CGI�ĵ�¼�ɹ���advcҳ������
int handler_cgi_advc(char *p_shm)
{
    s_cgi_advc *p_cgi_advc;
    p_cgi_advc = (s_cgi_advc *)(p_shm + sizeof(int)*3);
    
    // pgh Ŀǰ�����������ڴ���գ���������΢Ӱ���ٶȣ��ɷ�Ľ�?
    memset(p_shm, 0, SEGSIZE);

    *(int *)p_shm = CGI_REQ_SUC;
    
    p_cgi_advc->num_camera = g_config.camara_id.camara_num.var;
    p_cgi_advc->video_port = g_config.para_TermRegister.VideoPort.var;
    strcpy(p_cgi_advc->device_IP, g_config.para_TermRegister.DeviceIP.var);

    return 0;
}

// �������������ݣ�ר����handler_cgi_detect_action()����
// Ŀǰ������ҳ�У�ֻд��һ������ͷ�ı������ã����Ӧ�ö�ÿ������ͷдһ�ױ�������
// ���ԣ�Ŀǰϵͳʵ�ַ�ʽ�ǣ���һ������ͷ�ı������ã�ʵ���ж�Ӧ���е�����ͷ����ÿ��������ͬ
int handler_cgi_detect_action_r(s_cgi_detect_action *p_cgi_detect_action)
{
   
    g_config.alarm_task.alarminA.var                        = p_cgi_detect_action->cgi_detect_action_step1.alarminA;         // �豸A:1 ���� 0 ֹͣ
    g_config.alarm_task.alarminB.var                        = p_cgi_detect_action->cgi_detect_action_step1.alarminB;         // �豸B:1 ���� 0 ֹͣ  
    g_config.alarm_task.movedetect.var                      = p_cgi_detect_action->cgi_detect_action_step1.movedetect;       // �ƶ����:1 ���� 0 ֹͣ
    g_config.alarm_task.Capture_enable.var                  = p_cgi_detect_action->Capture_enable;                           // ץ�ģ�1 ���� 0 ֹͣ
    g_config.alarm_task.Cap_count.var                       = p_cgi_detect_action->Cap_count;                                // ����ʱ����ץ������  
    g_config.alarm_task.alarmbell.var                       = p_cgi_detect_action->alarmbell;                                // �쾯�� ��1���� 0 ֹͣ 
    g_config.alarm_task.alarmbelltime_a.var                 = p_cgi_detect_action->alarmbelltime_a;                          // ������ʱʱ��  
    g_config.alarm_task.recording.var                       = p_cgi_detect_action->recording;                                // ¼�� ��1 ���� 0 ֹͣ
    g_config.alarm_task.recordingtime_r.var                 = p_cgi_detect_action->recordingtime_r;                          // ¼��ʱ��
    g_config.alarm_task.Capture_style.var                   = p_cgi_detect_action->Capture_style;                            // ץ��ͼƬ�洢��ַ 1 ���� 2ftp������                                           
    g_config.alarm_task.Cap_Time.var                        = p_cgi_detect_action->Cap_Time;                                 // ���ץ������ 
    strcpy(g_config.alarm_task.Detect_Ftp.var               , p_cgi_detect_action->Detect_Ftp);                              // ftp��������ַ
    strcpy(g_config.alarm_task.Detect_Ftp_user.var          , p_cgi_detect_action->Detect_Ftp_user);                         // ftp�û���
    strcpy(g_config.alarm_task.Detect_Ftp_pwd.var           , p_cgi_detect_action->Detect_Ftp_pwd);                          // ftp���� 
    strcpy(g_config.alarm_task.Detect_Ftp_Dir.var           , p_cgi_detect_action->Detect_Ftp_Dir);                          // ftp��ͼƬ�洢·��
    strcpy(g_config.alarm_task.week_plan.var                , p_cgi_detect_action->week_plan);                               // �����ƻ�(һλ����һ���һСʱ) 
    /*  ��g_config��Ӧ�����ļ���week_plan�ֶ����ݿ�����schedule_0 ~ schedule_6��
        148λ��week_plan����д�����������ļ��в���schedule_0 ~ schedule_6����ʼ����ʱ�򣬻��schedule_0 ~ schedule_6�����ݿ�����week_plan
        ����Ƶ�������õ�ʱ��Ϊ�˷��㣬ֱ�Ӷ�week_plan���������ã�������Ҫ����Щ���ݿ�����schedule_0 ~ schedule_6  */
    vs_config_weekplan2schedule(0);
    
#if 0
    // ������������ͷid, Ŀǰ�����ļ�ֻ����4����������ͷ
    strcpy(g_config.para_alarm_camera_0.camera_id_0.var, alarmconfig1_req.cameraList.camera[0].id);
    strcpy(g_config.para_alarm_camera_0.camera_id_1.var, alarmconfig1_req.cameraList.camera[1].id);
    strcpy(g_config.para_alarm_camera_0.camera_id_2.var, alarmconfig1_req.cameraList.camera[2].id);
    strcpy(g_config.para_alarm_camera_0.camera_id_3.var, alarmconfig1_req.cameraList.camera[3].id);
#endif

    return 0;

}


// д�����������ݣ�ר����handler_cgi_detect_action()����
int handler_cgi_detect_action_w(s_cgi_detect_action *p_cgi_detect_action)
{
    memset(p_cgi_detect_action, 0, SEGSIZE - sizeof(int)*3);
    
    p_cgi_detect_action->cgi_detect_action_step1.alarminA   = g_config.alarm_task.alarminA.var;          // �豸A:1 ���� 0 ֹͣ
    p_cgi_detect_action->cgi_detect_action_step1.alarminB   = g_config.alarm_task.alarminB.var;          // �豸B:1 ���� 0 ֹͣ  
    p_cgi_detect_action->cgi_detect_action_step1.movedetect = g_config.alarm_task.movedetect.var;        // �ƶ����:1 ���� 0 ֹͣ
    p_cgi_detect_action->Capture_enable                     = g_config.alarm_task.Capture_enable.var;    // ץ�ģ�1 ���� 0 ֹͣ
    p_cgi_detect_action->recording                          = g_config.alarm_task.recording.var;         // ¼�� ��1 ���� 0 ֹͣ
    p_cgi_detect_action->alarmbell                          = g_config.alarm_task.alarmbell.var;         // �쾯�� ��1���� 0 ֹͣ 
    p_cgi_detect_action->Capture_style                      = g_config.alarm_task.Capture_style.var;     // ץ��ͼƬ�洢��ַ 1 ���� 2ftp������
    p_cgi_detect_action->recordingtime_r                    = g_config.alarm_task.recordingtime_r.var;   // ¼��ʱ��
    p_cgi_detect_action->alarmbelltime_a                    = g_config.alarm_task.alarmbelltime_a.var;   // ������ʱʱ��  
    p_cgi_detect_action->Cap_count                          = g_config.alarm_task.Cap_count.var;         // ����ʱ����ץ������
    p_cgi_detect_action->Cap_Time                           = g_config.alarm_task.Cap_Time.var;          // ���ץ������ 
    strcpy(p_cgi_detect_action->Detect_Ftp                  , g_config.alarm_task.Detect_Ftp.var);       // ftp��������ַ
    strcpy(p_cgi_detect_action->Detect_Ftp_user             , g_config.alarm_task.Detect_Ftp_user.var);  // ftp�û���
    strcpy(p_cgi_detect_action->Detect_Ftp_pwd              , g_config.alarm_task.Detect_Ftp_pwd.var);   // ftp���� 
    strcpy(p_cgi_detect_action->Detect_Ftp_Dir              , g_config.alarm_task.Detect_Ftp_Dir.var);   // ftp��ͼƬ�洢·��
    strcpy(p_cgi_detect_action->week_plan                   , g_config.alarm_task.week_plan.var);        // �����ƻ�(һλ����һ���һСʱ) 


#ifdef debug_handler_cgi_detect_action
    printf("enter into handler_cgi_detect_action_w()");
    
    printf("p_cgi_detect_action=%d\n",              p_cgi_detect_action->cgi_detect_action_step1.alarminA   );
    printf("p_cgi_detect_action=%d\n",              p_cgi_detect_action->cgi_detect_action_step1.alarminB   );
    printf("p_cgi_detect_action=%d\n",              p_cgi_detect_action->cgi_detect_action_step1.movedetect );
    printf("p_cgi_detect_action.var=%d\n",          p_cgi_detect_action->Capture_enable                     );
    printf("p_cgi_detect_action=%d\n",              p_cgi_detect_action->recording                          );
    printf("p_cgi_detect_action=%d\n",              p_cgi_detect_action->alarmbell                          );
    printf("p_cgi_detect_action=%d\n",              p_cgi_detect_action->Capture_style                      );
    printf("p_cgi_detect_action=%d\n",              p_cgi_detect_action->recordingtime_r                    );
    printf("p_cgi_detect_action=%d\n",              p_cgi_detect_action->alarmbelltime_a                    );
    printf("p_cgi_detect_action=%d\n",              p_cgi_detect_action->Cap_count                          );                       
    printf("p_cgi_detect_action=%s\n",              p_cgi_detect_action->Detect_Ftp      );
    printf("p_cgi_detect_action=%s\n",              p_cgi_detect_action->Detect_Ftp_user );
    printf("p_cgi_detect_action=%s\n",              p_cgi_detect_action->Detect_Ftp_pwd  );
    printf("p_cgi_detect_action=%s\n",              p_cgi_detect_action->Detect_Ftp_Dir  );
    printf("p_cgi_detect_action=%s\n",              p_cgi_detect_action->week_plan       );
#endif

    return 0;
}

// ����CGI��alertҳ������
int handler_cgi_detect_action(char *p_shm)
{
    int send_rev;                                               // cgi�Ƿ������ݻ�����������

printf("enter into handler_cgi_detect_action()");
    
    s_cgi_detect_action *p_cgi_detect_action;
    p_cgi_detect_action = (s_cgi_detect_action *)(p_shm + sizeof(int)*3);
   
    //memset(p_cgi_detect_action, 0, SEGSIZE);                                  // pgh Ŀǰ��������գ���������΢Ӱ���ٶȣ��ɷ�Ľ�?
    *(int *)p_shm = CGI_REQ_SUC;                                // ���óɹ����
    send_rev = *(int *)(p_shm + sizeof(int)*2);                 // �õ����͡����ձ�־

    switch(send_rev)
    {
       case 0:                                                  // CGI��������, ��ʱvsӦ�ö�����������
         handler_cgi_detect_action_r(p_cgi_detect_action);
         vs_alarm_para_set(0);                                  // ������ʼ��        
         vs_config_write();                                     // ��д�����ļ�
         break;
       case 1:                                                  // CGI��������, ��ʱvsӦ��д����������
         handler_cgi_detect_action_w(p_cgi_detect_action);
         break;
       default:
         break;
    }
  
    return 0;
}

// ����alert_moveҳ������--������������
int handler_cgi_add_area_r(s_cgi_add_area *p_cgi_add_area)
{
    printf("enter into handler_cgi_add_area_r()");
    g_config.para_alarm_camera_0.DetectGrade.var = p_cgi_add_area->difference;
    strcpy(g_config.alarm_task.week_plan.var, p_cgi_add_area->dete_areas);
    vs_config_weekplan2schedule(0);
    
    // ��д�������ļ�
    vs_file_opt_conf_write();
    
    return 0;
}

// ����alert_moveҳ������--д����������
int handler_cgi_add_area_w(s_cgi_add_area *p_cgi_add_area)
{
    printf("enter into handler_cgi_add_area_w()");
    memset(p_cgi_add_area, 0, SEGSIZE - sizeof(int)*3);
    strcpy(p_cgi_add_area->dete_areas, g_config.alarm_task.week_plan.var);
    p_cgi_add_area->difference = g_config.para_alarm_camera_0.DetectGrade.var;

    return 0;
}

// ����alert_moveҳ������
int handler_cgi_add_area(char *p_shm)
{
    int send_rev;                                               // cgi�Ƿ������ݻ�����������
    
    s_cgi_add_area *p_cgi_add_area;
    p_cgi_add_area = (s_cgi_add_area *)(p_shm + sizeof(int)*3);
   
    
    *(int *)p_shm = CGI_REQ_SUC;                                // ���óɹ����
    send_rev = *(int *)(p_shm + sizeof(int)*2);                 // �õ����͡����ձ�־

    switch(send_rev)
    {
       case 0:                                                  // CGI��������, ��ʱvsӦ�ö�����������
         handler_cgi_add_area_r(p_cgi_add_area);
         break;
       case 1:                                                  // CGI��������, ��ʱvsӦ��д����������
         handler_cgi_add_area_w(p_cgi_add_area);
         break;
       default:
         break;
    }
  
    return 0;
}



/***********************************  sd������ ***********************************/

// ����sd_cardҳ������--д����������
int handler_cgi_sd_card_w(s_cgi_sd_card *p_cgi_sd_card)
{
    memset(p_cgi_sd_card, 0, SEGSIZE - sizeof(int)*3);
    
    p_cgi_sd_card->cgi_para_storeconf.sdStore_w      = g_config.para_store.SD_store.var;                // �Ƿ�SD���洢  0: ��ѡ��SD���洢  1: ѡ��SD���洢
    p_cgi_sd_card->cgi_para_sdinfo.sd_status         = g_config.para_store.SD_status.var;                // SD��״̬��0: �����ڣ�1:����
    p_cgi_sd_card->cgi_para_sdinfo.sd_capb           = g_config.para_store.SD_size_amount.var;           // SD����������
    p_cgi_sd_card->cgi_para_sdinfo.sd_apb            = g_config.para_store.SD_size_useable.var;          // SD��ʣ��������
    p_cgi_sd_card->cgi_para_sdinfo.alarm_mes         = g_config.para_store.SD_alarm.var;                 // ������Ϣ��:�ռ��������ռ��㹻
    p_cgi_sd_card->cgi_para_sdconf.sdOverlay_w       = g_config.para_store.SD_overWrite.var;             // SD���洢��Ƭ�������Ƿ��Զ����ǡ�0:�����ǡ���1:����
    p_cgi_sd_card->cgi_para_sdconf.sdBackup_w        = g_config.para_store.SD_backup.var;                // ����SD������Ƭ������SD���е���Ƭ���浽���ؼ����Ӳ���У� 0 :�����ݡ�1:����
    p_cgi_sd_card->cgi_para_sdconf.sdBackupclear_w   = g_config.para_store.SD_delete.var;                // ���ݺ�ѡ���0: ��ɾ����1:ɾ��

    return 0;
}

// ����sd_cardҳ������
int handler_cgi_sd_card(char *p_shm)
{
    //int send_rev;                                               // cgi�Ƿ������ݻ�����������
    
    s_cgi_sd_card *p_cgi_sd_card;
    p_cgi_sd_card = (s_cgi_sd_card *)(p_shm + sizeof(int)*3);
   
    *(int *)p_shm = CGI_REQ_SUC;                                // ���óɹ����
    
    handler_cgi_sd_card_w(p_cgi_sd_card);                        // CGI��������, ��ʱvsӦ��д����������
  
    return 0;
}

// ����SD�洢��������--������������
int handler_cgi_para_storeconf_r(s_cgi_para_storeconf *p_cgi_para_storeconf)
{
 //   p_cgi_para_storeconf->sdStore_w = g_config.para_store.SD_store.var;                // �Ƿ�SD���洢  0: ��ѡ��SD���洢  1: ѡ��SD���洢  
  // ww modified
    g_config.para_store.SD_store.var = p_cgi_para_storeconf->sdStore_w;
    printf("receive sdStore_w = %d\n",p_cgi_para_storeconf->sdStore_w);

    return 0;
}

// ����SD�洢��������
int handler_cgi_para_storeconf(char *p_shm)
{
    s_cgi_para_storeconf *p_cgi_para_storeconf;
    p_cgi_para_storeconf = (s_cgi_para_storeconf *)(p_shm + sizeof(int)*3);
   
    *(int *)p_shm = CGI_REQ_SUC;                                // ���óɹ����
    
    handler_cgi_para_storeconf_r(p_cgi_para_storeconf);         // CGI��������, ��ʱvsӦ�ö�����������
  
    return 0;
}

int handler_cgi_para_sdcrtl_refresh(s_cgi_para_sdcrtl *p_cgi_para_sdcrtl)
{

    return 0;
}

int handler_cgi_para_sdcrtl_clear(s_cgi_para_sdcrtl *p_cgi_para_sdcrtl)
{

    return 0;
}


int handler_cgi_para_sdcrtl_format()
{
    sd_format();    
    return 0;
}

int handler_cgi_para_sdcrtl_umount()
{
    sd_format();    
    return 0;
}

// ����SD������: 1: ˢ�� 2:�����Ƭ 3. SD����ʽ�� 4.ж��SD��
int handler_cgi_para_sdcrtl(char *p_shm)
{
    int id_oper;                                              
    
    s_cgi_para_sdcrtl *p_cgi_para_sdcrtl;
    p_cgi_para_sdcrtl = (s_cgi_para_sdcrtl *)(p_shm + sizeof(int)*3);
   
    //memset(p_shm, 0, SEGSIZE);                                  // pgh Ŀǰ�����������ڴ���գ���������΢Ӱ���ٶȣ��ɷ�Ľ�?
    *(int *)p_shm = CGI_REQ_SUC;                                // ���óɹ����

    id_oper = p_cgi_para_sdcrtl->sdhandle_w;
    
     switch(id_oper) // 1: ˢ�� 2:�����Ƭ 3. SD����ʽ�� 4.ж��SD��
    {
       case 1:                                                  
         handler_cgi_para_sdcrtl_refresh(p_cgi_para_sdcrtl);
         break;
       case 2:                                                  
         handler_cgi_para_sdcrtl_clear(p_cgi_para_sdcrtl);
         break;
       case 3:                                                  
         handler_cgi_para_sdcrtl_format();
         break;
       case 4:                                                  
         handler_cgi_para_sdcrtl_umount();
         break;
       default:
         break;
    }
  
    return 0;
}

// ����SD���������ã��� ��
int handler_cgi_para_sdconf_r(s_cgi_para_sdconf *p_cgi_para_sdconf)
{
    g_config.para_store.SD_delete.var       = p_cgi_para_sdconf->sdBackupclear_w;       // ���ݺ�ѡ���0: ��ɾ����1:ɾ��
    g_config.para_store.SD_backup.var       = p_cgi_para_sdconf->sdBackup_w;            // ����SD������Ƭ������SD���е���Ƭ���浽���ؼ����Ӳ���У� 0 :�����ݡ�1:����
    g_config.para_store.SD_overWrite.var    = p_cgi_para_sdconf->sdOverlay_w;           // SD���洢��Ƭ�������Ƿ��Զ����ǡ�0:�����ǡ���1:����
    
    return 0;
}

// ����SD����������
int handler_cgi_para_sdconf(char *p_shm)
{                                        
    s_cgi_para_sdconf *p_cgi_para_sdconf;
    p_cgi_para_sdconf = (s_cgi_para_sdconf *)(p_shm + sizeof(int)*3);
   
    *(int *)p_shm = CGI_REQ_SUC;                                // ���óɹ����

    handler_cgi_para_sdconf_r(p_cgi_para_sdconf);           
    return 0;
}



/***********************************  �������� ***********************************/

int handler_cgi_misc_w(s_cgi_misc *p_cgi_misc)
{
    memset(p_cgi_misc, 0, SEGSIZE - sizeof(int)*3);

    //������Ϣ
    p_cgi_misc->cgi_para_connect.dev_max_con                = g_config.para_connect.dev_max_con.var;
    strcpy(p_cgi_misc->cgi_para_connect.dvs_id              , g_config.para_connect.dvs_id.var);
    strcpy(p_cgi_misc->cgi_para_connect.ftp_password        , g_config.para_connect.ftp_password.var);
    strcpy(p_cgi_misc->cgi_para_connect.ftp_username        , g_config.para_connect.ftp_username.var);

    // �������
    strcpy(p_cgi_misc->cgi_para_network.ip_local            , g_config.para_network.ip_local.var);
    strcpy(p_cgi_misc->cgi_para_network.ip_mask             , g_config.para_network.ip_mask.var);
    strcpy(p_cgi_misc->cgi_para_network.ip_gateway          , g_config.para_network.ip_gateway.var);
    strcpy(p_cgi_misc->cgi_para_network.ip_DNS              , g_config.para_network.ip_DNS.var);
    strcpy(p_cgi_misc->cgi_para_network.ip_server_access    , g_config.para_network.ip_server_access_1.var);
    p_cgi_misc->cgi_para_network.port_server_access         = g_config.para_network.port_server_access.var;

    // ��ʾ���Ƽ�ͨ��Э�� �ַ����� 
    p_cgi_misc->cgi_para_osd.osd_enable                     = g_config.para_alarm_camera_0.osd_enable.var;
    p_cgi_misc->cgi_para_osd.tpox                           = g_config.para_alarm_camera_0.tpox.var;
    p_cgi_misc->cgi_para_osd.tpoy                           = g_config.para_alarm_camera_0.tpoy.var;
    p_cgi_misc->cgi_para_osd.osd_time_enable                = g_config.para_alarm_camera_0.osd_enable.var;
    p_cgi_misc->cgi_para_osd.cpox                           = g_config.para_alarm_camera_0.cpox.var;
    p_cgi_misc->cgi_para_osd.cpoy                           = g_config.para_alarm_camera_0.cpoy.var;

    strcpy(p_cgi_misc->cgi_para_osd.text                    , g_config.para_alarm_camera_0.discontent.var);
    strcpy(p_cgi_misc->cgi_para_osd.sTfmt                   , g_config.para_alarm_camera_0.sTfmt.var);

    // ͼ��Ʒ��--������� 
    p_cgi_misc->cgi_para_venc.videoPara                     = g_config.para_VENC.radio_rdvfmt.var; 
    p_cgi_misc->cgi_para_venc.birate                        = g_config.para_VENC.rate_bit.var;
    p_cgi_misc->cgi_para_venc.maxquant                      = g_config.para_VENC.badest_quality.var;
    p_cgi_misc->cgi_para_venc.quant1                        = g_config.para_VENC.best_quality.var;
    p_cgi_misc->cgi_para_venc.fps                           = g_config.para_VENC.FrameRate.var;
    p_cgi_misc->cgi_para_venc.bitype                        = g_config.para_VENC.VENC_type.var;
    p_cgi_misc->cgi_para_venc.gjfra                         = g_config.para_VENC.KeyFrameIntervals.var;
    p_cgi_misc->cgi_para_venc.videoformat                   = g_config.para_VENC.resolution.var;
    return 0;
}

// ������������ҳ�� -- misc
int handler_cgi_misc(char *p_shm)
{
    s_cgi_misc *p_cgi_misc;
    p_cgi_misc = (s_cgi_misc *)(p_shm + sizeof(int)*3);
   
    *(int *)p_shm = CGI_REQ_SUC;                                // ���óɹ����

    handler_cgi_misc_w(p_cgi_misc);           
    return 0;   
}

int handler_cgi_para_network_r(s_cgi_para_network *p_cgi_para_network)
{
    strcpy(g_config.para_network.ip_local.var               , p_cgi_para_network->ip_local         );
    strcpy(g_config.para_network.ip_mask.var                , p_cgi_para_network->ip_mask          );        
    strcpy(g_config.para_network.ip_gateway.var             , p_cgi_para_network->ip_gateway       );
    strcpy(g_config.para_network.ip_DNS.var                 , p_cgi_para_network->ip_DNS           );
    strcpy(g_config.para_network.ip_server_access_1.var     , p_cgi_para_network->ip_server_access );
    g_config.para_network.port_server_access.var            = p_cgi_para_network->port_server_access ;
    
    return 0;         
}

// ������������ҳ�� -- misc -- �������
int handler_cgi_para_network(char *p_shm)
{
    s_cgi_para_network *p_cgi_para_network;
    p_cgi_para_network = (s_cgi_para_network *)(p_shm + sizeof(int)*3);
   
    *(int *)p_shm = CGI_REQ_SUC;                                // ���óɹ����

    handler_cgi_para_network_r(p_cgi_para_network);           
    return 0;   
}

int handler_cgi_para_connect_r(s_cgi_para_connect *p_cgi_para_connect)
{
    g_config.para_connect.dev_max_con.var                   = p_cgi_para_connect->dev_max_con        ;
    strcpy(g_config.para_connect.dvs_id.var                 , p_cgi_para_connect->dvs_id)      ;       
    strcpy(g_config.para_connect.ftp_password.var           , p_cgi_para_connect->ftp_password);
    strcpy(g_config.para_connect.ftp_username.var           , p_cgi_para_connect->ftp_username);
         
    return 0;         
}

// ������������ҳ�� -- misc -- ������Ϣ
int handler_cgi_para_connect(char *p_shm)
{
    s_cgi_para_connect *p_cgi_para_connect;
    p_cgi_para_connect = (s_cgi_para_connect *)(p_shm + sizeof(int)*3);
   
    *(int *)p_shm = CGI_REQ_SUC;                                // ���óɹ����

    handler_cgi_para_connect_r(p_cgi_para_connect);           
    return 0;   
}


int handler_cgi_para_osd_r(s_cgi_para_osd *p_cgi_para_osd)
{
    g_config.para_alarm_camera_0.osd_enable.var             = p_cgi_para_osd->osd_enable       ;  
    g_config.para_alarm_camera_0.tpox.var                   = p_cgi_para_osd->tpox             ;
    g_config.para_alarm_camera_0.tpoy.var                   = p_cgi_para_osd->tpoy             ;
    g_config.para_alarm_camera_0.osd_enable.var             = p_cgi_para_osd->osd_time_enable  ;
    g_config.para_alarm_camera_0.cpox.var                   = p_cgi_para_osd->cpox             ;
    g_config.para_alarm_camera_0.cpoy.var                   = p_cgi_para_osd->cpoy             ;
    strcpy(g_config.para_alarm_camera_0.discontent.var      , p_cgi_para_osd->text)            ;
    strcpy(g_config.para_alarm_camera_0.sTfmt.var           , p_cgi_para_osd->sTfmt);
   
    return 0;         
}

// ������������ҳ�� -- misc -- ��ʾ���Ƽ�ͨ��Э��--�ַ�����
int handler_cgi_para_osd(char *p_shm)
{
    s_cgi_para_osd *p_cgi_para_osd;
    p_cgi_para_osd = (s_cgi_para_osd *)(p_shm + sizeof(int)*3);
   
    *(int *)p_shm = CGI_REQ_SUC;                                // ���óɹ����

    handler_cgi_para_osd_r(p_cgi_para_osd);           
    return 0;   
}


int handler_cgi_para_venc_r(s_cgi_para_venc *p_cgi_para_venc)
{
    g_config.para_VENC.radio_rdvfmt.var             = p_cgi_para_venc->videoPara    ; 
    g_config.para_VENC.rate_bit.var                 = p_cgi_para_venc->birate       ;
    g_config.para_VENC.badest_quality.var           = p_cgi_para_venc->maxquant     ;
    g_config.para_VENC.best_quality.var             = p_cgi_para_venc->quant1       ;
    g_config.para_VENC.FrameRate.var                = p_cgi_para_venc->fps          ;
    g_config.para_VENC.VENC_type.var                = p_cgi_para_venc->bitype       ;
    g_config.para_VENC.KeyFrameIntervals.var        = p_cgi_para_venc->gjfra        ;
    g_config.para_VENC.resolution.var               = p_cgi_para_venc->videoformat  ;

    return 0;         
}

// ������������ҳ�� -- misc -- ͼ��Ʒ��--������� 
int handler_cgi_para_venc(char *p_shm)
{
    s_cgi_para_venc *p_cgi_para_venc;
    p_cgi_para_venc = (s_cgi_para_venc *)(p_shm + sizeof(int)*3);
   
    *(int *)p_shm = CGI_REQ_SUC;                                // ���óɹ����

    handler_cgi_para_venc_r(p_cgi_para_venc);    
    //vs_vm_set_D1_CIF(g_config.para_vedio.SupportFormat.var, 0); 
    vs_vm_set_D1_CIF(g_config.para_VENC.resolution.var, 0); 
    return 0;   
}

// ������������ҳ�� -- misc -- �ָ���������
int handler_cgi_para_restore(char *p_shm)
{
printf("���лָ���������\n");
    // ���ú��������лָ���������
    vs_restore();  
    *(int *)p_shm = CGI_REQ_SUC;                                // pgh ���óɹ���ǣ�����CGI�ӵ��ɹ���־��Ӧ��ʾϵͳ��Ҫ������Ϣ
    vs_stop_thread();                                           // ֹͣ�����̣߳��Ӷ�ϵͳ����
              
    return 0;   
}

// ������������ҳ�� -- misc -- ��������
int handler_cgi_para_reboot(char *p_shm)
{
printf("��������8888888888888888888888888888\n");
    *(int *)p_shm = CGI_REQ_SUC;                                // pgh ���óɹ���ǣ�����CGI�ӵ��ɹ���־��Ӧ��ʾϵͳ��Ҫ������Ϣ
    vs_reboot();
         
    return 0;   
}


// ������������ҳ�� -- misc -- �汾����
int handler_cgi_para_update(char *p_shm)
{
    *(int *)p_shm = CGI_REQ_SUC;                                // pgh ���óɹ���ǣ�����CGI�ӵ��ɹ���־��Ӧ��ʾϵͳ��Ҫ������Ϣ
         
    return 0;   
}




/******************************************* ��Ƭ���&���� ɾ�� *****************************************************/

int printf_s_cgi_photo_search(s_cgi_photo_search *p_cgi_photo_search)
{
    printf("p_cgi_photo_search->begindate_w=%s\n",p_cgi_photo_search->begindate_w);
    printf("p_cgi_photo_search->beginhour_w=%d\n",p_cgi_photo_search->beginhour_w);
    printf("p_cgi_photo_search->beginminute_w=%d\n",p_cgi_photo_search->beginminute_w);
 
    printf("p_cgi_photo_search->enddate_w=%s\n",p_cgi_photo_search->enddate_w);
    printf("p_cgi_photo_search->endhour_w=%d\n",p_cgi_photo_search->endhour_w);
    printf("p_cgi_photo_search->endminute_w=%d\n",p_cgi_photo_search->endminute_w);
 
    
    printf("p_cgi_photo_search->pageno_w=%d\n",p_cgi_photo_search->pageno_w);
    printf("p_cgi_photo_search->searchtype_w=%d\n",p_cgi_photo_search->searchtype_w);
 
    printf("p_cgi_photo_search->bphoto_search=%d\n",p_cgi_photo_search->bphoto_search);
    printf("p_cgi_photo_search->photoSize_r=%d\n",p_cgi_photo_search->photoSize_r);
    printf("p_cgi_photo_search->pageSize_r=%d\n",p_cgi_photo_search->pageSize_r);
    printf("p_cgi_photo_search->pageNum_r=%d\n",p_cgi_photo_search->pageNum_r);

    printf("p_cgi_photo_search->photoUrl1_r=%s\n",p_cgi_photo_search->photoUrl1_r);
    printf("p_cgi_photo_search->photoUrl2_r=%s\n",p_cgi_photo_search->photoUrl2_r);
    printf("p_cgi_photo_search->photoUrl3_r=%s\n",p_cgi_photo_search->photoUrl3_r);
    printf("p_cgi_photo_search->photoUrl4_r=%s\n",p_cgi_photo_search->photoUrl4_r);
    printf("p_cgi_photo_search->photoUrl5_r=%s\n",p_cgi_photo_search->photoUrl5_r);
    printf("p_cgi_photo_search->photoUrl6_r=%s\n",p_cgi_photo_search->photoUrl6_r);
    printf("p_cgi_photo_search->photoUrl7_r=%s\n",p_cgi_photo_search->photoUrl7_r);
    printf("p_cgi_photo_search->photoUrl8_r=%s\n",p_cgi_photo_search->photoUrl8_r);
    printf("p_cgi_photo_search->photoUrl9_r=%s\n",p_cgi_photo_search->photoUrl9_r);
    
    return 0;
}

int printf_g_cgi_photo_search(s_photo_manage photo_manage)
{
    printf("photo_manage.begindate_w=%s\n", photo_manage.begindate_w.var);
    printf("photo_manage.beginhour_w=%d\n", photo_manage.beginhour_w.var);
    printf("photo_manage.beginminute_w=%d\n", photo_manage.beginminute_w.var);

    printf("photo_manage.enddate_w=%s\n", photo_manage.enddate_w.var);
    printf("photo_manage.endhour_w=%d\n", photo_manage.endhour_w.var);
    printf("photo_manage.endminute_w=%d\n", photo_manage.endminute_w.var);

    printf("photo_manage.pageno_w=%d\n", photo_manage.pageno_w.var);
    printf("photo_manage.searchtype_w=%d\n", photo_manage.searchtype_w.var);

    printf("photo_manage.bphoto_search=%d\n", photo_manage.bphoto_search.var);
    printf("photo_manage.photoSize_r=%d\n", photo_manage.photoSize_r.var);
    printf("photo_manage.pageSize_r=%d\n", photo_manage.pageSize_r.var);
    printf("photo_manage.pageNum_r=%d\n", photo_manage.pageNum_r.var);

    printf("photo_manage.photoUrl1_r=%s\n",photo_manage.photoUrl1_r.var);
    printf("photo_manage.photoUrl2_r=%s\n",photo_manage.photoUrl2_r.var);
    printf("photo_manage.photoUrl3_r=%s\n",photo_manage.photoUrl3_r.var);
    printf("photo_manage.photoUrl4_r=%s\n",photo_manage.photoUrl4_r.var);
    printf("photo_manage.photoUrl5_r=%s\n",photo_manage.photoUrl5_r.var);
    printf("photo_manage.photoUrl6_r=%s\n",photo_manage.photoUrl6_r.var);
    printf("photo_manage.photoUrl7_r=%s\n",photo_manage.photoUrl7_r.var);
    printf("photo_manage.photoUrl8_r=%s\n",photo_manage.photoUrl8_r.var);
    printf("photo_manage.photoUrl9_r=%s\n",photo_manage.photoUrl9_r.var);
    
    
    return 0;
}


int handler_cgi_photo_search_r(s_cgi_photo_search *p_cgi_photo_search)
{ 
   // printf_s_cgi_photo_search(p_cgi_photo_search);
    
    strcpy(g_config.photo_manage.begindate_w.var            ,  p_cgi_photo_search->begindate_w);
    strcpy(g_config.photo_manage.enddate_w.var              ,  p_cgi_photo_search->enddate_w);
    g_config.photo_manage.beginhour_w.var                   =  p_cgi_photo_search->beginhour_w;
    g_config.photo_manage.beginminute_w.var                 =  p_cgi_photo_search->beginminute_w;
    g_config.photo_manage.endhour_w.var                     =  p_cgi_photo_search->endhour_w;
    g_config.photo_manage.endminute_w.var                   =  p_cgi_photo_search->endminute_w;
    g_config.photo_manage.pageno_w.var                      =  p_cgi_photo_search->pageno_w;
    g_config.photo_manage.searchtype_w.var                  =  p_cgi_photo_search->searchtype_w;

    g_config.photo_manage.bphoto_search.var                 =  p_cgi_photo_search->bphoto_search;
  //g_config.photo_manage.pageno_w.var                      =  p_cgi_photo_search->pageNum_r;
  printf("p_cgi_photo_search->pageno_w=%d\n",p_cgi_photo_search->pageno_w);
  printf("p_cgi_photo_search->pageNum_r=%d\n",p_cgi_photo_search->pageNum_r);
    
  //  printf_g_cgi_photo_search(g_config.photo_manage);
    return 0;         
}



int handler_cgi_photo_search_w(s_cgi_photo_search *p_cgi_photo_search)
{

    memset(p_cgi_photo_search, 0, SEGSIZE - sizeof(int)*3);
    
    strcpy(p_cgi_photo_search->begindate_w         ,  g_config.photo_manage.begindate_w.var  );
    strcpy(p_cgi_photo_search->enddate_w           ,  g_config.photo_manage.enddate_w.var    );
    p_cgi_photo_search->beginhour_w         =  g_config.photo_manage.beginhour_w.var    ;
    p_cgi_photo_search->beginminute_w       =  g_config.photo_manage.beginminute_w.var  ;
    p_cgi_photo_search->endhour_w           =  g_config.photo_manage.endhour_w.var      ;
    p_cgi_photo_search->endminute_w         =  g_config.photo_manage.endminute_w.var    ;
    p_cgi_photo_search->pageno_w            =  g_config.photo_manage.pageno_w.var       ;
    p_cgi_photo_search->searchtype_w        =  g_config.photo_manage.searchtype_w.var   ;
  //  p_cgi_photo_search->pageNum_r           =  g_config.photo_manage.pageno_w.var       ;

    p_cgi_photo_search->bphoto_search               =  g_config.photo_manage.bphoto_search.var ;
    p_cgi_photo_search->photoSize_r                 =  g_config.photo_manage.photoSize_r.var   ;
    p_cgi_photo_search->pageSize_r                  =  g_config.photo_manage.pageSize_r.var    ;
    p_cgi_photo_search->pageNum_r                   =  g_config.photo_manage.pageNum_r.var     ;

    strcpy(p_cgi_photo_search->photoUrl1_r         ,  g_config.photo_manage.photoUrl1_r.var  );
    strcpy(p_cgi_photo_search->photoUrl2_r         ,  g_config.photo_manage.photoUrl2_r.var    );
    strcpy(p_cgi_photo_search->photoUrl3_r         ,  g_config.photo_manage.photoUrl3_r.var  );
    strcpy(p_cgi_photo_search->photoUrl4_r         ,  g_config.photo_manage.photoUrl4_r.var    );
    strcpy(p_cgi_photo_search->photoUrl5_r         ,  g_config.photo_manage.photoUrl5_r.var  );
    strcpy(p_cgi_photo_search->photoUrl6_r         ,  g_config.photo_manage.photoUrl6_r.var    );
    strcpy(p_cgi_photo_search->photoUrl7_r         ,  g_config.photo_manage.photoUrl7_r.var  );
    strcpy(p_cgi_photo_search->photoUrl8_r         ,  g_config.photo_manage.photoUrl8_r.var    );
    strcpy(p_cgi_photo_search->photoUrl9_r         ,  g_config.photo_manage.photoUrl9_r.var  );

    return 0;         
}

// ������Ƭ���&����
int handler_cgi_photo_search(char *p_shm)
{
    int send_rev;
    s_cgi_photo_search *p_cgi_photo_search;
    p_cgi_photo_search = (s_cgi_photo_search *)(p_shm + sizeof(int)*3);
   
    *(int *)p_shm = CGI_REQ_SUC;                                // ���óɹ����

    send_rev = *(int *)(p_shm + sizeof(int)*2);
    switch(send_rev) 
    {
       case 0:   
printf("������Ƭ���\n");
         handler_cgi_photo_search_r(p_cgi_photo_search);
         vs_photo_browse(0);
         break;
       case 1: 
 printf("������Ƭ����\n");
         handler_cgi_photo_search_w(p_cgi_photo_search);      
         break;
       case 3:                                                  
       default:
         break;
    }

    return 0;   
}


int handler_cgi_photo_delete_r(s_cgi_photo_delete *p_cgi_photo_delete)
{
    strcpy(g_config.photo_manage.del_img1_w.var                    ,  p_cgi_photo_delete->del_img1);
    strcpy(g_config.photo_manage.del_img2_w.var                    ,  p_cgi_photo_delete->del_img2);
    strcpy(g_config.photo_manage.del_img3_w.var                    ,  p_cgi_photo_delete->del_img3);
    strcpy(g_config.photo_manage.del_img4_w.var                    ,  p_cgi_photo_delete->del_img4);
    strcpy(g_config.photo_manage.del_img5_w.var                    ,  p_cgi_photo_delete->del_img5);
    strcpy(g_config.photo_manage.del_img6_w.var                    ,  p_cgi_photo_delete->del_img6);
    strcpy(g_config.photo_manage.del_img7_w.var                    ,  p_cgi_photo_delete->del_img7);
    strcpy(g_config.photo_manage.del_img8_w.var                    ,  p_cgi_photo_delete->del_img8);
    strcpy(g_config.photo_manage.del_img9_w.var                    ,  p_cgi_photo_delete->del_img9);

    g_config.photo_manage.del_imgall_w.var = p_cgi_photo_delete->del_img_all;

    return 0;         
}

// ������Ƭɾ��
int handler_cgi_photo_delete(char *p_shm)
{
    s_cgi_photo_delete *p_cgi_photo_delete;
    p_cgi_photo_delete = (s_cgi_photo_delete *)(p_shm + sizeof(int)*3);
   
    *(int *)p_shm = CGI_REQ_SUC;                                // ���óɹ����

    handler_cgi_photo_delete_r(p_cgi_photo_delete);    
    vs_delete_photo();
    return 0;   
}

// ������Ƭ��ʾ��ҳ��, ���������������
int handler_cgi_photo_puts(char *p_shm)
{

    
    return 0;   
}



/* ��handler��Ӧ������ƥ�����鵥Ԫ��  */
int cgi_getHandlerMatch(int id_cgi_handler)
{
	int i;
printf("rrrrrrrrrrrrrrrrrrrrrrrr id_cgi_handler=%d\n", id_cgi_handler);
	for(i=0; map_CGI_flag2Handler[i].cgi_handler!= '\0'; i++)
		if(id_cgi_handler == map_CGI_flag2Handler[i].id_cgi_handler)
		{	
			return i;
		}
	return -1;
}

// ���̼�ͨ����Ϣ�����̣߳�Ŀǰ���ù����ڴ淽ʽ
int thread_shm_main() 
{ 
    key_t key;                  
    key_t key_cgi;                // Ϊ�ⲿCGI����Թ����ڴ�����ź���
    int shm_id,sem_id; 
    int sem_id_cgi;                 // �ⲿCGI����Թ����ڴ�����ź���
    struct semid_ds buf; 

    int id_cgi_handler; 
    int id_cgi_data_DS;

    char *p_shm;
    // ���������������ڴ�
    key = ftok("./vs_shm",0); 
    key_cgi = ftok("./vs_shm_cgi",0); 
    //shm_id = shmget(key,SEGSIZE,IPC_CREAT |0604); 
    shm_id = shmget(key,SEGSIZE,IPC_CREAT |0777);
    if (-1 == shm_id){ 
        logger(TLOG_ERROR, "shared memory create error, system will stop");
        thread_stop_all();
        return -1; 
    }  
    p_shm = (char *)shmat(shm_id,0,0);   
    if (-1 == (int)p_shm){ 
        logger(TLOG_ERROR, "shared memory attach error, system will stop");
        thread_stop_all();
        return -1; 
    } 

    // ����������
    sem_id = sem_creat(key);                    // vs������Թ����ڴ�����ź���
    sem_id_cgi = sem_creat(key_cgi);            // �ⲿCGI����Թ����ڴ�����ź���
    
    while( !mapThreadID2Handler[THREAD_SHM].thread_quit_flag )
    {
        p(sem_id);      // vs�ȴ������ڴ�
        
        id_cgi_data_DS = *(int*)(p_shm + sizeof(int));
        id_cgi_handler = cgi_getHandlerMatch(id_cgi_data_DS);
        if(-1 == id_cgi_handler)
        {             
            logger(TLOG_WARNING, "no cgi handle.");
            continue;
        }
        
        map_CGI_flag2Handler[id_cgi_handler].cgi_handler(p_shm );    

        v(sem_id_cgi);    // �ⲿ������������ڴ�ʹ��
    }

    shmdt(p_shm); 
    shmctl(shm_id,IPC_RMID,&buf); 
    del_sem(sem_id); 
    del_sem(sem_id_cgi);
	
    return 0; 
//gcc -o shm shm.c -g 
} 



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

