/*****************************************************************
*     2007.9.5 ��� "��Ƶ�������ӿ�Э�飨VSP��3.0"�Ķ�ȡ����
*****************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include <stdio.h>
#include <string.h>
#include <expat.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "vs_xml_parser_new.h"

#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif

#define BUFFSIZE        8192                // �������ڲ���ʱ


// ���лᱻ�ӽṹ���õ�,����������һ����,����find_sub_struct_desc��������ʹ��
enum sub_struct_index
{
    DEMO_INDEX = 0,                      // 0
    Record_INDEX,                          // 1
    StoreRecord_INDEX,                 // 2
    StoreRecord2_INDEX,               // 3
    test_xml_INDEX
};

/********************************************************************
*                 ��¼�ṹ��Ƕ�׹�ϵ�Ľ��ṹ
********************************************************************/
typedef struct tagSAVE_XML_PARSE_DATA
{
    FIELD_TYPE * fdarray;                 // ��ǰ����ʲô�ֶ�������,����λ��ĳ����struct����
    int fdcount;                          // ��ǰfdarray������
    int in_fdarray_index;               // ��ǰ���,��fdarray�е�����λ��
    int getted_data;                       // 0,1, ��ǰ����ֵ�Ѹ���,���������ظ���ֵ����
    int in_parent_fdarray_index;       // ��ǰ���,�ڸ��ṹ��fdarray�е�����λ��
    int offset;                           // ��ǰ���,�ڵ�ǰ�ṹ���е�����ƫ��λ��
    char start_sub_structing;           // 0,��û�����ӽ��,1,��ʼѭ�������ӽ��,2������

    int loop_index;                      // �������ʱ,����ά��.
    int loop_current_count;             // �������ʱ,�䵱ǰ��ά��. (ֻ��һ���ֵܼ�¼)
    char start_name[50];                 // �ж�ÿ����������

    struct tagSAVE_XML_PARSE_DATA * next;             // �ֵ�֮��һ���
    struct tagSAVE_XML_PARSE_DATA * first_prev;      // �ֵܵ�һ��(ÿ���ֵܶ���¼)
    struct tagSAVE_XML_PARSE_DATA * end_next;        // �ֵ���ĩһ��(ֻ��һ���ֵܼ�¼)

    struct tagSAVE_XML_PARSE_DATA * subfirst;        // ������struct֮��һ��
    struct tagSAVE_XML_PARSE_DATA * parent;          // ������

    struct tagSAVE_XML_PARSE_DATA * allsave_next;   // ͳһ����ʱ,������
}SAVE_XML_PARSE_DATA,*PSAVE_XML_PARSE_DATA;

/********************************************************************
*                 �Ը��ֽṹ��������Ϣ�ı���
********************************************************************/
typedef struct tagFDARRAY_PTR
{
    INPUT_MESSAGE inputmsg;          // ���ڵ�һλ
    int index;
    FIELD_TYPE * fdarray;
    struct tagFDARRAY_PTR *next;
}FDARRAY_PTR,*PFDARRAY_PTR;

/********************************************************************
*                   �������������ܱ�
********************************************************************/
typedef struct PARSER_FUN_PARAM
{
    char *outbuf;                           // ���ؽṹ��buf
    int write_buf_len;                      // ��дoutbufΪxmlʱ����¼�䳤��
    int ret_err;                              // ���ش�����Ϣ
    SAVE_XML_PARSE_DATA * head;
    SAVE_XML_PARSE_DATA * current;

    SAVE_XML_PARSE_DATA * allsave;
    SAVE_XML_PARSE_DATA * allsave_end;

    FDARRAY_PTR * fdarrayptr;     // fdarray������

    // ����ģʽר��
    int easymode;                         // �Ƿ����ģʽ 0,1
    char easymode_findname[50];             // ��easyģʽ��,��Ҫ���ҵ�����
    int easymode_ret_type;         // ����ģʽ��,��Ҫ���ص�����. int or string
    int easymode_finded;             // �ҵ�û�� 0: ��δ�ҵ�. 1.�ҵ��˵���δ��ֵ. 2.�����.

    void * fun;                               // ��ס���ε��õĺ�����
}PARSER_FUN_PARAM,*PPARSER_FUN_PARAM;

/********************************************************************
*                         �м���ɺ���
********************************************************************/
static int find_xml_value(void *data, int index, const char *el);
static void XMLCALL start_xml(void *data, const char *el, const char **attr);
static int XMLCALL XMLEncodingHandle(void *data, const char *el, XML_Encoding *info);
//int GBK_convert(void *data, const char *s);
static void XMLCALL end_xml(void *data, const char *el);
static void XMLCALL chardatahandle(void *data, const char *el, int len);

// --------------------------------------------------------------------------------------
/********************************************************************
*                          ���� �ṹ�� ����
********************************************************************/
typedef struct tagRecord
{
    char BeginTime[100];
    char EndTime[100];
}Record;
typedef struct tagStoreRecord
{
    int DevType;
    int ID;
    Record record[8];
}StoreRecord;
typedef struct tagStoreRecord2
{
    int DevType;
    int ID;
    Record record[8];
}StoreRecord2;
typedef struct tagtest_xml
{
    int ID[3];
    StoreRecord storerecord[3];
    StoreRecord2 storerecord2[3];
    int ID2;
}test_xml;







/************************************************************************
 * ������: malloc_struct
 * ��  ��: sx
 * ��  ��: ��������������ṹ��������Ϣ,����һ��INPUT_MESSAGE,��malloc
 *          ����һ��FIELD_TYPE*������
 * �������: INPUT_MESSAGE *in_inputmsg,������������Ϣ
 *          FIELD_TYPE * in_fdarray,�����ĸ��ֶε���ϸ��Ϣ
 * �������: char *out_msg_buf,���Ƶ� INPUT_MESSAGE *in_inputmsg��Ϣ
 * ����ֵ: ���Ƶ� FIELD_TYPE * in_fdarray ��ϸ��Ϣ. (malloc������)
************************************************************************/
char * malloc_struct( INPUT_MESSAGE *in_inputmsg,
                            char *out_msg_buf,
                            FIELD_TYPE * in_fdarray)
{
    FIELD_TYPE * buf_fdarray;
    int i;
    if ( in_inputmsg == NULL )
    {
        return NULL;
    }
    if ( in_inputmsg->input_fdarray_count > 10000 || in_inputmsg->input_fdarray_count <= 0)
    {
        return NULL;
    }

    buf_fdarray = malloc( in_inputmsg->input_fdarray_count * sizeof(FIELD_TYPE) );
    if (buf_fdarray)
    {
        memcpy(buf_fdarray, in_fdarray, in_inputmsg->input_fdarray_count * sizeof(FIELD_TYPE) );
        // У������һ������ֵ:�ǽṹ������� �� int a[3]
        for(i=0; i <in_inputmsg->input_fdarray_count; i++)
        {
            if ( buf_fdarray[i].type != TYPE_STRUCT && buf_fdarray[i].repeat > 1 )
                buf_fdarray[i].only_struct_index = 0;       // ����Ϊ���Ժ�ͳ�����鵱ǰ����ʹ��.
        }
    }
    memcpy( out_msg_buf, in_inputmsg, sizeof(INPUT_MESSAGE) );
    return (char *)buf_fdarray;
}


