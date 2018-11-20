#ifndef SEND_PACKET_H
#define SEND_PACKET_H

#include <stdint.h>

/*
 * 功能：发送ICMP请求包
 * 参数1：无
 * 返回值：true：成功，false：失败
 */
bool send_echo_icmp();

#endif // SEND_PACKET_H
