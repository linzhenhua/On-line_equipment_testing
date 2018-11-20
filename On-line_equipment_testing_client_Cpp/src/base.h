#ifndef BASE_H
#define BASE_H

#include <exception>

#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <unistd.h>

#define ICMP 0

class Socket
{
public:
    Socket()
    {
        sock_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (-1 == sock_)
        {
            fprintf(stderr, "socket run error\n");
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
            fprintf(stderr, "fopen conf error\n");
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

typedef struct program_conf
{
    unsigned int send_packet_interval_by_millisec;      /*发包间隔，单位：毫秒*/
    char ip_range[50];                                  /*扫描网段*/
    int send_packet_strategy;                           /*发包策略*/
}program_conf_t;


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

/*
 * 功能：读取程序的配置的信息
 * 参数1：配置文件路径
 * 参数2：获取的配置信息
 * 返回值：true：成功，false：失败
 */
bool read_program_conf(const char *conf_path, program_conf_t *my_program_conf);

#endif