/************************************************************************
 * ������: find_sub_strcut_desc
 * ��  ��: sx
 * ��  ��: ����������Ϣ,���ظýṹ�������������Ϣ
 *           1.�������,���Ը������·�ʽ,�����Լ��ĺ���,������������Ϊ
 *             xml2struct,struct2xml�ĵ�4������������.
 *           2.���溯����,������ʾ����.������struct������Ϣ,Ҫʹ���������
 * �������: int index,����
 *            FIELD_TYPE * in_fdarray,�����ĸ��ֶε���ϸ��Ϣ
 * �������: char *out_msg_buf,���Ƶ� INPUT_MESSAGE *in_inputmsg��Ϣ
 * ����ֵ: ���Ƶ� FIELD_TYPE * in_fdarray ��ϸ��Ϣ. (malloc������)
************************************************************************/
char * find_sub_strcut_desc(int index, INPUT_MESSAGE *out_inputmsg)
{
    if (index == Record_INDEX)
    {
        INPUT_MESSAGE msg = { "Record",2,TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"BeginTime",      100,               TYPE_STRING,   0, 0 },
            {"EndTime",        100,               TYPE_STRING,   0, 0 }
        };

        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

    if (index == StoreRecord_INDEX)
    {
        INPUT_MESSAGE msg = { "StoreRecord",3,TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"DevType",        sizeof(int),       TYPE_INT,      0, 0 },
            {"ID",             sizeof(int),       TYPE_INT,      0, 0 },
            {"Record",         sizeof(Record),TYPE_STRUCT, 8, Record_INDEX }
        };
            
        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    }

    if (index == test_xml_INDEX)
    {
        INPUT_MESSAGE msg = { "test_xml",3,TEST_MODE };
        FIELD_TYPE fdarray[] =
        {
            {"ID",             sizeof(int),       TYPE_INT,          3, 0 },
            {"StoreRecord",    sizeof(StoreRecord),TYPE_STRUCT,3, StoreRecord_INDEX },
            {"StoreRecord2",       sizeof(StoreRecord2),TYPE_STRUCT,3, StoreRecord_INDEX },
        };

        return malloc_struct(&msg, (char *)out_inputmsg, fdarray);
    };
    return NULL;
}

/************************************************************************
* ������: find_fdarray
* ��  ��: sx
* ��  ��: 1.����������Ϣ,���ȴ���ǰ����Ϣ�в���,���û���ҵ�,�͵���ָ��fun������,
*           ���ҳ��µĽṹ��������Ϣ,�õ�����������
*         2.�ڲ�����.
* �������: PARSER_FUN_PARAM *pf,���ݼ���
*           int index,����
*            __find_sub_strcut_desc fun,ָ����fun����
* �������: ��
* ����ֵ: ����һ���ṹ��������Ϣ(һ�����).
************************************************************************/
FDARRAY_PTR * find_fdarray( PARSER_FUN_PARAM *pf, int index, __find_sub_strcut_desc fun )
{
    FDARRAY_PTR *next;
    FDARRAY_PTR *end_next;
    
    next = pf->fdarrayptr;
    end_next = next;
    while( next != NULL )
    {
        end_next = next;
        if ( next->index == index )
        {
            return next;
        }
        next = end_next->next;
    }
    
    // �ø��򵥵ĵ�������
    next = malloc( sizeof(FDARRAY_PTR) );
    if ( next == NULL )
        return NULL;
    memset( next,0,sizeof(FDARRAY_PTR) );
    // û���ҵ�
    next->fdarray = (FIELD_TYPE *)fun(index,&next->inputmsg);
    next->index = index;
    if ( pf->fdarrayptr == NULL )
    {
        pf->fdarrayptr = next;
    }
    else
    {
        end_next->next = next;
    }
    return next;
}

/************************************************************************
* ������: find_xml_value
* ��  ��: sx
* ��  ��: 1.���ҵ�һ���ؼ��ʼ��ؼ��ʵ�ֵ��,���ݽṹ��������Ϣ,ȷ����Ӧ��
*           ֵ�������ṹ���е�ƫ�Ƶ�ַ.��ֵ
*         2.�ڲ�����.
* �������: void *data,���ݼ���,�ڲ�������������,Ҳ���������
*           int index,�ڽṹ��������Ϣ�е�����(�����±�)
*           const char *el,�ؼ��ʵ�ֵ
* �������: ��
* ����ֵ: 0��ʾ�ɹ�. >0��ʾ������
************************************************************************/
int find_xml_value(void *data, int index, const char *el)
{
    PARSER_FUN_PARAM *pf;
    int n;
    int i;
    int sum;

    pf = (PARSER_FUN_PARAM *)data;

    sum = pf->current->offset;
    for ( i = 0; i < index; i ++ )
    {
        n = pf->current->fdarray[i].repeat > 1 ? pf->current->fdarray[i].repeat : 1;
        sum += n * pf->current->fdarray[i].size;
    }

    if ( pf->current->fdarray[index].type != TYPE_STRUCT && pf->current->fdarray[index].repeat > 1 )
    {
        // ���ڷǽṹ���������˵,��Ϊû�н����Ϣ,�����ڴ�ȡ��,
        // ������ĵ�ǰά��,���浽 pf->current->fdarray[index].only_struct_index��,
        // ��Ϊ����int[10]���ַǽṹ��,�ǲ�ʹ��only_struct_index������.
        sum += pf->current->fdarray[index].only_struct_index * pf->current->fdarray[index].size;    // ����λ��ƫ��
        pf->current->fdarray[index].only_struct_index ++;

        // ͬʱ,��������ά������
        n = pf->current->fdarray[index].repeat > 1 ? pf->current->fdarray[index].repeat : 1;
        if (pf->current->fdarray[index].only_struct_index > n )
        {
            // ʧ��,ά������
            pf->ret_err |= XML_ERR_ARRAY_LESS;
            return pf->ret_err;
        }
    }

    if( TYPE_INT == pf->current->fdarray[index].type )
    {
        // ��Ϊ�˴��п�����bool��,�������ų�
        n = strlen( el );
        if ( n == 0 || el[0] == 'f' || el[0] == 'F' )
        {
            n = 0;
        }
        else if ( el[0] == 't' || el[0] == 'T' )
        {
            n = 1;
        }
        else
        {
            n = atoi( el );
        }
        memcpy( pf->outbuf + sum, &n, sizeof(int) );
    }
    else if ( TYPE_STRING == pf->current->fdarray[index].type )
    {
        n = strlen( el );
        memcpy( pf->outbuf + sum, el, n );
        pf->outbuf[sum +n] = '\0';
    }
    else if ( TYPE_BOOL == pf->current->fdarray[index].type )
    {
        /* bool Ӧ�ö����� int ��ʾ�� */
        n = strlen( el );
        if ( n > 0 && (el[0] == 't' || el[0] == 'T') ) {
            n = 1;
        }
        else {
            n = 0;
        }
        memcpy( pf->outbuf + sum, &n, sizeof(int) );
    }
    else if ( TYPE_CHAR == pf->current->fdarray[index].type )
    {
        n = atoi( el );
        memcpy( pf->outbuf + sum, &n, sizeof(int) );
    }
    else if ( TYPE_IP_ADDR == pf->current->fdarray[index].type )
    {
        struct in_addr addr;
        addr.s_addr = 0;
        n = inet_aton(el, &addr);
        memcpy( pf->outbuf + sum, &addr.s_addr, sizeof(unsigned long int) );
    }
    else if ( TYPE_TIME_LDT == pf->current->fdarray[index].type )
    {
         // ������,��ûŪ���linux����ôӦ�ü������.linux c��date��ô?
    }
    
    return 0;
}

