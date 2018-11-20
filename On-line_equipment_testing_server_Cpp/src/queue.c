#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "queue.h"

/*
 * 功能：初始化队列
 * 参数1：队列
 * 参数2：队列大小
 * 返回值：0：成功，1：失败
 */
int32_t queue_init(queue_t *Q, int32_t size)
{
    Q->head = 0;
    Q->tail = 0;
    Q->count = 0;

    Q->array = (item_t*)calloc(size, sizeof(item_t));
    if(Q->array != NULL)
    {
        Q->size = size;
        return 0;
    }
    else
    {
        Q->size = 0;
        fprintf(stderr, "queue_init run error.\n");
        return 1;
    }
}

/*
 * 功能：释放队列
 * 参数1：队列
 * 返回值：无
 */
void queue_free(queue_t *Q)
{
    free(Q->array);
    Q->array = NULL;
    Q->head = 0;
    Q->tail = 0;
    Q->count = 0;
    Q->size = 0;
}

/*
 * 功能：判断队列是否为空
 * 参数1：队列
 * 返回值：0：队列不为空，1：队列为空
 */
int32_t queue_isempty(queue_t *Q)
{
    if(Q->array == NULL)
    {
        fprintf(stderr, "Queue is not initialized.\n");
        return 1;
    }

    if(Q->count == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
 * 功能：入队列
 * 参数1：队列
 * 参数2：队列元素
 * 返回值：0：成功，1：失败
 */
int32_t queue_enqueue(queue_t *Q, item_t *item)
{
    if(Q->array == NULL)
    {
        fprintf(stderr, "Queue is not initialized.\n");
        return 1;
    }

    if(Q->count < Q->size)
    {
        memcpy(&Q->array[Q->head], item, sizeof(struct ITEM));
        Q->head = (Q->head + 1) % Q->size;
        Q->count++;
        return 0;
    }
    else
    {
        fprintf(stderr, "Queue overflow.\n");
        return 1;
    }
}

/*
 * 功能：出队列
 * 参数1：队列
 * 参数2：队列元素
 * 返回值：0：成功，1：失败
 */
int32_t queue_dequeue(queue_t *Q, item_t *item)
{
    if(Q->array == NULL)
    {
        fprintf(stderr, "Queue is not initialized.\n");
        return 1;
    }

    if(0 == queue_isempty(Q))
    {
        //*item = Q->array[Q->tail];
        memcpy(item, &Q->array[Q->tail], sizeof(struct ITEM));

        Q->tail = (Q->tail + 1) % Q->size;
        Q->count--;
        return 0;
    }
    else
    {
        fprintf(stderr, "Queue is empty.\n");
        return 1;
    }
}


