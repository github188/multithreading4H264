#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include <stdlib.h>         /*  for malloc() */
#include <string.h>         /*  for memcpy() */
#include <dirent.h>         /* for opendir() */
#include <sys/vfs.h>        /* for statfs() */
#include <stdio.h>          /* for sprintf() */

#include <sys/stat.h>        /* for stat() */
#include <unistd.h>
#include <sys/types.h>       /* for mkdir() */

#include "vs_main_rountin.h"
#include "vs_timer.h"
#include "vs_config.h"


typedef struct tagConfig_timer_param
{
    int         timer_id_detect;            // ��ʱ�ƶ����Ķ�ʱ�����
    int         timer_id_cap;               // ��ʱץ�ĵĶ�ʱ�����
}Config_timer_param;
Config_timer_param g_conf_timer;

// sx added auto
#include "vs_config_put_var_auto.h"
// sx added auto end 


// ��weekplan�е����ݿ�����schedule��
int vs_config_weekplan2schedule(int camera_index)
{
    switch (camera_index)
    {
        case 0:
            memcpy(g_config.para_alarm_camera_0.schedule_0.var, g_config.alarm_task.week_plan.var + 24 * 0, 24);
            memcpy(g_config.para_alarm_camera_0.schedule_1.var, g_config.alarm_task.week_plan.var + 24 * 1, 24);
            memcpy(g_config.para_alarm_camera_0.schedule_2.var, g_config.alarm_task.week_plan.var + 24 * 2, 24);
            memcpy(g_config.para_alarm_camera_0.schedule_3.var, g_config.alarm_task.week_plan.var + 24 * 3, 24);
            memcpy(g_config.para_alarm_camera_0.schedule_4.var, g_config.alarm_task.week_plan.var + 24 * 4, 24);
            memcpy(g_config.para_alarm_camera_0.schedule_5.var, g_config.alarm_task.week_plan.var + 24 * 5, 24);
            memcpy(g_config.para_alarm_camera_0.schedule_6.var, g_config.alarm_task.week_plan.var + 24 * 6, 24);
            break;
        case 1:
            memcpy(g_config.para_alarm_camera_1.schedule_0.var, g_config.alarm_task.week_plan.var + 24 * 0, 24);
            memcpy(g_config.para_alarm_camera_1.schedule_1.var, g_config.alarm_task.week_plan.var + 24 * 1, 24);
            memcpy(g_config.para_alarm_camera_1.schedule_2.var, g_config.alarm_task.week_plan.var + 24 * 2, 24);
            memcpy(g_config.para_alarm_camera_1.schedule_3.var, g_config.alarm_task.week_plan.var + 24 * 3, 24);
            memcpy(g_config.para_alarm_camera_1.schedule_4.var, g_config.alarm_task.week_plan.var + 24 * 4, 24);
            memcpy(g_config.para_alarm_camera_1.schedule_5.var, g_config.alarm_task.week_plan.var + 24 * 5, 24);
            memcpy(g_config.para_alarm_camera_1.schedule_6.var, g_config.alarm_task.week_plan.var + 24 * 6, 24);
            break;    
        case 2:
            memcpy(g_config.para_alarm_camera_2.schedule_0.var, g_config.alarm_task.week_plan.var + 24 * 0, 24);
            memcpy(g_config.para_alarm_camera_2.schedule_1.var, g_config.alarm_task.week_plan.var + 24 * 1, 24);
            memcpy(g_config.para_alarm_camera_2.schedule_2.var, g_config.alarm_task.week_plan.var + 24 * 2, 24);
            memcpy(g_config.para_alarm_camera_2.schedule_3.var, g_config.alarm_task.week_plan.var + 24 * 3, 24);
            memcpy(g_config.para_alarm_camera_2.schedule_4.var, g_config.alarm_task.week_plan.var + 24 * 4, 24);
            memcpy(g_config.para_alarm_camera_2.schedule_5.var, g_config.alarm_task.week_plan.var + 24 * 5, 24);
            memcpy(g_config.para_alarm_camera_2.schedule_6.var, g_config.alarm_task.week_plan.var + 24 * 6, 24);
            break;
        case 3:
            memcpy(g_config.para_alarm_camera_3.schedule_0.var, g_config.alarm_task.week_plan.var + 24 * 0, 24);
            memcpy(g_config.para_alarm_camera_3.schedule_1.var, g_config.alarm_task.week_plan.var + 24 * 1, 24);
            memcpy(g_config.para_alarm_camera_3.schedule_2.var, g_config.alarm_task.week_plan.var + 24 * 2, 24);
            memcpy(g_config.para_alarm_camera_3.schedule_3.var, g_config.alarm_task.week_plan.var + 24 * 3, 24);
            memcpy(g_config.para_alarm_camera_3.schedule_4.var, g_config.alarm_task.week_plan.var + 24 * 4, 24);
            memcpy(g_config.para_alarm_camera_3.schedule_5.var, g_config.alarm_task.week_plan.var + 24 * 5, 24);
            memcpy(g_config.para_alarm_camera_3.schedule_6.var, g_config.alarm_task.week_plan.var + 24 * 6, 24);
            break;
    }

    return 0;
}