/************************************************************************
* ������: start_xml
* ��  ��: sx
* ��  ��: 1.����XML����ĺ���,���ڴ����ҵ���ǩͷ������ֵ
*         2.�ڲ�����.
*         3.ʵ��֧��ȫ���ṹ��Ƕ��,������Ƕ��ֻ����ĳһֵ�ļ��׷�ʽ
* �������: void *data,���ݼ���,�ڲ�������������,Ҳ���������
*           const char *el,��ǩͷ����
*           const char **attr,��ǩ������,�ֱ���(�ؼ���,�ؼ��ʵ�ֵ)(,)
* �������: ��
* ����ֵ: ��
************************************************************************/
static void XMLCALL start_xml(void *data, const char *el, const char **attr)
{
    int i;
    int j;
    int k;
    int sum;

    PARSER_FUN_PARAM *pf;
    pf = (PARSER_FUN_PARAM *)data;

    // ����ģʽ
    if (pf->easymode)
    {
        if (pf->easymode_finded == 2)
        {
            return;
        }
        if ( strcmp(el, pf->easymode_findname)==0 )
        {
            pf->easymode_finded = 1;
            return;
        }
        /* �������� attr[] */
        for (i = 0; attr[i]; i += 2)
        {
            if ( strcmp(attr[i], pf->easymode_findname) == 0 )
            {
                strcpy( pf->outbuf, attr[i+1] );                    // ��ֵ,Ȼ�������
                pf->easymode_finded = 2;
                return;
            }
        }
        return;
    }

    /* ���� *el */
    pf->current->in_fdarray_index = -1;
    pf->current->getted_data = 0;
    for (i = 0; i < pf->current->fdcount; i ++)
    {
        if ( strcmp(el, pf->current->fdarray[i].name) == 0 )
        {
            pf->current->in_fdarray_index = i;      // ��ǰ�ҵ���λ��
            break;
        }
    }
    if ( pf->current->in_fdarray_index < 0)         // �����Ǳ���ṹ�������
    {
        /* �������� attr[] */
        for (i = 0; attr[i]; i += 2)
        {
            for (j = 0; j < pf->current->fdcount; j ++)
            {
                if ( strcmp(attr[i], pf->current->fdarray[j].name) == 0 )
                {
                    find_xml_value( data, j, attr[i+1] );
                    break;
                }
            }
        }
        return;
    }
    // ����һ: start��struct,��һ,end��struct,��һ
    // �����: start��struct,�����Ƿ��Ѵ���,����,����׷��
    if ( pf->current->fdarray[ pf->current->in_fdarray_index ].type == TYPE_STRUCT )        // ��ǰλ����һ�����
    {
        // ����һ���µ��ӽ��,������ǰ����赽���½����
        SAVE_XML_PARSE_DATA *sub;
        SAVE_XML_PARSE_DATA *next;
        SAVE_XML_PARSE_DATA *search_end;
        FDARRAY_PTR *fdarrayptr;
        int sub_fdcount;
        
        sub_fdcount = 0;
        // ���Ҹý���������Ϣ
        fdarrayptr = find_fdarray( pf, pf->current->fdarray[ pf->current->in_fdarray_index ].only_struct_index, pf->fun );
        if (fdarrayptr != NULL)
        {
            sub_fdcount = fdarrayptr->inputmsg.input_fdarray_count;
        }

        if (fdarrayptr == NULL || fdarrayptr->fdarray == NULL )
        {
            // ���û���ҵ�.��endҲ������
            pf->current->in_fdarray_index = -1;
            pf->ret_err |= XML_ERR_NO_FIND_DESC;
            return;
        }

        // ����һ��,ǰ���Ƿ����д˽ṹ��
        sub = NULL;
        next = pf->current->subfirst;
        search_end = next;
        while (next != NULL)
        {
            search_end = next;
            if ( strcmp(next->start_name, el) == 0 )
            {
                sub = next;
            }
            next = next->next;
        }

        if (sub)
        {
            SAVE_XML_PARSE_DATA *prev;

            if ( sub->first_prev->loop_current_count >= pf->current->fdarray[ pf->current->in_fdarray_index ].repeat )
            {
                pf->current->in_fdarray_index = -1;
                pf->ret_err |= XML_ERR_ARRAY_LESS;
                return;
            }

            // ���ҵ�ͬ������һ��
            prev = sub;
            sub = malloc( sizeof(SAVE_XML_PARSE_DATA) );
            if (!sub)
            {
                pf->current->in_fdarray_index = -1;
                pf->ret_err |= XML_ERR_MEMORY;
                return;
            }
            memset(sub,0,sizeof(SAVE_XML_PARSE_DATA));
            strcpy(sub->start_name, el);

            sub->parent = prev->parent;          // ���ӹ�ϵ
            prev->next = sub;                          // �ֵܹ�ϵ��Ӧ
            sub->first_prev = prev->first_prev; // ��һ��
            sub->first_prev->end_next = sub;   // ��¼��ĩһ��

            sub->in_parent_fdarray_index = pf->current->in_fdarray_index;
            sub->fdarray = fdarrayptr->fdarray;
            sub->fdcount = sub_fdcount;

            // ������һ��,����һЩ���ݱ���: ƫ��,������
            sub->loop_index = prev->loop_index +1;  // ������һ��
            sub->first_prev->loop_current_count = sub->loop_index;  // ��¼��ǰ����
            sub->offset = prev->offset + pf->current->fdarray[ pf->current->in_fdarray_index ].size;    // ��ǰһ��������,��ǰ��һ��struct����

            // ��һ
            pf->current = sub;

            pf->allsave_end->allsave_next = sub;
            pf->allsave_end = sub;
        }
        else
        {
            // ֮ǰû������������ṹ��
            sub = malloc( sizeof(SAVE_XML_PARSE_DATA) );
            if (!sub)
            {
                pf->current->in_fdarray_index = -1;
                pf->ret_err |= XML_ERR_MEMORY;
                return;
            }
            // ���������ϵ
            memset(sub,0,sizeof(SAVE_XML_PARSE_DATA));
            strcpy(sub->start_name, el);

            if (search_end != NULL)
            {
                search_end->next = sub;         // �����ֵܹ�ϵȷ��
                sub->first_prev = search_end->first_prev;
            }
            else
            {
                pf->current->subfirst = sub;    // ���ӹ�ϵ��Ӧ
                sub->first_prev = sub;          // ��һ��
            }
            sub->parent = pf->current;
            sub->first_prev->end_next = sub;            // ��¼��ĩһ��

            sub->in_parent_fdarray_index = pf->current->in_fdarray_index;
            sub->fdarray = fdarrayptr->fdarray;
            sub->fdcount = sub_fdcount;

            // ����ϵ����. �ټ��㵱ǰ�����outbuf�е�ƫ�Ƶ�ַ
            sub->loop_index = 1;            // ���ǵ�һ��
            sub->first_prev->loop_current_count = sub->loop_index;  // ��¼��ǰ����
            
            // ���������struct���ڴ�λ��,��Ϊ�������еĵ�һ��,���� offset = parent.end.offset + pf->current->in_fdarray_index֮���
            sum = 0;
            for ( j = 0; j < pf->current->in_fdarray_index; j++ )
            {
                k = pf->current->fdarray[j].repeat > 1 ? pf->current->fdarray[j].repeat : 1;
                sum += pf->current->fdarray[j].size * k;
            }
            sub->offset = pf->current->offset + sum;

            // ��һ
            pf->current = sub;

            if ( pf->allsave == NULL )
                pf->allsave = sub;
            else
                pf->allsave_end->allsave_next = sub;
            pf->allsave_end = sub;
        }
    }
    /* �������� attr[] */
    for (i = 0; attr[i]; i += 2)
    {
        for (j = 0; j < pf->current->fdcount; j ++)
        {
            if ( strcmp(attr[i], pf->current->fdarray[j].name) == 0 )
            {
                find_xml_value( data, j, attr[i+1] );
                break;
            }
        }
    }
}

