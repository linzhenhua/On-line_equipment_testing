#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/send_packet.h"
#include "src/base.h"

program_conf_t my_program_conf;

int main()
{
    const char *conf_path = "../program_conf";

    memset(&my_program_conf, 0, sizeof(program_conf_t));

    if(false == read_program_conf(conf_path, &my_program_conf))
    {
        //test begin
        //printf("program_conf->send_packet_interval_by_millisec:%d\n", program_conf.send_packet_interval_by_millisec);
        //printf("program_conf->ip_range:%s\n", program_conf.ip_range);
        //printf("program_conf->send_packet_strategy:%d\n", program_conf.send_packet_strategy);
        //test end

        fprintf(stderr, "read_program_conf run error\n");
        return 1;
    }

    switch(my_program_conf.send_packet_strategy)
    {
    case ICMP:
        {
            if(send_echo_icmp())
            {
                fprintf(stderr, "send_echo_icmp run ok\n");
            }
            else
            {
                fprintf(stderr, "send_echo_icmp run error\n");
                return 1;
            }
            break;
        }
    default:
        {
            fprintf(stderr, "not thing run\n");
            break;
        }
    }

    return 0;
}