// ��schedule�е����ݿ�����weekplan��
int vs_config_schedule2weekplan()
{
    memcpy(g_config.alarm_task.week_plan.var + 24 * 0, g_config.para_alarm_camera_0.schedule_0.var, 24);
    memcpy(g_config.alarm_task.week_plan.var + 24 * 1, g_config.para_alarm_camera_0.schedule_1.var, 24);
    memcpy(g_config.alarm_task.week_plan.var + 24 * 2, g_config.para_alarm_camera_0.schedule_2.var, 24);
    memcpy(g_config.alarm_task.week_plan.var + 24 * 3, g_config.para_alarm_camera_0.schedule_3.var, 24);
    memcpy(g_config.alarm_task.week_plan.var + 24 * 4, g_config.para_alarm_camera_0.schedule_4.var, 24);
    memcpy(g_config.alarm_task.week_plan.var + 24 * 5, g_config.para_alarm_camera_0.schedule_5.var, 24);
    memcpy(g_config.alarm_task.week_plan.var + 24 * 6, g_config.para_alarm_camera_0.schedule_6.var, 24);

    return 0;
}

// ��ȡ�����ļ�
int config_read(char *file_name)
{
    FILE *fp;
    char line[512];
    char name[48];      // ǰ������֣��涨�������20
    char var[256];      // �������ֵ
    char note[256];      // ע������
    
    char c;
    int start, pos;
	int flag_4_close = 0;       // var�����Ƿ���"��
	int flag_1_box = 0;
	int flag_1_box_end = 0;
    int flag = 0;
    char *ret = line;

    memset(&g_config, 0, sizeof(AllConfig));
    
    fp = fopen(file_name, "r");
    if ( fp == NULL )
    {
        return -1;
    }
    
    while( !feof(fp) )
    {
        ret = fgets(line, 512, fp);
        if ( ret == NULL )
        {
            break;
        }

        // ����line�Ľ���, ��=�ŷָ�. ������strstr, �Լ�������. ��Ϊ��ϣ��trim,ȥ��"�ŵȵȡ�ʹ��strstr�鷳��

		// ������Ч���� name���֣����������=��
		//              var���֣����Ժ���=�š�
		//              ���пո񶼺��ԡ�������trim���ܡ�var���֣����ϣ�����пո��"�ţ���Ҫ��"��������(��Ҫ��ǰ�沿��)
		//              ������[]ʱ,Ч������name[0]=='\0', ��var��ֵΪ[]�е��ڲ�,var�Ǿ���trim�ġ�
		// ���������У�
		//aaa=bbbb
		//  aaa   =    bbb
        //  aaa   == bbb
        //  aaa   ===  bb===bb
        //  aaa  ======= "  b ==  b   b  "
        //[abc]
        //  [  abc  ]
        //  [  a  b  c ]

        // ����ǡ������ aaa = bb bb    ��ֻ�൱�� aaa = bb�ˣ���Ϊ bb bb �м��пո�ȴû��"�Ű�������
        
		start = 0;
		flag = 0;
		pos = 0;
		name[0] = '\0';
		var[0] = '\0';
		note[0] = '\0';
		flag_4_close = 0;
		flag_1_box = 0;
		flag_1_box_end = 0;
		c = line[0];

		// |_| AAA |_| = |_| BBB |_|
		// 0   1  2    3     4  5     flag
		while( c != '\0' )
		{
			c = line[pos];
			if ( (( c == ' ' || c == '\t') && flag_4_close == 0 ) || c == '\0' || c == '\n' || c == '\r' || (c == '=' && flag <= 3) || (c == '/' && line[pos+1] == '/') )
			{
				if ( flag == 0 || flag == 3 )
				{
					start = pos;        // �����ո���ʼ����ǰ��
				}
				if ( flag == 1 )        // ����
				{
					memcpy( name, line+start, pos-start );
					name[pos-start] = '\0';
					flag ++;
				}
				if ( flag == 4 )
				{
					if (flag_1_box == 1)
					{
						if (start > 0)
						{
							memcpy( var, line+start, flag_1_box_end-start+1);
							var[flag_1_box_end-start+1] = '\0';
						}
						if ( flag_1_box_end-start >= 0 && var[flag_1_box_end-start] == '"' && flag_4_close == 1 )
						{
							var[flag_1_box_end-start] = '\0';  // ȥ�� " ��
						}
						break;
					}
					if ( line[start-1] == '/' && line[start] == '/' )       //  a=//ccc �������var�ǿ�
					{
						strcpy(note, line+start-1);
						break;
					}
					memcpy( var, line+start, pos-start );
					var[pos-start] = '\0';
					flag ++;
					
					if ( pos-start-1 >= 0 && var[pos-start-1] == '"' && flag_4_close == 1 )
					{
						var[pos-start-1] = '\0';  // ȥ�� " ��
					}
					break;
				}
				if ( c == '=' && ( flag == 2 || flag == 5) )
				{
					flag = 3;
				}
			}
			else
			{
				if ( flag_1_box == 1 && c != ' ' && c != '\t' && c != ']' )
				{
					if (start < 0)
					{
						start = pos;
					}
					flag_1_box_end = pos;
				}
				if ( flag == 0 || flag == 3 )
				{
					flag ++;
					start = pos;

					if ( flag == 1 && c == '[' )
					{
						flag = 4;
						flag_1_box = 1;	// �����ŷ�ʽ
						flag_4_close = 1;		// ����"��
						start = -1;
					}
					if ( flag == 4 && c == '"' )
					{
						flag_4_close = 1;		// ����"��
						start ++;
						flag_1_box = 1;
					}
				}
				if ( flag == 4 && flag_1_box == 1 && c == ']' )
				{
					if (start > 0)
					{
						memcpy( var, line+start, flag_1_box_end-start+1);
						var[flag_1_box_end-start+1] = '\0';
					}
					break;
				}
			}
			pos ++;
		}

		// ��pos��ʼ������noteע������
		if (note[0] =='\0')
		{
			while (line[pos] != '\0')
			{
				if ( line[pos] == '/' && line[pos+1] == '/' )
				{
					strcpy(note, line+pos);
					/*
					int note_len = strlen(note);
					if ( note[note_len-1] == '\n' )
					    note[note_len-1] = '\0';
					if ( note[note_len -2] == '\r' )
					    note[note_len -2] = '\0';
					*/
					break;
				}
				pos ++;
			}
		}
		if (name[0] == '\r' || name[0] == '\n')
		{
			continue;
		}
        // ����ȫ�ֱ����ĸ�ֵ
        vs_config_put_var(name, var, note);
        
    }

       
    return 0;
}

