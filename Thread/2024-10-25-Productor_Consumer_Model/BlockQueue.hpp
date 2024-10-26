#include <iostream>
#include <queue>
#include <pthread.h>

template<class T>
class BlockQueue
{
    static const int defaultnum = 20;
public:
    BlockQueue(int maxcap = defaultnum)
    :_maxcap(maxcap)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_c_cond, nullptr);
        pthread_cond_init(&_p_cond, nullptr);
    }

    T pop()
    {
        //1.上锁  --> 消费的时候，需要给消费者上锁
        pthread_mutex_lock(&_mutex);

        while(_q.size() == 0)
        {
            //当商品为空的时候，就阻塞消费者
            pthread_cond_wait(&_c_cond, &_mutex);
        }

        //3.走到这里，两种情况 ： 1.队列满了  2.被唤醒
        T out = _q.front();
        _q.pop();

        //4.当pop之后，队列就一定没满，因此可以唤醒生产者去生产了
        pthread_cond_signal(&_p_cond);

        pthread_mutex_unlock(&_mutex);

        return out;
    }

    void push(const T& in)
    {
        //1.上锁  --> 生产的时候，需要给生产者上锁
        pthread_mutex_lock(&_mutex);
        
        //2.当条件满足的时候，释放与p_cond相关的互斥锁，使“生产”线程进入阻塞状态
        //伪唤醒的情况
        while(_q.size() == _maxcap)
        {
            //自动唤醒  --> 释放_p_cond持有的锁，进入阻塞状态
            pthread_cond_wait(&_p_cond, &_mutex);
        }

        //3.生产内容 --> 走到这一步有两种可能：1.队列未满，2.被唤醒
        _q.push(in);

        pthread_cond_signal(&_c_cond);
        pthread_mutex_unlock(&_mutex);
    }

    ~BlockQueue()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_c_cond);
        pthread_cond_destroy(&_p_cond);
    }
private:
    std::queue<T> _q;  //共享资源，只有一个，但是可以被当成很多个
    int _maxcap;  //最大值

    pthread_mutex_t _mutex;   //锁
    pthread_cond_t _c_cond;    //consumer cond 消费者条件变量
    pthread_cond_t _p_cond;    //productor cond 生产者条件变量
};