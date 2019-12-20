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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>                             // for sleep()


#include "vs_def_videoProtocol_3.h"
#include "vs_video.h"
#include "vs_socket.h"
#include "vs_config.h"
#include "vs_server.h"
#include "vs_main_rountin.h"


// ��������Socket ����
int g_vs_sock[ MAX_CONNECT_TO_SERVER ];

fd_set  g_sock_fd;
int     g_fd_max;
int     g_quit_sock = 0;    // ��1 ʱ�˳�socket_main �߳�

extern pthread_t th_recv_from_con_sockfd;

char buf[MAX_SOCKET_BUF_SIZE];
char *buf_msg1, *buf_msg2;


s_socket_maindata socket_maindata;   // �ͻ������ӱ���

int listen_sock(int socket_id, int port)
{
    struct  sockaddr_in myaddr;
    int yes = 1;

    if( setsockopt( socket_id, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) ) == -1 )
    {
        printf( "\n vs_socket.c : setsockopt error! socket_id=%d\n", socket_id );
        return -1;
    }

    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = INADDR_ANY;
    myaddr.sin_port = htons( port );
    memset( &(myaddr.sin_zero), '\0', 8 );
    if( bind( socket_id, (struct sockaddr *)&myaddr, sizeof(myaddr) ) == -1 )
    {
        printf( "\n vs_socket.c : bind error! socket_id=%d\n", socket_id );
        return -1;
    }


    if( listen( socket_id, 10 ) == -1 )
    {
        printf( "\n vs_socket.c : listen error! socket_id=%d\n",  socket_id);
        return -1;
    }

    FD_SET( socket_id, &g_sock_fd );

    if(g_fd_max < socket_id)
    {
        g_fd_max = socket_id;
    }

    //printf( "\nvs_socket.c : Waitting connect with port %d and sock %d......\n", port, socket_id );
    return 0;
}


/****************************************

ϵͳ����ʱ�����˺��������߳�һֱ���С�

������ʼ��Socket ���������ݽ��ա�

Ret:  if error, return the error number, else return 0.

****************************************/