// дһ�д�[]����Ϣ
int vs_config_box_line(FILE *fp, char *line, char *name, char *note)
{
    int max_first_len = 48;     // ǰ���48���ַ�
    int len = 0;

    //fprintf(fp, "\r\n");
    
    sprintf(line, "[%s]", name);
    if ( note[0] != '\0' )
    {
        len = strlen(line);     // �ƻ�ǰ��ĳ��ȣ�׷��һ���Ŀո񣬶���
        if (len < max_first_len)
        {
            memset(line+len, ' ', max_first_len - len);
            line[max_first_len] = '\0';
        }
        strcat(line, note);  // ��ע��׷��
    }
    fprintf(fp, "%s\r\n", line);
    return 0;
}

// дһ��������ֵ. name = var   note(//...)
int vs_config_var_line(FILE *fp, char *line, char *name, char *var, char *note)
{
    int max_first_len = 48;     // ǰ���48���ַ�
    int len = 0;

    if ( name[0] == '\0' )
    {
        return 0;
    }
    if ( var[0] == '\0' )
    {
        sprintf(line, "  %s = ", name);
    }
    else
    {
        sprintf(line, "  %s = \"%s\"", name, var);
    }
    if ( note[0] != '\0' )
    {
        len = strlen(line);     // �ƻ�ǰ��ĳ��ȣ�׷��һ���Ŀո񣬶���
        if (len < max_first_len)
        {
            memset(line+len, ' ', max_first_len - len);
            line[max_first_len] = '\0';
        }
        if ( note[0] == '\0')
        {
            strcat(line, "\n");
        }
        else
        {
            strcat(line, note);  // ��ע��׷��
            if (note[strlen(note)-1] != '\n' )
            {
                strcat(line, "\n");
            }
        }
    }
    fprintf(fp, "%s\r\n", line);
    return 0;
}