/************************************************************************
* ������: end_xml
* ��  ��: sx
* ��  ��: 1.����XML����ĺ���,���ڴ����ҵ���ǩβ
*         2.�ڲ�����.
*         3.��Ҫ����Ƕ�׹�ϵ
* �������: void *data,���ݼ���,�ڲ�������������,Ҳ���������
*           const char *el,��ǩβ����
* �������: ��
* ����ֵ: ��
************************************************************************/
static void XMLCALL end_xml(void *data, const char *el)
{
    PARSER_FUN_PARAM *pf;
    pf = (PARSER_FUN_PARAM *)data;

    if ( pf->easymode > 0 || pf->current == NULL)       // ����ģʽ��,���������������
    {
        return;
    }
    // �ϴ�start����û���ҵ������ǩ.��ʱҲ�Ͳ�����
    if ( pf->current->in_fdarray_index < 0 )
    {
        return;
    }

    if ( pf->current->parent != NULL && pf->current->parent->fdarray[ pf->current->in_parent_fdarray_index].type == TYPE_STRUCT )       // ��ǰλ����һ�����
    {
        // ����һ: start��struct,��һ,end��struct,��һ
        pf->current = pf->current->parent;
    }
}

/************************************************************************
* ������: chardatahandle
* ��  ��: sx
* ��  ��: 1.����XML����ĺ���,���ڴ����ҵ��ı�ǩ�м������.���ؼ��ʵ�ֵ
*         2.�ڲ�����.
* �������: void *data,���ݼ���,�ڲ�������������,Ҳ���������
*           const char *el,ֵ,û��\0��β
*           int len,ֵ�ĳ���,�����ж�el����Чֵ����
* �������: ��
* ����ֵ: ��
************************************************************************/
static void XMLCALL chardatahandle(void *data, const char *el, int len)
{
    char *buf;
    PARSER_FUN_PARAM *pf;
    pf = (PARSER_FUN_PARAM *)data;

    // ǰ���start��ǩ,��û���ڵ�ǰ����������Ϣ���ҵ�
    // ����<A struct>�ո�</A> ��,����Ŀո����׽��ṹ��ĵ�һ���������.���Դ��ж���������.
	if ( (pf->easymode && pf->easymode_finded != 1) 
		|| (pf->easymode == 0 && pf->current->in_fdarray_index < 0)
		|| (pf->easymode == 0 && pf->current->parent != NULL && pf->current->parent->fdarray[ pf->current->in_parent_fdarray_index].type == TYPE_STRUCT ) )
	{
		return;
	}

    
    // ����һ�����⣬��<A>1</A>    <B>2</B>�е�</A>ִ�����
    // �ֿ��ܳ���һȺ�ո��ֵ,��</A>   <B>֮��Ŀո�, ��ô���أ�
    if ( !pf->easymode && pf->current->getted_data == 1 )        // ����Ѿ�����ֵ
    {
        return;
    }
    buf = malloc(len +2);
    if (!buf)
    {
        return;
    }

    memcpy( buf, el, len );
    buf[ len ] = '\0';
    if ( pf->easymode )
    {
        // ����ģʽר��
        strcpy( pf->outbuf, buf );
        pf->easymode_finded = 2;            // ֻ�ҵ�һ��
    }
    else
    {
        find_xml_value(data, pf->current->in_fdarray_index, buf);
        pf->current->getted_data = 1;          // ����ֵ�󣬾ͱ����´��ظ���������Ŀո�ֵ
    }
    free(buf);
}

