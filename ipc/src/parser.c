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


int parser_core(char *buf)
{
    int ret;
    int cmd_head_ID;
    s_cmd_req_head *cmd_req_head = NULL;
    cmd_req_head = (s_cmd_req_head *)buf;

    // ���汾�ţ�������ֱ�ӷ���
    if(100 != cmd_req_head->version)
    {
        logger(TLOG_ERROR, "sm_scp_parser version error, version=%d", cmd_req_head->version);
        rountin_common_flag(CMD_ID_ERR_FORMAT_RES, MSG_RES_FLAG_ERR_FORMAT_VER, 0);
        return -1;
    }
    
    cmd_head_ID = cmd_req_head->ID;
    ret = sm_handler(cmd_head_ID, buf);
    return ret;
}

/* ��Ϣ�������߳�*/
void thread_parser_main()
{
    struct member *list_node;

    while( !mapThreadID2Handler[THREAD_PARSER].thread_quit_flag )
	{
	    // ȡ�ڵ㣬������գ��������ȴ���Ϣ�����̻߳���
	    list_node_get(&list_node);
	    if(NULL == list_node)
	    {
	        thread_syn_flag_clr(&syn_ctrl.mutex, &syn_ctrl.cond, &syn_ctrl.flag);
            thread_syn_wait(&syn_ctrl.mutex, &syn_ctrl.cond, &syn_ctrl.flag);
	    }

        // ���ǿգ�ȡ�½ڵ㣬����ʵ���������
        list_node_get(&list_node);
        parser_core(list_node->data);

        // �ͷŽڵ�ռ�
        free(list_node);
	}

	logger(TLOG_NOTICE, "%s quit", __FUNCTION__);
    return 0;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

