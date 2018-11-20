#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

typedef struct ITEM         // 队列元素
{
    uint32_t saddr;         // 用于保存Source address
    uint32_t daddr;         // 用于保存Destination address
}__attribute__((packed)) item_t;

typedef struct QUEUE        //环形队列
{
    int32_t head;               //头
    int32_t tail;               //尾
    int32_t count;              //队列当前元素个数
    int32_t size;               //队列大小
    item_t *array;
}__attribute__((packed)) queue_t;

/*
 * 功能：初始化队列
 * 参数1：队列
 * 参数2：队列大小
 * 返回值：0：成功，1：失败
 */
int32_t queue_init(queue_t *Q, int32_t size);

/*
 * 功能：释放队列
 * 参数1：队列
 * 返回值：无
 */
void queue_free(queue_t *Q);

/*
 * 功能：判断队列是否为空
 * 参数1：队列
 * 返回值：0：队列不为空，1：队列为空
 */
int32_t queue_isempty(queue_t *Q);

/*
 * 功能：入队列
 * 参数1：队列
 * 参数2：队列元素
 * 返回值：0：成功，1：失败
 */
int32_t queue_enqueue(queue_t *Q, item_t *item);

/*
 * 功能：出队列
 * 参数1：队列
 * 参数2：队列元素
 * 返回值：0：成功，1：失败
 */
int32_t queue_dequeue(queue_t *Q, item_t *item);

#endif