/************************************************************************
* ������: XMLEncodingHandle
* ��  ��: sx
* ��  ��: 1.����XML����ĺ���,���ڴ�����ʶ��ı��룬����GBK��GB2312��BIG5�ȵ�
*         2.�ڲ�����.
* �������: void *data,���ݼ���,�ڲ�������������,Ҳ���������,�ڴ˴��ò���
*           const char *name, ��������,��"GBK"
*           XML_Encoding *info, һ��ָ���������ת���Ľṹ
* �������: ��
* ����ֵ: ��
************************************************************************/
static int XMLCALL XMLEncodingHandle(void *data, const char *name, XML_Encoding *info)
{
   //  map[i]  ֵ����,
   //      >=0 ��ʾ��Ӧ��ֵ�ǵ��ַ�
   //      -1  ��ʾ�Ƿ��ַ�
   //      -2,-3,-4    ��ʾ�Ƕ��ֽ��ַ��ĵ�һ��,�ֱ���2�ֽڡ�3�ֽڡ�4�ֽڵ��ַ�
   //  ʹ��:
   //      expat�����map[(unsigned char)s[i]] ��ֵ�ж�ĳ���ַ���������ٸ��ֽڣ�Ȼ��������Ӧ����Ŀ
   //  
   //  info->data    �������ݣ����ֻ��ش���convert��release,expat�ڲ������д���,
   //  expat���������л����convert,����������release(���������ֵ��ΪNULL�Ļ�)
   //  ����ǵ��ֽ��ַ�������convert����ΪNULL,
   //  ����Ƕ��ֽڰ�����,convertӦ����ֵ,����Ѷ�Ӧ������ת����ucs2����
    int   i;
    if( name != NULL && ( strcasecmp(name,"GBK") == 0 || strcasecmp(name,"GB2312") == 0 ) )
    {
        for(i=0;i<128;i++)
            info->map[i]   =   i;
        for(;i<256;i++)
                info->map[i]   =   -2;
        
        // ��Ȼ��ΪЭ��v3.0����������Ϣû�к��֣���ô��Ҳ����͵���������ˡ�
        info->convert       =   NULL;       // GBK_convert
        info->release       =   NULL;
        return   1;//XML_STATUS_OK;
    }

    return   XML_STATUS_ERROR;

}

//int GBK_convert(void *data, const char *s)
//{
//    return 0;
//}

/************************************************************************
* ������: free_pf
* ��  ��: sx
* ��  ��: 1.�����ݼ�������malloc������Ƕ�׽������free
*         2.�ڲ�����.
* �������: PARSER_FUN_PARAM *pf,���ݼ���
* �������: ��
* ����ֵ: ��
************************************************************************/
void free_pf(PARSER_FUN_PARAM *pf)
{
    SAVE_XML_PARSE_DATA * allsave_next;
    SAVE_XML_PARSE_DATA * current;

    if ( pf == NULL || pf->allsave == NULL)
    {
        return;
    }

    allsave_next = pf->allsave;
    current = allsave_next;
    while( current != NULL )
    {
        allsave_next = current->allsave_next;
        free( current );
        current = allsave_next;
    }
}

/************************************************************************
* ������: free_fdarray
* ��  ��: sx
* ��  ��: 1.�����ݼ�������malloc�����Ľṹ��������Ϣfree
*         2.�ڲ�����.
* �������: PARSER_FUN_PARAM *pf,���ݼ���
* �������: ��
* ����ֵ: ��
************************************************************************/
void free_fdarray(PARSER_FUN_PARAM *pf)
{
    FDARRAY_PTR * fdarrayptr;
    FDARRAY_PTR * current;

    if( pf == NULL || pf->fdarrayptr == NULL )
    {
        return;
    }

    fdarrayptr = pf->fdarrayptr;
    current = fdarrayptr;
    while ( current != NULL )
    {
        fdarrayptr = current->next;
        if ( current->fdarray != NULL )
        {
            free(current->fdarray);
        }
        free(current);
        current = fdarrayptr;
    }
}

/************************************************************************
* ������: vs_xml_parser
* ��  ��: sx
* ��  ��: ���Ժ���
* �������: ��
* �������: ��
* ����ֵ: ��
************************************************************************/
//#ifdef AMIGA_SHARED_LIB
//#include <proto/expat.h>
//int
//amiga_main(int argc, char *argv[])
//#else
int vs_xml_parser()
//main()
//#endif
{
    char *Buff2;
    test_xml ptest;
    int ret_err;
    int n;
    char ret_string[300];
    memset( &ptest, 0, sizeof( test_xml ) );      // ������ʼ��0

    ret_err = sizeof(char) + sizeof(int);

    Buff2 = malloc( BUFFSIZE );
    memset( Buff2, 0, BUFFSIZE );


    //printf("start------------------\n");

    // ��ȷ��ʽ
    strcpy( Buff2 ,"<Message>"
        "<ID>10001</ID>"
        "<ID>10002</ID>"
        "<ID>10003</ID>"

        "<Record BeginTime = \"EXIT-MM-DD HH:MM:SS\" EndTime = \"EXIT-MM-DD HH:MM:SS\" />"

        "<StoreRecord DevType=\"100000\" ID=\"1111\" >"
        "<Record BeginTime = \"1-MM-DD HH:MM:SS\" EndTime = \"11-MM-DD HH:MM:SS\" />"
        "<Record BeginTime = \"2-MM-DD HH:MM:SS\" EndTime = \"22-MM-DD HH:MM:SS\" />"
        "</StoreRecord>"

        "<StoreRecord2 DevType=\"999999\" ID=\"999\" >" 
        "<Record BeginTime = \"2___0-MM-DD HH:MM:SS\" EndTime = \"2___11-MM-DD HH:MM:SS\" />"
        "<Record BeginTime = \"2___0-MM-DD HH:MM:SS\" EndTime = \"2___22-MM-DD HH:MM:SS\" />"
        "</StoreRecord2>"

        "<StoreRecord DevType=\"200000\" ID=\"2222\" >"
        "<Record BeginTime = \"3-MM-DD HH:MM:SS\" EndTime = \"33-MM-DD HH:MM:SS\" />"
        "<Record BeginTime = \"4-MM-DD HH:MM:SS\" EndTime = \"44-MM-DD HH:MM:SS\" />"
        "<Record BeginTime = \"5-MM-DD HH:MM:SS\" EndTime = \"55-MM-DD HH:MM:SS\" />"
        "</StoreRecord>"

        "</Message>");

    //printf( "\r\nԭʼ��xml�ַ���Ϊ:\r\n%s\r\n", Buff2 );

    //printf("\r\ndo 1------------------xml2struct\r\n");
    ret_err = xml2struct( Buff2, strlen(Buff2),
        (char *)&ptest, test_xml_INDEX, find_sub_strcut_desc );

    //printf("��һ��storerecord��record[0]��BeginTime Ӧ����1-MM-DD HH:MM:SS, ʵ���أ�\r\n%s\r\n", ptest.storerecord[0].record[0].BeginTime);
    //printf("��һ��storerecord��record[2]��BeginTime Ӧ�ÿ�, ʵ���أ�\r\n%s\r\n", ptest.storerecord[0].record[2].BeginTime);
    //printf("�ڶ���storerecord��record[1]��BeginTime Ӧ����4-MM-DD HH:MM:SS, ʵ���أ�\r\n%s\r\n", ptest.storerecord[1].record[1].BeginTime);

    //printf("\r\ndo 2------------------findxml_string\r\n");
    ret_err = findxml_string( Buff2, strlen(Buff2), "DevType", ret_string );
    //printf( "���ҵ���(string) DevType=%s\r\n", ret_string );

    //printf("\r\ndo 3------------------findxml_int\r\n");
    ret_err = findxml_int( Buff2, strlen(Buff2), "DevType", &n );
    //printf( "���ҵ���(int) DevType=%d\r\n", n );

    //printf("\r\ndo 4------------------struct2xml\r\n");
    memset( Buff2,0,BUFFSIZE );
    ret_err = struct2xml( (char*)&ptest, Buff2, test_xml_INDEX, find_sub_strcut_desc );
    n = strlen(Buff2);

    //printf( "��������struct���ݣ����ɵ�xmlΪ��\r\n%s\r\n", Buff2 );
    //printf("--------- �������賤��Ϊ return len = %d,   ʵ�����ɵ�xml����Ϊ strlen( new_xml_buf ) = %d \r\n", ret_err, n );

    free( Buff2 );


    //printf("end------------------\n");
    return 0;
}

