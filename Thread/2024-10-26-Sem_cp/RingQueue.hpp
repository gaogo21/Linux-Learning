#include <iostream>
#include <vector>
#include <semaphore.h>
#include <pthread.h>

//为什么基于阻塞队列的生产者消费者模型只需要一个锁 ， 而基于阻塞队列的生产者消费者模型需要两个锁？
//1.因为阻塞队列是对同一个队列的同一个位置进行操作，队列是共享资源，需要对整个队列加锁
//2.阻塞队列中，生产者和消费者访问的不是同一个下标，所以两者是互不干扰的，只需要用条件变量来唤醒阻塞
//但是生产者对生产者而言，是需要加锁的。消费者对消费者而言，是需要加锁的。

template <class T>
class RingQueue
{
    static const int defaultnum = 5;
public:
    //p操作，申请信号量，信号量--
    void p(sem_t& sem)
    {
        sem_wait(&sem);
    }

    //v操作，释放信号量，信号量++
    void v(sem_t& sem)
    {
        sem_post(&sem);
    }

    void Lock(pthread_mutex_t& mutex)
    {
        pthread_mutex_lock(&mutex);
    }

    void UnLock(pthread_mutex_t& mutex)
    {
        pthread_mutex_unlock(&mutex);
    }

public:
    RingQueue(int cap = defaultnum)
    :_ringqueue(cap)
    ,_cap(cap)
    ,_c_step(0)
    ,_p_step(0)
    {
        //初始化信号量，给消费者初始的信号量为0，给生产者初始的信号量为cap(满)
        //因为最开始的时候没有商品，无法消费，只能生产
        sem_init(&_c_data_sem, 0, 0);
        sem_init(&_p_space_sem, 0, cap);

        pthread_mutex_init(&_c_mutex, nullptr);
        pthread_mutex_init(&_p_mutex, nullptr);
    }

    //生产商品
    void push(const T &in)
    {   
        //1.申请信号量
        p(_p_space_sem);
        //2.消费者加锁
        Lock(_p_mutex);

        //3.进行生产
        _ringqueue[_p_step] = in;
        _p_step++;
        _p_step %= _cap;  //保证下标一直都在环形队列里面

        //4.解锁
        UnLock(_p_mutex);
        //5.释放信号量
        v(_c_data_sem);
    }

    void pop(T* out)
    {
        //1.申请信号量
        p(_c_data_sem);
        //2.申请锁
        Lock(_c_mutex);

        //3.消费数据
        *out = _ringqueue[_c_step];
        ++_c_step;
        _c_step %= _cap;

        //4.解锁
        UnLock(_c_mutex);
        //5.生产者信号量++
        v(_p_space_sem);  //消费完数据之后，生产者的信号量++
    }
private:
    std::vector<T> _ringqueue;
    int _cap;     //capacity
    int _c_step;  //消费者在环形队列中的下标
    int _p_step;  //生产者在环形队列中的下标

    sem_t _c_data_sem;  //消费者关注的数据资源
    sem_t _p_space_sem; //生产者关注的消费资源

    pthread_mutex_t _c_mutex;   //消费者锁
    pthread_mutex_t _p_mutex;   //生产者锁
};