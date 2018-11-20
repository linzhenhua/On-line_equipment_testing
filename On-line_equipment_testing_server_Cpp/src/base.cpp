#include <memory>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include "base.h"

/*
 * 功能：获取设备ip
 * 参数1：网卡名，eg:eth0
 * 参数2：目的ip地址
 * 返回值：true：成功，false：失败
 */
bool get_dev_ip(const char *devname, struct sockaddr_in *sin)
{
    if(nullptr == devname || nullptr == sin)
    {
        return false;
    }

    Socket Sock;
    struct ifreq ifr;

    memset(&ifr, 0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, devname, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;
    if(ioctl(Sock.get_sock(), SIOCGIFADDR, &ifr) < 0)
    {
        return false;
    }

    memcpy(sin, &ifr.ifr_addr, sizeof(struct sockaddr_in));

    return true;
}

/*
 * 功能：获取设备mac
 * 参数1：网卡名，eg:etho
 * 参数2：目的mac地址
 * 返回值：true：成功，false：失败
 */
bool get_dev_mac(const char *devname, char *mac)
{
    if(nullptr == devname || nullptr == mac)
    {
        return false;
    }

    Socket Sock;
    struct ifreq ifr;

    memset(&ifr, 0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, devname, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;
    if(ioctl(Sock.get_sock(), SIOCGIFHWADDR, &ifr) < 0)
    {
        return false;
    }

    memcpy(mac, (char*)&ifr.ifr_hwaddr.sa_data, IFHWADDRLEN);

    return true;
}

/*
 * 功能：把点分十进制的ip地址转换成整数
 * 参数1：点分十进制的ip地址
 * 返回值：其它：成功，-1：失败
 */
int ip_to_int(const char *ip)
{
    int len = strlen(ip);
    if (len < 7 || len > 16)
    {
        return -1;
    }

    int ipip[4];
    if(4 == sscanf(ip, "%d.%d.%d.%d", &ipip[0], &ipip[1], &ipip[2], &ipip[3]))
    {
        if (ipip[0] > 255 || ipip[0] < 0 ||
            ipip[1] > 255 || ipip[1] < 0 ||
            ipip[2] > 255 || ipip[2] < 0 ||
            ipip[3] > 255 || ipip[3] < 0
                )
        {
            return -1;
        }

        return (ipip[0] << 24) + (ipip[1] << 16) + (ipip[2] << 8) + ipip[3];
    }
    else
    {
        return -1;
    }
}

/*
 * 功能：把整数ip转换成点分十进制的ip地址
 * 参数1：整数ip
 * 参数2：转换后的ip地址
 * 返回值：true：成功，false：失败
 */
bool int_to_ip(unsigned int iip, char *ip)
{
    if(nullptr == ip)
    {
        return false;
    }

    if(snprintf((ip), 16, "%d.%d.%d.%d", (((iip) >> 24) & 0xFF), (((iip) >> 16) & 0xFF), (((iip) >> 8) & 0xFF), ((iip) & 0xFF)) < 0)
    {
        return false;
    }

    return true;
}

/*
 * 功能：延时操作(微秒级别)
 * 参数1：延时的微秒数
 * 返回值：无
 */
void delay(unsigned long usec)
{
    struct timeval tv;
    tv.tv_sec = usec/1000000;
    tv.tv_usec = usec%1000000;
    int err;

    do{
        err = select(0,NULL,NULL,NULL,&tv);
    }while(err<0 && errno==EINTR);
}