/************************************************************************
* ������: my_xml_parse
* ��  ��: sx
* ��  ��: 1.��װ��expat�Ľ�������,��xml2struct�ڲ�����
*         2.�ڲ�����
* �������: PARSER_FUN_PARAM *pf,���ݼ���
*           char *xml_string,����˼��
* �������: ��
* ����ֵ: 0��ʾ�ɹ�,>0��ʾ������
************************************************************************/
int my_xml_parse(PARSER_FUN_PARAM *pf, char *xml_string, int xml_len)
{
    XML_Parser p;

    p = XML_ParserCreate( NULL );
    if (!p)
    {
        fprintf( stderr, "�޷�����XML������\n" );
        return -1;
    }

    /* �������趨׼����ʼ�ͽ���ǩ�� */
    XML_SetElementHandler( p, start_xml, end_xml );

    /* ����Ԫ�����ı��Ĵ����� */
    XML_SetCharacterDataHandler( p, chardatahandle );

    /* һ���Խ����� */
    //xml_len = strlen( xml_string );

    /* ����һ����������ݽṹ������������ʹ�� */
    XML_SetUserData( p, pf );

    /* ��GBK,GB2312�Ȳ�ʶ��ı��룬���˵�����ΪĿǰ����Ϊ����������Ӣ�� */
    XML_SetUnknownEncodingHandler( p, XMLEncodingHandle, 0 );
    if (XML_Parse( p, xml_string, xml_len, 1 ) == XML_STATUS_ERROR)
    {
#if 0
        fprintf(stderr, "Parse error at line %" XML_FMT_INT_MOD "u:\n%s\n",
            XML_GetCurrentLineNumber( p ),
            XML_ErrorString(XML_GetErrorCode( p ) ) );
        /* �ͷű�������ʹ�õ��ڴ档*/
#endif
        XML_ParserFree( p );
        return -1;
    }
    /* �ͷű�������ʹ�õ��ڴ档*/
    XML_ParserFree( p );
    return pf->ret_err;
}

/************************************************************************
* ������: xml2struct
* ��  ��: sx
* ��  ��: ��xml��������struct�е�����
* �������: char *xml_string,һ��XML���
*           char *outbuf,��󷵻صĽṹ���ڲ��ռ�
*           int struct_index,���ṹ�������������е�����
*           __find_sub_strcut_desc fun,�Զ���Ľṹ����������
* �������: ��
* ����ֵ: 0��ʾ�ɹ�,>0��ʾ������
************************************************************************/
int xml2struct( char *xml_string, int xml_len, char *outbuf, int struct_index, __find_sub_strcut_desc fun )
{
    int fdcount;
    int ret_err;
    PARSER_FUN_PARAM pf;
    SAVE_XML_PARSE_DATA savept;

    FDARRAY_PTR * fdarrayptr;

    if ( xml_string == NULL || outbuf == NULL || fun == NULL )
        return -1;

    memset( &pf, 0, sizeof(PARSER_FUN_PARAM) );
    memset( &savept, 0, sizeof(SAVE_XML_PARSE_DATA) );

    pf.fun = (void *)fun;                           // ��ס���ú���
    fdarrayptr = find_fdarray( &pf, struct_index, fun );
    if ( fdarrayptr != NULL && fdarrayptr->fdarray != NULL )
        fdcount = fdarrayptr->inputmsg.input_fdarray_count;
    else
    {
        free_pf( &pf );                       // ��find_fdarray()���������ڴ�,����Ҫfree
        free_fdarray( &pf );
        return XML_ERR_NO_FIND_DESC;        // δ�ҵ��ṹ�嶨��
    }

    savept.offset = 0;
    savept.fdarray = fdarrayptr->fdarray;
    savept.fdcount = fdcount;
    savept.in_fdarray_index = -1;
    pf.outbuf = outbuf;                        // ��󷵻ص��˴�
    pf.head = &savept;
    pf.current = pf.head;

    ret_err = my_xml_parse( &pf, xml_string, xml_len );         // ����expat,��xml������pf��

    free_pf( &pf );
    free_fdarray( &pf );

    return ret_err;
}

/************************************************************************
* ������: findxml_int
* ��  ��: sx
* ��  ��: ��xml��������struct�е�����
* �������: char *xml_string,һ��XML���
*           find_str,��Ҫ�����Ĺؼ���
* �������: int *out,���صĹؼ��ʵ�ֵ
* ����ֵ: 0��ʾ�ɹ�,>0��ʾ������
************************************************************************/
int findxml_int( char *xml_string, int xml_len, char *find_str, int *out )
{
    char outbuf[50];
    int ret_err;

    ret_err = findxml_string( xml_string, xml_len, find_str, outbuf );
    if (ret_err == 0)
        *out = atoi(outbuf);
    else
        *out = -1;

    return ret_err;
}

/************************************************************************
* ������: findxml_string
* ��  ��: sx
* ��  ��: ��xml��������struct�е�����
* �������: char *xml_string,һ��XML���
*           find_str,��Ҫ�����Ĺؼ���
* �������: char *outbuf,��󷵻ص��ַ���
            ��: xml����:  BeginTime = "YYYY-MM-DD HH:MM:SS"
                find_str: BeginTime
            ��: ����ֵ 0�� outbuf�д��YYYY-MM-DD HH:MM:SS
* ����ֵ: 0��ʾ�ɹ�,>0��ʾ������
************************************************************************/
int findxml_string( char *xml_string, int xml_len, char *find_str, char *outbuf )
{
    PARSER_FUN_PARAM pf;

    memset( &pf, 0, sizeof(PARSER_FUN_PARAM) );
    pf.easymode = 1;
    pf.outbuf = outbuf;
    strcpy( pf.easymode_findname, find_str );
    return my_xml_parse(&pf, xml_string, xml_len);            // ����expat,��xml������pf��
}

