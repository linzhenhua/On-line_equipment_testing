#include <memory>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "pcap.h"
#include "../include/concurrentqueue.h"
#include "../include/blockingconcurrentqueue.h"
#include "capture_packet.h"
#include "base.h"

extern moodycamel::ConcurrentQueue<item_t> g_concurrent_queue;
//extern moodycamel::BlockingConcurrentQueue<item_t> g_blocking_concurrent_queue;

/* Callback function invoked by libpcap for every incoming packet */
/*生产者线程*/
/*这里把捕获的数据包放入队列*/
/*异步线程*/
//static void capture_packet_callback_v2(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
//{
//    /* retireve the position of the ip header */
//    ip_header *ih = (ip_header *)(pkt_data + sizeof(eth_header));
//
//    item_t item;
//    item.saddr = ih->saddr;
//    item.daddr = ih->daddr;
//
//    //加锁
//    pthread_mutex_lock(&mutex);
//
//    queue_enqueue(&Q, &item);     //入队列
//
//    pthread_cond_signal(&cond);
//
//    //解锁
//    pthread_mutex_unlock(&mutex);
//
//    return;
//}

/* Callback function invoked by libpcap for every incoming packet */
/*生产者线程*/
/*这里把捕获的数据包放入队列*/
/*异步线程*/
static void capture_packet_callback(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
    /* retireve the position of the ip header */
    ip_header *ih = (ip_header *)(pkt_data + sizeof(eth_header));

    item_t item;
    item.saddr = ih->saddr;    //大端形态
    item.daddr = ih->daddr;

//    g_blocking_concurrent_queue.enqueue(item);
    g_concurrent_queue.enqueue(item);

    //test
    //static int count = 0;
    //printf("%d\n", count++);
}

/*
 * 功能：开启抓包程序
 * 参数：无
 * 返回值：true：成功
 *        false：失败
 */
bool capture_packet()
{
    int adaptor = 0;
    int i = 0;
    char errbuf[PCAP_ERRBUF_SIZE] = {0};
    pcap_if_t *alldevs, *d;

    /* Retrieve the device list from the local machine */
    if(pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        printf("Error in pcap_findalldevs_ex: %s\n", errbuf);
        return false;
    }

    std::shared_ptr<pcap_if_t> alldevs_ptr(alldevs, [](pcap_if_t *x){pcap_freealldevs(x);});

	if(adaptor == 0)
    {
		/* Print the list */
		for (d = alldevs_ptr.get(); d != nullptr; d = d->next)
        {
			printf("%d. %s\n", ++i, d->description ? d->description : d->name);
        }

		if(i == 0)
        {
			printf("\nNo interfaces found! Make sure WinPcap/libpcap is installed.\n");
			return false;
		}

		printf("Enter the interface number (1-%d): ", i);
		scanf("%d", &adaptor);
		//adaptor = 1;    //test

		if(adaptor <= 0 || adaptor > i)
        {
			printf("\nInterface number out of range.\n");
			return false;
		}
	}

    /* Jump to the selected adapter */
    for(d = alldevs_ptr.get(), i = 1; i < adaptor; d = d->next, ++i);

    std::shared_ptr<pcap_t> adhandle_ptr(pcap_open_live(d->name, 65535, 1, 1000, errbuf), [](pcap_t *x){pcap_close(x);});

    /* Open the device */
    if(adhandle_ptr.get() == nullptr)
    {
        printf("\nUnable to open the adapter. %s is not supported by WinPcap\n", d->name);
        return false;
    }

    /* Check the link layer. We support only Ethernet for simplicity. */
    if(pcap_datalink(adhandle_ptr.get()) != DLT_EN10MB)
    {
        printf("\nThis program works only on Ethernet networks.\n");
        return false;
    }

    struct bpf_program fcode;
    std::shared_ptr<bpf_program> fcode_ptr(&fcode, [](struct bpf_program *x){pcap_freecode(x);});

    // If the interface is without an address we suppose to be in a C class network
    bpf_u_int32 netmask = 0xffffff;

    //const char *filter_snippet = "ip and tcp"; //keep only the packets that are both IPv4 and TCP and deliver them to the application
    const char *filter_snippet = "icmp[icmptype] == icmp-echo";     //捕获icmp的echo包

    /* compile the filter */
    if(pcap_compile(adhandle_ptr.get(), fcode_ptr.get(), filter_snippet, 1, netmask) < 0)
    {
        printf("\nUnable to compile the packet filter. Check the syntax.\n");
        return false;
    }

    /* set the filter */
    if(pcap_setfilter(adhandle_ptr.get(), fcode_ptr.get()) < 0)
    {
        fprintf(stderr, "\nError setting the filter.\n");
        return false;
    }

    printf("\nlistening on %s...\n", d->name);

    /* start the capture */
    pcap_loop(adhandle_ptr.get(), -1, capture_packet_callback, NULL);

    //test
    //pcap_loop(adhandle_ptr.get(), 10, capture_packet_callback, NULL);

    return true;
}

