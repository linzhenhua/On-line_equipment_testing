#include <vector>
#include <thread>

#include <stdio.h>
#include <string.h>

#include "./src/base.h"
#include "./src/capture_packet.h"
#include "./src/send_packet.h"
#include "./include/concurrentqueue.h"
#include "./include/blockingconcurrentqueue.h"

#define THREAD_NUM 10

moodycamel::ConcurrentQueue<item_t> g_concurrent_queue;
//moodycamel::BlockingConcurrentQueue<item_t> g_blocking_concurrent_queue;

//消费者线程
//同步线程
void consumer()
{
   item_t item;

   //test
   static int count = 0;

   //发送响应数据包
   while(1)
   {
      //g_blocking_concurrent_queue.wait_dequeue(item);     //只在获取元素的时候可以阻塞

      if(!g_concurrent_queue.try_dequeue(item))
      {
         //printf("try_dequeue error\n");

         delay(1000);

         continue;
      }  

       //test
       //printf("%d\n", count++);

       send_reply_icmp(item.saddr, item.daddr);
       //memset(&item, 0, sizeof(item_t));
   }
}

int main()
{ 
   //消费者线程id
   std::vector<std::thread> threads;

   //创建线程
   for(int i = 0; i < THREAD_NUM; ++i)
   {
      threads.push_back(std::thread(consumer));
   }

   if(false == capture_packet())
   {
      for(auto& th : threads)
      {
         th.join(); 
      }

      return 1;
   }

   for(auto& th : threads)
   {
      th.join(); 
   }

   //test
   //capture_packet();

   return 0;
}