/************************************************************************
* ������: makexmlstring
* ��  ��: sx
* ��  ��: ��structת����XML���,�˺������ڵݹ�,ʵ��Ƕ��struct
* �������: PARSER_FUN_PARAM *pf,���ݼ���
*           char *pstruct,�ýṹ���ָ��
*           int struct_index,�ýṹ�������
*           __find_sub_strcut_desc fun, ָ���Ĳ��ҽṹ��������Ϣ֮������
* �������: char *outbuf,��󷵻ص�xml�ַ���
* ����ֵ: �Ƿ����������ַ���
************************************************************************/
int makexmlstring( PARSER_FUN_PARAM *pf, char *pstruct, char *outbuf,
                  int struct_index, int offset )
{
    int i;
    int j;
    int fdcount;
    int input_type;
    int in_offset;

    // 0, ��δ����, ��: <A
    // 1, ������һ��, �� <A>
    // 2, ȫ������, �� <A />, <A></A>
    int closed;
    int loop;
    int attribshow;
    int itemp;
    char *ptemp;
    char cc;

    int after_closed_have;

    FDARRAY_PTR * fdarrayptr;
    char ctemp[256];
    struct in_addr ip_temp;
    int outbuf_len;
    int addnewinfo;
    int old_write_buf_len;

    old_write_buf_len = pf->write_buf_len;
    outbuf_len = 0;
    if (outbuf)
        outbuf_len = strlen(outbuf);    // ��¼һ�µ�ǰ����,���û����������,��Ѷ�����Ч������ɾ��
    addnewinfo = 0;

    fdarrayptr = find_fdarray( pf, struct_index, pf->fun );
    if ( fdarrayptr == NULL && fdarrayptr->fdarray == NULL )
    {
        return 0;
    }
    fdcount = fdarrayptr->inputmsg.input_fdarray_count;
    input_type = fdarrayptr->inputmsg.input_type;

    closed = 0;
    after_closed_have = 0;
    if (outbuf)
    {
        sprintf( ctemp, "<%s", fdarrayptr->inputmsg.name );
        strcat( outbuf, ctemp );
    }
    pf->write_buf_len = pf->write_buf_len + 1 + strlen(fdarrayptr->inputmsg.name);

    in_offset = 0;
    for ( i = 0; i < fdcount; i ++ )
    {
        // �ṹ��,���еݹ�
        if ( fdarrayptr->fdarray[i].type == TYPE_STRUCT )
        {
            if ( closed == 0 )
            {
                if (outbuf)
                {
                    strcat( outbuf, ">\r\n" );
                }
                closed = 1;
                pf->write_buf_len += 3;
            }

            loop = fdarrayptr->fdarray[i].repeat > 1 ? fdarrayptr->fdarray[i].repeat : 1;
            for ( j = 0; j < loop; j ++ )
            {
                itemp = makexmlstring(pf, pstruct, outbuf,
                    fdarrayptr->fdarray[i].only_struct_index,
                    offset + in_offset + j * fdarrayptr->fdarray[i].size);
                addnewinfo += itemp;
            }
        }
        else
        {
            // ǿ��Ҫ�� <A> </A>��ʱ,��ǿ��Ҫ��<A>1</A> <B>2<B> �б���ʾ���Ե�
            // ���������,Ҳֻ�ܰѱ�ǩ��<A> </A>����
            if ( closed == 0
                && ((input_type & FULL_CLOSE) == FULL_CLOSE
                || (input_type & LIST_SHOW) == LIST_SHOW 
                || fdarrayptr->fdarray[i].repeat > 1 ) )
            {
                if (outbuf)
                {
                    strcat( outbuf, ">\r\n" );
                }
                pf->write_buf_len += 3;
                closed = 1;
            }

            attribshow = 0;
            // ������ڻ���close == 0 ��Ҫ��<A a=1 b=2 />����ʾ����
            if ( closed == 0 && (input_type & ATTRIB_SHOW) == ATTRIB_SHOW )
            {
                attribshow = 1;
            }

            loop = fdarrayptr->fdarray[i].repeat > 1 ? fdarrayptr->fdarray[i].repeat : 1;

            for ( j = 0; j < loop; j ++ )
            {
                switch( fdarrayptr->fdarray[i].type )
                {
                case TYPE_INT:
                    itemp = *(int *)(pstruct + offset + in_offset + j * fdarrayptr->fdarray[i].size);
                    // �����������ʾ0
                    if ( ( input_type & SHOW_INT_0 ) != SHOW_INT_0 && itemp == 0 )
                        continue;

                    if (attribshow == 0)
                    {
                        sprintf( ctemp, "<%s>%d</%s>\n", fdarrayptr->fdarray[i].name,
                            itemp, fdarrayptr->fdarray[i].name );
                    }
                    else
                    {
                        sprintf( ctemp, " %s = \"%d\" ", fdarrayptr->fdarray[i].name, itemp );
                    }
                    if(outbuf)
                    {
                        strcat( outbuf, ctemp );
                    }
                    pf->write_buf_len += strlen(ctemp);

                    addnewinfo ++;

                    break;
                case TYPE_STRING:
                    ptemp = (char *)(pstruct + offset + in_offset + j * fdarrayptr->fdarray[i].size);

                    // �����������ʾ0
                    if ( ( input_type & SHOW_INT_0 ) != SHOW_INT_0 && ptemp[0] == '\0' )
                        continue;

                    if (attribshow == 0)
                    {
                        sprintf( ctemp, "<%s>%s</%s>\n", fdarrayptr->fdarray[i].name,
                            ptemp, fdarrayptr->fdarray[i].name );
                    }
                    else
                    {
                        sprintf( ctemp, " %s = \"%s\" ", fdarrayptr->fdarray[i].name, ptemp );
                    }
                    if (outbuf)
                    {
                        strcat( outbuf, ctemp );
                    }
                    pf->write_buf_len += strlen(ctemp);

                    addnewinfo ++;
                    break;

                case TYPE_BOOL:
                    itemp = *(int *)(pstruct + offset + in_offset + j * fdarrayptr->fdarray[i].size);
                    if (attribshow == 0)
                    {
                        if (itemp)
                        {
                            sprintf( ctemp, "<%s>true</%s>\n", fdarrayptr->fdarray[i].name,
                                fdarrayptr->fdarray[i].name );
                        }
                        else
                        {
                            sprintf( ctemp, "<%s>false</%s>\n", fdarrayptr->fdarray[i].name,
                                fdarrayptr->fdarray[i].name );
                        }
                    }
                    else
                    {
                        if (itemp)
                        {
                            sprintf( ctemp, " %s = \"true\" ", fdarrayptr->fdarray[i].name );
                        }
                        else
                        {
                            sprintf( ctemp, " %s = \"false\" ", fdarrayptr->fdarray[i].name );
                        }
                    }
                    if (outbuf)
                    {
                        strcat( outbuf, ctemp );
                    }
                    pf->write_buf_len += strlen(ctemp);

                    addnewinfo ++;

                    break;

                case TYPE_CHAR:
                    itemp = *(int *)(pstruct + offset + in_offset + j * fdarrayptr->fdarray[i].size);
                    cc = (char)itemp;
                    if (attribshow == 0)
                    {
                        sprintf( ctemp, "<%s>%c</%s>\n", fdarrayptr->fdarray[i].name,
                            cc, fdarrayptr->fdarray[i].name );
                    }
                    else
                    {
                        sprintf( ctemp, " %s = \"%c\" ", fdarrayptr->fdarray[i].name, cc );
                    }

                    if (outbuf)
                    {
                        strcat( outbuf, ctemp );
                    }
                    pf->write_buf_len += strlen(ctemp);

                    addnewinfo ++;
                    break;
              case TYPE_IP_ADDR:
                    ip_temp.s_addr = *(unsigned long int *)(pstruct + offset + in_offset + j * fdarrayptr->fdarray[i].size);

                    if (attribshow == 0)
                    {
                        sprintf( ctemp, "<%s>%s</%s>\n", fdarrayptr->fdarray[i].name,
                            inet_ntoa(ip_temp), fdarrayptr->fdarray[i].name );
                    }
                    else
                    {
                        sprintf( ctemp, " %s = \"%s\" ", fdarrayptr->fdarray[i].name, inet_ntoa(ip_temp) );
                    }
                    if (outbuf)
                    {
                        strcat( outbuf, ctemp );
                    }
                    pf->write_buf_len += strlen(ctemp);

                    break;
                    //�����ݲ���
                }
            }
        }
        itemp = fdarrayptr->fdarray[i].repeat > 1 ? fdarrayptr->fdarray[i].repeat : 1;
        in_offset += itemp * fdarrayptr->fdarray[i].size;
    }

    if (addnewinfo == 0)        // ���û����������,��ɾ���ո��¼ӵ���Ч����
    {
        if (outbuf)
            outbuf[outbuf_len] = '\0';
        pf->write_buf_len = old_write_buf_len;
        return 0;
    }
    if ( closed == 0 )
    {
        if ( ( input_type & EASY_CLOSE ) != EASY_CLOSE )        // ����ģʽ <A .. />
        {
            if (outbuf)
            {
                strcat( outbuf, " />\r\n" );
            }
            pf->write_buf_len += 5;
        }
        else
        {
            sprintf( ctemp, "></%s>\n", fdarrayptr->inputmsg.name );
            if (outbuf)
            {
                strcat( outbuf, ctemp );
            }
            pf->write_buf_len += strlen(ctemp);
        }
        closed = 2;
    }
    if ( closed == 1 )
    {
        sprintf( ctemp, "</%s>\n", fdarrayptr->inputmsg.name );
        if (outbuf)
        {
            strcat( outbuf, ctemp );
        }
        pf->write_buf_len += strlen(ctemp);
    }
    return 1;
}


