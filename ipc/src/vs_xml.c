/***********************************************************************************
*              Copyright 2007 - 2010, Megaeyes. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_server.c
* Description: main model .
*
* History:
* Version   Date              Author        DefectNum    Description
* 1.1       2007-08-22   q60002125     NULL            Create this file.
***********************************************************************************/




#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include "stdlib.h"
#include "vs_xml.h"
#include "sm_parser.h"



/* ��������ͷ��ţ�������Ӧxml  ��ʽ�ַ��� */
/************************************************************************
 * ������: buf_construct_xml
 * ��  ��: pgh, sx
 * ��  ��: ��������ͷ���, ��������ͷ��ţ�������Ӧxml  ��ʽ�ַ���
 * �������: int cmd_head_ID, ����ͷ���,���� ע�� TermRegister_Req�ı�� 2002, ����2003�� 
 *           char *p_struct_data, �Ǳ�����ͷ��Ӧ��struct������, ������������ݲ�����ȷ����xml
 * �������: int *length_xml, ���ص�xml�ĳ���.
 *           char *out_xml_buf, ���ص�xml�ַ���, ���==NULL,��ʾ�����֪xml�ĳ����Ա�����ڴ�
 * ����ֵ: 0: �ҵ�������, -1: û���ҵ�����ṹ
************************************************************************/
int buf_construct_xml(int cmd_head_ID, char *p_struct_data, int *length_xml, char *out_xml_buf)
{
    int struct_index;

    if ( length_xml == NULL )
        return -1;

    *length_xml = 0;
    struct_index = -1;              // ����ͷ�����struct�Ķ�Ӧ��ϵ
    
    switch (cmd_head_ID) 
    {
        case XML_TYPE_ID_0:
            struct_index = s_member_node_index;
            break;
        default:
            break;
    }

    if (struct_index >= 0)
    {
        *length_xml = struct2xml(p_struct_data, out_xml_buf, struct_index, all_xml_find_desc );
        return 0;
    }
    return -1;
}