// дһ��������ֵ. name = var   note(//...)
int vs_config_var_line2(FILE *fp, char *line, char *name, long var, char *note)
{
    int max_first_len = 48;     // ǰ���48���ַ�
    int len = 0;

    if ( name[0] == '\0' )
    {
        return 0;
    }

    sprintf(line, "  %s = %ld", name, var);
    if ( note[0] != '\0' )
    {
        len = strlen(line);     // �ƻ�ǰ��ĳ��ȣ�׷��һ���Ŀո񣬶���
        if (len < max_first_len)
        {
            memset(line+len, ' ', max_first_len - len);
            line[max_first_len] = '\0';
        }
        if ( note[0] == '\0')
        {
            strcat(line, "\n");
        }
        else
        {
            strcat(line, note);  // ��ע��׷��
            if (note[strlen(note)-1] != '\n' )
            {
                strcat(line, "\n");
            }
        }
    }
    fprintf(fp, "%s\r\n", line);
    return 0;
}


// ���棬ȫ�����Ƕ�ʱ���������ȴ�ȫ�ֱ����з�������ʱ�������ټӶ�ʱ
int vs_config_get_curtime(struct tm *pout)
{
    time_t curtime;
    struct tm *p;
    curtime = vs_get_center_time();         // �õ�����ʱ��
    p = gmtime( &curtime );
    memcpy(pout, p, sizeof(struct tm));
    return 0;
}

// ����Ķ�ʱ�ص�
int vs_config_hour_callback(void *data)
{
    struct tm t;
    char *pvar = NULL;
    int i;
    int index;
    
    vs_config_get_curtime(&t);

    if (t.tm_wday == 6) // ������
    {
        index = 0;      // ����Э��Ĳ���ʱ�䣬�Ǵ������տ�ʼ
    }
    else
    {
        index = (t.tm_wday +1) * 24;     // 0��ʾ����һ
    }
    index += t.tm_hour;     // �ڲ���ʱ����ĵ�ǰʱ�̡�

    // �����Ĳ���ʱ��...
    // target... ���ڻ�û���𶯱����ĳ����Ժ��ټӡ�
    
    // �жϵ�ǰ�Ƿ����������
    for ( i = 0; i < _MAX_CAMERA; i ++ )
    {
/*        pvar = g_config.week_time.week_alarm_detect_time.var;       // ���Ǳ���֮�ƶ���Ⲽ��ʱ��
        if ( strlen(pvar) > index )            // ��ʼ������ʱ�ƶ����
        {
            if ( pvar[index] > '0' )
            {
                //����Ӧ����ΰ�ĺ������� target_data�����ڴ�
                vs_vm_set_camera_target(i, RTV_TIME_DETECT, NULL, 1);        // ������������ʱ�ƶ��������
            }
            else
            {
                //ȡ����ʱ�ƶ����
                vs_vm_set_camera_target(i, RTV_REMOVE_TIME_DETECT, NULL, 1); // ȡ����ʱ�ƶ��������
            }
        }
*/
    }
    // ����ٴ�׷����һ����Ķ�ʱ������
    vs_config_detect_add_timer();
    return 0;
}
// �ƶ�����׷�Ӷ�ʱ������
int vs_config_detect_add_timer()
{
    struct tm t;
    int next_time;

    if (g_conf_timer.timer_id_detect >= 0)
    {
        // ɾ��ԭ���Ķ�ʱ��
        //vs_timer_chain_node_del_by_timerid(g_conf_timer.timer_id_detect);
    }
    vs_config_get_curtime(&t);

    // ���ڶ�ʱΪÿ����Сʱ��һ�Ρ��˴����ж�
    next_time = 3600 - ( t.tm_min * 60 + t.tm_sec );

    // Ȼ����׷��
    //vs_timer_add2(next_time, 1, &g_timer_id, vs_config_hour_callback, NULL);
    //vs_timer_add2(15, 1, &g_timer_id, vs_config_hour_callback, NULL);
    printf("�������㶨ʱ����15��֮��ʼִ��(Ϊ�˲��ԣ��ĳɵ�15��!)\n");
    return 0;
}

// ����ȫ�ֲ���������һ���Ķ�ʱ������
int vs_config_init_timer()
{
    memset(&g_conf_timer, 0, sizeof(Config_timer_param));
    g_conf_timer.timer_id_detect = -1;
    g_conf_timer.timer_id_cap = -1;

    // �ҿ�vs_timer.h�У���಻����30����ʱ����㣬Ҳ��֪���Ƿ�ᳬ����
    
    // ׷���ƶ����Ķ�ʱ������
    vs_config_detect_add_timer();
    return 0;
}

// sx added auto
#include "vs_config_write_auto.h"
// sx added auto en

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

