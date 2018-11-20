#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <libnet.h>

#include "send_packet.h"
#include "base.h"

extern program_conf_t my_program_conf;

/*
 * 功能：发送ICMP请求包
 * 参数1：无
 * 返回值：true：成功，false：失败
 */
bool send_echo_icmp()
{
    u_long source_ip = 0;            //本机地址
    struct sockaddr_in sin;

    memset(&sin, 0, sizeof(struct sockaddr_in));
    if(0 == get_dev_ip("ens32", &sin))
    {
        fprintf(stderr, "get_dev_ip run error\n");
        return false;
    }
    source_ip = (u_long)sin.sin_addr.s_addr;

    libnet_t *l = NULL;
    char errbuf[LIBNET_ERRBUF_SIZE] = {0};
    // 初始化libnet
    // libnet类型，为原始套接字IPv4类型
    // 网络设备接口,NULL为libnet寻找
    // 错误信息
    l = libnet_init(LIBNET_RAW4, NULL, errbuf);
    if(NULL == l)
    {
        fprintf(stderr, "libnet_init run error: %s\n", errbuf);
        return false;
    }

    u_long dest_ip = 0;
    libnet_ptag_t protocol_tag;
    uint8_t *payload = NULL;
    uint32_t payload_length = 0;

    char begin[16] = {0};
    char end[16] = {0};

    if(0 == strlen(my_program_conf.ip_range))
    {
        fprintf(stderr, "ip_range is NULL");
        return false;
    }
    else
    {
        char *first = NULL;
        first = strtok(my_program_conf.ip_range, "-");
        strcpy(begin, first);
        char *second = NULL;
        second = strtok(NULL, "-");
        strcpy(end, second);
    }

    //test
    static int count = 0;

    for(int i = ip_to_int(begin); i <= ip_to_int(end); ++i)
    {
        dest_ip = (u_long)htonl(i);

        //构造ICMP应答数据包
        protocol_tag = libnet_build_icmpv4_echo(
                                                ICMP_ECHO,                                          /* 类型，此时为回显请求 */
                                                0,                                                  /* 代码，应该为0 */
                                                0,                                                  /* 校验和，为0，表示由libnet句柄自动计算 */
                                                123,                                                /* 标识符，赋值为123，自己任意填写数值 */
                                                456,                                                /* 序列号，赋值为245，自己任意填写数值 */
                                                payload,                                            /* 负载，赋值为空 */
                                                payload_length,                                     /* 负载的长度，赋值为0 */
                                                l,                                                  /* libnet句柄，应该是由libnet_init()函数得到的 */
                                                0                                                   /* 协议块标记，赋值为0，表示构造一个新的协议块 */
                                                );

        if(-1 == protocol_tag)
        {
            fprintf(stderr, "can't build error ICMP header: %s\n", libnet_geterror(l));
            continue;
        }

        // 构造IP协议块
        protocol_tag = libnet_build_ipv4(
                                         LIBNET_IPV4_H + LIBNET_ICMPV4_ECHO_H + payload_length,     /* IP协议块的总长度 */
                                         0,                                                         /* 服务质量，这里赋值为0 */
                                         10,                                                        /* 标识符，这里赋值为10 */
                                         0,                                                         /* 偏移，这里赋值为0 */
                                         20,                                                        /* 生存时间，这里赋值为20 */
                                         IPPROTO_ICMP,                                              /* 上层协议类型，这里是ICMP协议 */
                                         0,                                                         /* 校验和，这里为0表示由libnet计算校验和 */
                                         source_ip,                                                 /* 源IP地址 */
                                         dest_ip,                                                   /* 目的IP地址 */
                                         payload,                                                   /* 负载 */
                                         payload_length,                                            /* 负载的长度 */
                                         l,                                                         /* libnet句柄，应该是由libnet_init()函数得到的 */
                                         0                                                          /* 协议块标记，赋值为0，表示构造一个新的协议块 */
                                        );

        if(-1 == protocol_tag)
        {
            fprintf(stderr, "can't build error IPv4 header: %s\n", libnet_geterror(l));
            continue;
        }

        /* 发送由libnet句柄l表示的数据包 */
        int packet_length = -1;
        packet_length = libnet_write(l);
        if(-1 == packet_length)
        {
            fprintf(stderr, "libnet_write run error: %s\n", libnet_geterror(l));
            continue;
        }

        libnet_clear_packet(l);    //必须增加这个函数，不然发包到53个左右，包长度超过1460导致发送失败

        //增加发包间隔（间隔可以1ms 5个包或10个包）
        delay(my_program_conf.send_packet_interval_by_millisec);

        //test
        count++;
    }

    //test
    printf("count:%d\n", count);

    //销毁libnet
    libnet_destroy(l);
    
    return true;
}