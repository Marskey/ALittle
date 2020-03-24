
#ifndef _ALITTLE_TIMER_H_
#define _ALITTLE_TIMER_H_

#include "miniheap.h"
#include "klib/khash.h"

KHASH_MAP_INIT_INT(node_info, node_info*)

typedef struct
{
    long long cur_time;
    int max_id;
    mini_heap* heap;
    node_info* pool;
    int pool_count;
    khash_t(node_info)* map;
} timer;


timer* timer_create();
void timer_destroy(timer* ts);

// ��Ӷ�ʱ��
int timer_add(timer* ts, int delay_ms, int loop, int interval_ms);
// �Ƴ���ʱ��
int timer_remove(timer* ts, int id);
// ����ʱ��
void timer_update(timer* ts, int frame_ms);
// ȡ����ʱ�Ķ�ʱ��ID
// �������0��˵��û�г�ʱ�Ķ�ʱ��
// �������������˵����ʱ����ʱ�����ҵȴ��´�ѭ��
// ������ظ�����˵����ʱ����ʱ�����Ҷ�ʱ���ѱ�ɾ��
int timer_poll(timer* ts);

#endif // _ALITTLE_TIMER_H_
