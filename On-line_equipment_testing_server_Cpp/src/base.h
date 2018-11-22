#ifndef BASE_H
#define BASE_H

#include <exception>

#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <unistd.h>
#include <endian.h>

typedef struct {
    uint8_t dmac[6];        // Destination MAC address
    uint8_t smac[6];        // Source MAC address
    uint16_t eth_type;      // Ethernet type
} __attribute__((packed)) eth_header;

typedef struct {
    // Version (4 bits) + Internet header length (4 bits)
#if __BYTE_ORDER == __LITTLE_ENDIAN
    uint8_t  ihl : 4, version : 4;
#else
    uint8_t  version : 4, ihl : 4;
#endif
    uint8_t  tos;           // Type of service
    uint16_t tlen;          // Total length
    uint16_t ident;         // Identification
    uint16_t flags_fo;      // Flags (3 bits) + Fragment offset (13 bits)
    uint8_t  ttl;           // Time to live
    uint8_t  protocol;      // Protocol
    uint16_t check;         // Header checksum
    uint32_t saddr;         // Source address
    uint32_t daddr;         // Destination address
} __attribute__((packed)) ip_header;

//ICMP结构体定义（回显请求和回显应答的消息格式）
typedef struct {
    uint8_t type;       //类型
    uint8_t code;       //代码
    uint16_t chsum;     //校验和
    uint16_t iden;      //标识符
    uint16_t seq;       //序列号
    uint8_t *content;   //可选数据

} __attribute__((packed)) icmp_header;

typedef struct ITEM         // 队列元素
{
    uint32_t saddr;         // 用于保存Source address
    uint32_t daddr;         // 用于保存Destination address
}__attribute__((packed)) item_t;

class Socket
{
public:
    Socket()
    {
        sock_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (-1 == sock_)
        {
            throw std::exception();
        }
    }

    ~Socket()
    {
        close(sock_);
    }

    int get_sock()
    {
        return sock_;
    }

    Socket(const Socket &rhs) = delete;
    Socket& operator=(const Socket &rhs) = delete;

private:
    int sock_;
};

class File
{
public:
    explicit File(const char *conf_path)
    {
        fp_ = fopen(conf_path, "rb");
        if(nullptr == fp_)
        {
            throw std::exception();
        }
        /*把文件指针移动到文件尾*/
        fseek(fp_, 0, SEEK_END);
        /*获取文件内容长度*/
        file_len_ = ftell(fp_);
        /*把文件指针移动到文件首*/
        fseek(fp_, 0, SEEK_SET);
    }

    unsigned int get_file_len()
    {
        return file_len_;
    }

    FILE* get_fd()
    {
        return fp_;
    }

    ~File()
    {
        fclose(fp_);
        fp_ = nullptr;
        file_len_ = 0;
    }

    File(const File &rhs) = delete;
    File& operator=(const File &rhs) = delete;

private:
    FILE *fp_;
    unsigned int file_len_;
};

/*
 * 功能：获取设备ip
 * 参数1：网卡名，eg:eth0
 * 参数2：目的ip地址
 * 返回值：true：成功，false：失败
 */
bool get_dev_ip(const char *devname, struct sockaddr_in *sin);

/*
 * 功能：获取设备mac
 * 参数1：网卡名，eg:etho
 * 参数2：目的mac地址
 * 返回值：true：成功，false：失败
 */
bool get_dev_mac(const char *devname, char *mac);

/*
 * 功能：把点分十进制的ip地址转换成整数
 * 参数1：点分十进制的ip地址
 * 返回值：其它：成功，-1：失败
 */
int ip_to_int(const char *ip);

/*
 * 功能：把整数ip转换成点分十进制的ip地址
 * 参数1：整数ip
 * 参数2：转换后的ip地址
 * 返回值：true：成功，false：失败
 */
bool int_to_ip(unsigned int iip, char *ip);

/*
 * 功能：延时操作(微秒级别)
 * 参数1：延时的微秒数
 * 返回值：无
 */
void delay(unsigned long usec);

#endif