/************************************************************************
 * ������: all_xml_find_desc
 * ��  ��: sx
 * ��  ��: ����xml��Ӧ��struct����, ����struct������Ϣ. ��Щ��Ϣʹ�ù�������+�˹�����.
 *         ����ֻ��Ҫ������������� all_xml_find_desc, �������������Ҫʹ���ߵ���
 * �������: int index, ��ʹ�÷�����,ͨ��Ϊ ��struct��name + "_index", ���Ƿ���enum��ö�ٹ��˱���
 * �������: INPUT_MESSAGE *out_inputmsg, ���ص�������Ϣ(����Ϣ)
 * ����ֵ: ���ص�������Ϣ(�ֲ���Ϣ)
************************************************************************/
char * all_xml_find_desc(int index, INPUT_MESSAGE *out_inputmsg)
{
    if (index == s_member_node_index)
    {
        INPUT_MESSAGE msg = { "s_member_node", 9, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"ID",              sizeof(int),             TYPE_INT,     0,            0 },
            {"name",            XML_MAXLEN_NAME,         TYPE_STRING,  0,            0 },
            {"srcaddr",         XML_MAXLEN_ADDR,         TYPE_STRING,  0,            0 },
            {"srcmac",          XML_MAXLEN_MAC,          TYPE_STRING,  0,            0 },
            {"srcport",         sizeof(int),             TYPE_INT,     0,            0 },
            {"proto",           XML_MAXLEN_PROTO,        TYPE_STRING,  0,            0 },
            {"destaddr",        XML_MAXLEN_ADDR,         TYPE_STRING,  0,            0 },
            {"destmac",         XML_MAXLEN_MAC,          TYPE_STRING,  0,            0 },
            {"destport",        sizeof(int),             TYPE_INT,     0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }
  
    if (index == s_xml_test_index)
    {
        INPUT_MESSAGE msg = { "s_xml_test", 3, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"DestAddr",        XML_MAXLEN_ADDR,         TYPE_STRING,  0,            0 },
            {"DestMac",         XML_MAXLEN_MAC,          TYPE_STRING,  0,            0 },
            {"DestPort",        sizeof(int),             TYPE_INT,     0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

    if (index == s_opera_passwd_index)
    {
        INPUT_MESSAGE msg = { "s_opera_passwd", 2, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"username",        _LEN_NORMAL,             TYPE_STRING,  0,            0 },
            {"password",        _LEN_NORMAL,             TYPE_STRING,  0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

    if (index == s_passwd_set_req_index)
    {
        INPUT_MESSAGE msg = { "s_passwd_set_req", 2, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"passwd",          sizeof(s_opera_passwd),  TYPE_STRUCT,  0,            s_opera_passwd_index },
            {"passwd_new",      sizeof(s_opera_passwd),  TYPE_STRUCT,  0,            s_opera_passwd_index }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

    if (index == s_passwd_del_req_index)
    {
        INPUT_MESSAGE msg = { "s_passwd_del_req", 2, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"passwd",          sizeof(s_opera_passwd),  TYPE_STRUCT,  0,            s_opera_passwd_index },
            {"username",        _LEN_NORMAL,             TYPE_STRING,  0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

    if (index == s_time_syn_req_index)
    {
        INPUT_MESSAGE msg = { "s_time_syn_req", 2, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"passwd",          sizeof(s_opera_passwd),  TYPE_STRUCT,  0,            s_opera_passwd_index },
            {"time_current",    _LEN_NORMAL,             TYPE_STRING,  0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

    if (index == s_ip_get_res_index)
    {
        INPUT_MESSAGE msg = { "s_ip_get_res", 2, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"ip_inner",        _LEN_IPADDR,             TYPE_STRING,  0,            0 },
            {"ip_outer",        _LEN_IPADDR,             TYPE_STRING,  0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

    if (index == s_ip_set_req_index)
    {
        INPUT_MESSAGE msg = { "s_ip_set_req", 3, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"passwd",          sizeof(s_opera_passwd),  TYPE_STRUCT,  0,            s_opera_passwd_index },
            {"ip_inner",        _LEN_IPADDR,             TYPE_STRING,  0,            0 },
            {"ip_outer",        _LEN_IPADDR,             TYPE_STRING,  0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

    if (index == s_hb_get_res_index)
    {
        INPUT_MESSAGE msg = { "s_hb_get_res", 2, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"enabled",         _LEN_NORMAL,             TYPE_STRING,  0,            0 },
            {"pri_sec",         _LEN_NORMAL,             TYPE_STRING,  0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }
    
    if (index == s_hb_set_req_index)
    {
        INPUT_MESSAGE msg = { "s_hb_set_req", 3, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"passwd",          sizeof(s_opera_passwd),  TYPE_STRUCT,  0,            s_opera_passwd_index },
            {"enabled",         sizeof(int),             TYPE_INT,     0,            0 },
            {"pri_sec",         sizeof(int),             TYPE_INT,     0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

    

    if (index == s_ip_rt_get_res_index)
    {
        INPUT_MESSAGE msg = { "s_ip_rt_get_res", 1, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"hb_rt_ip",        _LEN_IPADDR,             TYPE_STRING,  0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }
    
    if (index == s_ip_rt_set_req_index)
    {
        INPUT_MESSAGE msg = { "s_ip_rt_set_req", 2, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"passwd",          sizeof(s_opera_passwd),  TYPE_STRUCT,  0,            s_opera_passwd_index },
            {"hb_rt_ip",        _LEN_IPADDR,             TYPE_STRING,  0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

/********************************************** IPC begin ***********************************************/
    if (index == s_rtv_str_req_index)
    {
        INPUT_MESSAGE msg = { "s_rtv_str_req", 2, TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"passwd",          sizeof(s_opera_passwd),  TYPE_STRUCT,  0,            s_opera_passwd_index },
            {"flag_start",      sizeof(int),             TYPE_INT,     0,            0 }
        };
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

    return NULL;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
