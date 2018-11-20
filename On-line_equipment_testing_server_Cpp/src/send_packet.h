#ifndef SEND_PACKET_H
#define SEND_PACKET_H

/*
 * 功能：发送ICMP应答包
 * 参数1：源ip地址
 * 参数2：目的ip地址
 * 返回值：true：成功，false：失败
 */
bool send_reply_icmp(unsigned int source_ip, unsigned int dest_ip);

#endif // SEND_PACKET_H