/************************************************************************
* ������: findxml_string
* ��  ��: sx
* ��  ��: ��һ���ṹ��ת����XML���
* �������: char *pstruct,�ýṹ���ָ��
*           int struct_index,�ýṹ�������
*           __find_sub_strcut_desc fun, ָ���Ĳ��ҽṹ��������Ϣ֮������
* �������: char *outbuf,��󷵻ص�xml�ַ���
*           outbuf == NULL����ʾ����ϣ����ȡxml�ַ����ĳ���
* ����ֵ: ��������xml�ַ���������Ҫ���ȡ���ʹbuf�ռ䲻���buf==NULL,������Ҳ�� >0������xml���賤��
*         ����ϣ���ڸ�����֪����Ҫ�೤ʱ�������� struct2xml(,NULL,,,)�õ�xml���ȣ��ٷ���buf����ȡ
*         -1: ����û���
************************************************************************/
int struct2xml( char *pstruct, char *outbuf,
               int struct_index, __find_sub_strcut_desc fun )
{
    int all_write_len;
    int input_type;
    PARSER_FUN_PARAM pf;
    FDARRAY_PTR * fdarrayptr;

    //FILE *fp;

    all_write_len = 0;
    if (pstruct == NULL || fun == NULL)
    {
        return -1;
    }

    memset( &pf, 0, sizeof(PARSER_FUN_PARAM) );

    pf.fun = (void *)fun;                           // ��ס���ú���
    fdarrayptr = find_fdarray( &pf, struct_index, fun );
    if ( fdarrayptr != NULL && fdarrayptr->fdarray != NULL )
    {
        input_type = fdarrayptr->inputmsg.input_type;
    }
    else
    {
        free_pf(&pf);            // ��find_fdarray()���������ڴ�,����Ҫfree
        free_fdarray(&pf);  
        return -1;               // δ�ҵ��ṹ�嶨��
    }

    pf.write_buf_len = 0;
    if ( outbuf )
    {
        outbuf[0] = '\0';            // ���ԭ������Ϣ
    }
    // ���ȴ�ӡͷ<Message>
    if ( (input_type & HAVE_TITLE) == HAVE_TITLE
        && fdarrayptr->inputmsg.name[0] != '\0' ) // �����<Message>������ͷʱ
    {
        if (outbuf)
        {
            sprintf( outbuf, "<Message>\r\n" );
        }
        pf.write_buf_len += 11;
    }

    // ���˽ṹ����Ϣ��ӡ��xml��
    makexmlstring( &pf, pstruct, outbuf, struct_index, 0 );

    // ����ӡβ</Message>
    if ( (input_type & HAVE_TITLE) == HAVE_TITLE
        && fdarrayptr->inputmsg.name[0] != '\0' ) // �����<Message>������ͷʱ
    {
        if (outbuf)
        {
            strcat( outbuf, "</Message>\r\n" );
        }
        pf.write_buf_len += 12;
    }

    //fp = fopen("c:/1.txt","w");
    //if (fp)
    //{
    //  fprintf(fp, "%s",outbuf);
    //  fclose(fp);
    //}

    all_write_len = pf.write_buf_len;

    free_pf(&pf);
    free_fdarray(&pf);
    return all_write_len;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