int d_savetofile_1(int len)
{
        FILE *fp;
        
        fp = fopen("msg_send.txt", "a+");
        if ( fp != NULL )
        {
            fwrite( buffer1, len, 1, fp );
            fwrite( "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 64, 1, fp );
            fclose(fp);
        }

        return 0;
}

int d_savetofile_2(int len)
{
        FILE *fp;
        
        fp = fopen("msg_send.txt", "a+");
        if ( fp != NULL )
        {
            fwrite( buffer2, len, 1, fp );
            fwrite( "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 64, 1, fp );
            fclose(fp);
        }

        return 0;
}

int vs_socket_vital_error()
{
    printf( "thread_socket_msg_main()�쳣�˳�...\n" );
    vs_thread_stop();

    return 0;
}

// ����socket������
int socket_setup(int socket_prot, int *socket_id)
{
    if(( *socket_id = socket( AF_INET, SOCK_STREAM, 0 )) == -1)
    {
        logger(TLOG_ERROR, "socket create error, socket_prot=%d", socket_prot);
        return -1;
    }
    if(listen_sock(*socket_id, socket_prot) == -1)
    {
        logger(TLOG_ERROR, "socket listen error, socket_prot=%d", socket_prot);
        return -1;
    }
    
    return 0;
}

int thread_socket_msg_main()
{
    fd_set	read_fds;
    struct  sockaddr_in remoteaddr;
    struct  timeval tv;
    int     socket_msg_id, socket_video_id;

    int     newfd;
    int     nbytes;
    int     addrlen;
    int     i;
    int     ret;
    int     len_req;                // �򻺳�������ĳ���
    int     len_4;                  // �ֽڶ��볤��

    g_fd_max = 0;
    FD_ZERO( &g_sock_fd );
    FD_ZERO( &read_fds );

    // ����socket����������������ִ�����ֹͣ���������̡߳����ʱ������Ӧ�ó���ֹͣ����RTU�������� 
    ret = socket_setup(MSGPORT, &socket_msg_id);
    if(ret != 0)
    {
        logger(TLOG_ERROR, "socket setup error, system will stop.");
        thread_stop_all();
        return -1;
    }

    ret = socket_setup(VIDEOPORT, &socket_video_id);
    if(ret != 0)
    {
        logger(TLOG_ERROR, "socket setup error, system will stop.");
        thread_stop_all();
        return -1;
    }

    while( !mapThreadID2Handler[THREAD_SOCKET].thread_quit_flag )
    {
        tv.tv_sec = 3;
        tv.tv_usec = 0;

        read_fds = g_sock_fd;
        ret = select( g_fd_max + 1, &read_fds, NULL, NULL, &tv );
        if(ret == -1)
        {
            logger(TLOG_ERROR, "socket select error, system will stop.");
            thread_stop_all();
            return -1;
        }
        else if(ret == 0)
        {
            continue;
        }
        for(i = 0; i <= g_fd_max; i++)
        {
            if(FD_ISSET(i, &read_fds))
            {
                // �Ƿ�Ϊ����SOCKET ���¼�
                if( i == socket_msg_id  || i == socket_video_id)
                {
                    addrlen = sizeof( remoteaddr );
                    if(( newfd = accept( i, ( struct sockaddr* )&remoteaddr, &addrlen )) == -1 )
                    {
                        logger(TLOG_ERROR, "socket accept error");
                    }
                    else
                    {
                        FD_SET( newfd, &g_sock_fd );
                        if( newfd > g_fd_max )
                        {
                            g_fd_max = newfd;
                        }
//                        printf( "\nvs_socket.c : Address '%s' connected with %d Socket.\n", inet_ntoa(remoteaddr.sin_addr), newfd );
                    }
                }

                // ��������
                else
                {
                    nbytes = recv( i, buf, MAX_SOCKET_BUF_SIZE, 0);
                    if( nbytes > 0 )
                    {
                        // �������γɽڵ�,���뵽����,�����ѽ����߳�
                        receiver_core(buf, nbytes);

                        // ����socket�ǼǱ�
                        g_vs_sock[PASSIVENESS_CONNECT] = i;
                    }
                    else if( nbytes == 0 )
                    {
                        close( i );
                        FD_CLR( i, &g_sock_fd );               

                        // ֪ͨĳ��socket���ٷ���ʵʱ��Ƶ
                        // ...

                        logger(TLOG_NOTICE, "Socket %d closed.\n", i);
                    }
                    else if( nbytes < 0 )
                    {
                        logger(TLOG_NOTICE, "Socket recv error, errno=%d: %s", errno, strerror(errno));
                    }
                }
            }
        }
    }
    
    logger(TLOG_NOTICE, "%s quit", __FUNCTION__);
    return 0;
}

/****************************************/

/****************************************
ϵͳ����ʱ�����˺��������߳�һֱ���С�
���̼߳�����Ƶ�˿ڣ������Ϳͻ���ͨ��
�˺�������
****************************************/
int thread_socket_msg_usr_main()
{
    s_thread_socket_maindata th_socket_maindata;
    int len_req;                // ��Ϣ�������볤��
    int len_4;                  // 4�ֽڶ��볤��

    socket_maindata.g_fd_max = 0;
    FD_ZERO( &socket_maindata.g_sock_fd );
    FD_ZERO( &th_socket_maindata.read_fds );
    
    if( ( th_socket_maindata.socket_video_id = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
    {
        printf( "\nvs_socket.c : socket error!\n" );
        return -1;
    }

    listen_sock(th_socket_maindata.socket_video_id, VIDEOPORT);


    while( !g_quit_sock )
    {
        th_socket_maindata.tv.tv_sec = 3;
        th_socket_maindata.tv.tv_usec = 0;

        th_socket_maindata.read_fds = socket_maindata.g_sock_fd;
        th_socket_maindata.ret = select( socket_maindata.g_fd_max + 1, &th_socket_maindata.read_fds, NULL, NULL, &th_socket_maindata.tv );
        if(th_socket_maindata.ret == -1)
        {
            printf( "\nvs_socket.c : ��Ƶ select error!\n" );
            return -1;
        }
        else if(th_socket_maindata.ret == 0)
        {
            continue;
        }
        for(th_socket_maindata.i = 0; th_socket_maindata.i <= socket_maindata.g_fd_max; th_socket_maindata.i++)
        {
            if(FD_ISSET(th_socket_maindata.i, &th_socket_maindata.read_fds))
            {
                // �Ƿ�Ϊ����SOCKET ���¼�
                if(  th_socket_maindata.i == th_socket_maindata.socket_video_id )
                {
                    th_socket_maindata.addrlen = sizeof( th_socket_maindata.remoteaddr );
                    if(( th_socket_maindata.newfd = accept( th_socket_maindata.i, ( struct sockaddr* )&th_socket_maindata.remoteaddr, &th_socket_maindata.addrlen )) == -1 )
                    {
                        printf( "\nvs_socket.c : ��Ƶ accept error!\n" );
                    }
                    else
                    {
                        FD_SET( th_socket_maindata.newfd, &socket_maindata.g_sock_fd );

                        if( th_socket_maindata.newfd > socket_maindata.g_fd_max )
                        {
                            socket_maindata.g_fd_max = th_socket_maindata.newfd;
                        }
                        printf( "\nvs_socket.c : Address '%s' ��Ƶ��connected with %d Socket.\n", inet_ntoa(th_socket_maindata.remoteaddr.sin_addr), th_socket_maindata.newfd );
                    }
                }

                // ��������
                else
                {
                    th_socket_maindata.nbytes = recv( th_socket_maindata.i, socket_maindata.buffer, BUF_REC_BLK_SIZE, 0);
                    printf("received %d bytes with ��Ƶ��socket %d\n", th_socket_maindata.nbytes, th_socket_maindata.i );

                    if( th_socket_maindata.nbytes > 0 )
                    {
                        // ȡ�û�������ַ
                        len_req = th_socket_maindata.nbytes + sizeof(int)*3;
                        buf_rec2_wb_get(&buf_msg2,&len_req, &len_4);  
                        *(int *)buf_msg2 = len_req;
                        *(int *)(buf_msg2 + sizeof(int)) = len_4;
                        *(int *)(buf_msg2 + sizeof(int)*2) = th_socket_maindata.i;
                        memcpy(buf_msg2+sizeof(int)*3, socket_maindata.buffer, th_socket_maindata.nbytes);
                        
                        // ֪ͨ������ʹ�õĴ�С�����黹������ʹ��Ȩ
                        buf_rec2_wb_free(len_req);

                        g_vs_sock[PASSIVENESS_CONNECT] = th_socket_maindata.i;
                    }
                    else
                    {
                        close( th_socket_maindata.i);
                        FD_CLR( th_socket_maindata.i, &socket_maindata.g_sock_fd );
                        //vs_video_desable_RTV(th_socket_maindata.i, );
                        printf( "\nvs_socket.c : ��Ƶ��Socket %d closed.\n", th_socket_maindata.i );
                    }
                }
            }
        }
    }

    printf( "\nvs_socket.c : Exit ��Ƶ��sockinit.\n" );

    return 0;
}

/****************************************

�������ӷ�����

[In]
char      *ip
int       port
type      ���������ͣ�����
{
ACCESS_SERVER     // ���������
BRODCAST_SERVER    // �ַ�������
BACKUP_SERVER      // ���ݷ�����
}

[Ret]
���󷵻ش�����룬��ȷ���� 0

****************************************/

int thread_recv_from_con(void *param)
{
    int     sockfd = (int)param;
    int     len;
    fd_set  rfds;
    struct  timeval tv;
    int     retval, maxfd = -1;
    int     len_req;                // ����Ϣ����������Ļ��峤��
    int     len_4;                  //���ֽڶ��볤��
    int     ret;

    while(1)
    {
        FD_ZERO(&rfds);
        maxfd = 0;
        FD_SET(sockfd, &rfds);
        if (sockfd > maxfd)
		{
            maxfd = sockfd;
		}

        tv.tv_sec = 1;
        tv.tv_usec = 0;

        retval = select(maxfd + 1, &rfds, NULL, NULL, &tv);
        if (retval == -1)
        {
            break;
        }
        else if (retval == 0)
        {
            continue;
        }
        else
        {
            if (FD_ISSET(sockfd, &rfds))
            {
                len = recv(sockfd, buffer1, BUF_REC_BLK_SIZE, 0);
#ifdef debug_printf_socket  
                printf("received %d bytes with socket id %d\n", len, sockfd );
#endif
                if (len > 0)
                {
                    len_req = len + sizeof(int)*3;
                    buf_rec2_wb_get(&buf_msg1, &len_req, &len_4); 
                    *(int *)buf_msg1 = len_req;
                    *(int *)(buf_msg1 + sizeof(int)) = len_4;
                    *(int *)(buf_msg1 + sizeof(int)*2) = sockfd;
                    memcpy(buf_msg1+sizeof(int)*3, buffer1, len);

                    d_savetofile_1(len);
                    
                    buf_rec2_wb_free(len_req);
                }
                else
                {
                    if (len < 0)
                        printf("%d: %s, sockid=%d\n",errno, strerror(errno),sockfd);
                    else
                    {
                        printf("Closed  sockid=%d\n", sockfd);                    
                    }

                    // pgh Ŀǰ��ֻ��Խ�������������ӣ�����������������������ط�ֱ�Ӷ������
                    // ע�⣬�������ط�����յĻ�����������ʱ�������ӻ�ϵ���vs�������������ԭ����
                    g_vs_sock[ACCESS_SERVER] = 0;
                    
                    break;
                }
            }
        }
    }

    printf( "thread_recv_from_con()�Ѿ��˳�....\n" );

    return 0;
}


int vs_connect( char *ip, int port, int type )
{
    int errno ;
    int sockfd = 0;
    struct sockaddr_in dest;

    if( !ip || !port )
    {
        return -1;
    }

    if(( sockfd = socket( AF_INET, SOCK_STREAM, 0 )) < 0 )
    {
        printf("\nsocket error!\n");
        return errno;
    }

    bzero( &dest, sizeof( dest ) );
    dest.sin_family = AF_INET;
    dest.sin_port = htons( port );
    if( inet_aton( ip, (struct in_addr *)&dest.sin_addr.s_addr ) == 0 )
    {
        return errno;
    }

    if( connect( sockfd, (struct sockaddr *) &dest, sizeof(dest)) != 0)
    {
        printf( "\nConnect error!\n" );
        return errno;
    }

    g_vs_sock[ type ] = sockfd;


    pthread_create(&th_recv_from_con_sockfd, NULL, thread_recv_from_con, (void*)g_vs_sock[ type ] );

 
    return 0;
}


int thread_access_reconnnet_main()
{
    int ret;
    
    // �������ӽ��������  192.168.5.88  ��������
    while(1)
    {    
        ret = vs_connect( g_config.para_network.ip_server_access_1.var, g_config.para_network.port_server_access.var , ACCESS_SERVER);
        if(0 != ret)
        {
            printf("���ӽ���ʧ�ܣ������ǽ��뻹�������У�sleep 2�������\n");
            sleep(2);
            continue;
        }

        // ������������ע������
        rountin_termRegister_req();
        pthread_join(th_recv_from_con_sockfd,NULL);
        th_recv_from_con_sockfd = 0;
        printf("�����������½��жԽ�������ӣ�������\n");
    }
    
    return 0;
}

int vs_reconnect( char *ip, int port, int type )
{
    int errno ;
    int sockfd = 0;
    struct sockaddr_in dest;

printf("gggggggggggggggggggggggggggggggg\n");

    if( !ip || !port )
    {
        return -1;
    }

    if(( sockfd = socket( AF_INET, SOCK_STREAM, 0 )) < 0 )
    {
        printf("\nsocket error!\n");
        return errno;
    }

    bzero( &dest, sizeof( dest ) );
    dest.sin_family = AF_INET;
    dest.sin_port = htons( port );
    if( inet_aton( ip, (struct in_addr *)&dest.sin_addr.s_addr ) == 0 )
    {
        return errno;
    }

    if( connect( sockfd, (struct sockaddr *) &dest, sizeof(dest)) != 0)
    {
        printf( "\nConnect error!\n" );
        return errno;
    }
printf("gggggggggggggggggggggggggggggggg, g_vs_sock[ type ]=%d\n", g_vs_sock[ type ]);
    g_vs_sock[ type ] = sockfd;
 printf("gggggggggggggggggggggggggggggggg, g_vs_sock[ type ]=%d\n", g_vs_sock[ type ]);
    return 0;
}

/****************************************

In :   
sockid:     socket id( ��������)
or macro( ��������)

buf:            buffer (point) which to send
ilen:       buflen

Ret:  if error, return the error number, else return 0.

****************************************/

int vs_send( int sockid, char *buf, int ilen )
{

    int errno;

    if( !sockid || !buf || ilen < 0 )
    {
        return -1;
    }
    
    int len = 0;


/* pgh 20080703 changed 
    if( sockid <= MAX_CONNECT_TO_SERVER )
    {
        len = send( g_vs_sock[ sockid ], buf, ilen, 0 );
        //printf("\n%d, sended %d bytes! \n", g_vs_sock[sockid], len );
    }
    else
    {
        len = send( sockid, buf, ilen, 0 );
        //printf("\n%d, sended %d bytes \n", sockid, len );
    }
*/
    len = send( sockid, buf, ilen, 0 );

    if( len < 0 )
    {
        printf( "\nSocket send error! Code : %d, Info: '%s'\n", errno, strerror(errno) );
        return errno;
    }

    return 0;
}

// �������� second:
int vs_socket_trans_control(int second, int bety_len)
{

    return 0;
}

int vs_send_real( int sockid, char *buf, int ilen )
{

    int errno;
    int len = 0;
    int second;
    int usecond;
    int bps;


struct timeval tv_pre, tv_after;
struct timezone tz_pre, tz_after;

    

    if( !sockid || !buf || ilen < 0 )
    {
        return -1;
    }

gettimeofday (&tv_pre , &tz_pre);    
    
    len = send( sockid, buf, ilen, 0 );
    //len = sendto( sockid, buf, ilen, 0 , NULL, 0);
    //printf("\n%d, sended %d bytes! \n", sockid, len );

    if( len < 0 )
    {
        printf( "\nSocket send error! Code : %d, Info: '%s'\n", errno, strerror(errno) );
        return errno;
    }

gettimeofday (&tv_after , &tz_after);

second = tv_after.tv_sec - tv_pre.tv_sec;
usecond = (tv_after.tv_usec - tv_pre.tv_usec) + second * 1000;

bps = ilen * 8 * 1000 / usecond / 1024;
//printf("------------------- �����ʱ=%d, bps=%d-----------------------\n", usecond, bps);    
    return 0;

}

/****************************************

������������

[In]
type      { SENDTYPE_TCP, SENDTYPE_UDP }

server    ���������ͣ�����
{
ACCESS_SERVER     // ���������
BRODCAST_SERVER    // �ַ�������
BACKUP_SERVER      // ���ݷ�����
}

[Ret]
���󷵻ش�����룬��ȷ���� 0

****************************************/

int package_send( char *buf, int ilen, int type, int send_type, int socketid )
{
    if( socketid != 0)
    {
        vs_send( socketid, buf, ilen );
    }
    else
    {
        if( type == SENDTYPE_TCP )
        {
            vs_send( g_vs_sock[send_type], buf, ilen );
        }
    }
    return 0;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

