#include <memory>

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <net/if.h>

#include "libnet.h"

/*
 * 功能：发送ICMP应答包
 * 参数1：源ip地址
 * 参数2：目的ip地址
 * 返回值：true：成功，false：失败
 */
bool send_reply_icmp(unsigned int source_ip, unsigned int dest_ip)
{
    char errbuf[LIBNET_ERRBUF_SIZE] = {0};

    // 初始化libnet
    // libnet类型，为原始套接字IPv4类型
    // 网络设备接口,NULL为libnet寻找
    // 错误信息
    std::shared_ptr<libnet_t> l(libnet_init(LIBNET_RAW4, nullptr, errbuf), [](libnet_t *l_ptr){libnet_destroy(l_ptr);});
    if(nullptr == l.get())
    {
        fprintf(stderr, "libnet_init error: %s\n", errbuf);
        return false;
    }

    libnet_ptag_t protocol_tag;
    const unsigned char *payload = NULL;
    unsigned int payload_length = 0;
    //构造ICMP应答数据包
    protocol_tag = libnet_build_icmpv4_echo(
                                            ICMP_ECHOREPLY,                                     /* 类型，此时为回显应答 */
                                            0,                                                  /* 代码，应该为0 */
                                            0,                                                  /* 校验和，为0，表示由libnet句柄自动计算 */
                                            123,                                                /* 标识符，赋值为123，自己任意填写数值 */
                                            456,                                                /* 序列号，赋值为245，自己任意填写数值 */
                                            payload,                                            /* 负载，赋值为空 */
                                            payload_length,                                     /* 负载的长度，赋值为0 */
                                            l.get(),                                            /* libnet句柄，应该是由libnet_init()函数得到的 */
                                            0                                                   /* 协议块标记，赋值为0，表示构造一个新的协议块 */
                                            );
    if(-1 == protocol_tag)
    {
        fprintf(stderr, "can't build error ICMP header: %s\n", libnet_geterror(l.get()));
        return false;
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
                                     l.get(),                                                   /* libnet句柄，应该是由libnet_init()函数得到的 */
                                     0                                                          /* 协议块标记，赋值为0，表示构造一个新的协议块 */
                                    );
    if(-1 == protocol_tag)
    {
        fprintf(stderr, "can't build error IPv4 header: %s\n", libnet_geterror(l.get()));
        return false;
    }

    /* 发送由libnet句柄l表示的数据包 */
    int packet_length = -1;
    packet_length = libnet_write(l.get());
    if(-1 == packet_length)
    {
        fprintf(stderr, "write error: %s\n", libnet_geterror(l.get()));
        return false;
    }

    return true;
}
