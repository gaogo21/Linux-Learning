#pragma once

#include <iostream>
#include <queue>
#include <string>
#include <vector>
#include <unistd.h>
#include "Task.hpp"

using namespace std;

static const int defaultnum = 5;

class ThreadInfo
{
public:
    pthread_t _tid;
    string _threadname;    
};

template <class T>
class ThreadPool
{
public:
    void Lock()
    {
        pthread_mutex_lock(&_mutex);
    }

    void Unlock()
    {
        pthread_mutex_unlock(&_mutex);
    }

    void WakeUp()
    {
        pthread_cond_signal(&_cond);
    }

    void ThreadSleep()
    {
        pthread_cond_wait(&_cond, &_mutex);
    }

    bool IsQueueEmpty()
    {
        return _tasks.empty();
    }

    string GetThreadName(pthread_t tid)
    {
        for (const auto &t : _threads)
        {
            if (t._tid == tid)
            {
                return t._threadname;
            }
        }

        return "None";
    }

public:

    static void* HandlerTask(void* args)
    {
        ThreadPool<T>* _tp = static_cast<ThreadPool<T> *> (args);
        string name = _tp->GetThreadName(pthread_self());

        while(true)
        {
            _tp->Lock();

            //当任务为空的时候，线程阻塞，直到Push()之后，任务不为空，就可以唤醒线程了
            while(_tp->IsQueueEmpty())
            {
                _tp->ThreadSleep();
            }

            T t = _tp->Pop();

            _tp->Unlock();

            t();
            cout << name << " run, " << "result: " << t.GetResult() << endl;
        }
    }
    //线程启动函数
    void start()
    {
        int num = _threads.size();
        for (int i = 0; i < num; ++ i)
        {
            _threads[i]._threadname = "thread-" + to_string(i);
            pthread_create(&_threads[i]._tid, nullptr, HandlerTask, this);
        }
    }

    T Pop()
    {
        T t = _tasks.front();
        _tasks.pop();
        return t;
    }

    void Push(const T& t)
    {
        Lock();
        _tasks.push(t);
        WakeUp();
        Unlock();
    }

    //单例模式
    static ThreadPool<T>* GetInstance()
    {   
        //为什么这么写？不是写_tp == nullptr?
        if (nullptr == _tp)
        {
            pthread_mutex_lock(&_lock);
            if (nullptr == _tp)
            {
                cout << "singleton create done first!" << endl;
                _tp = new ThreadPool<T>();
            }
            pthread_mutex_unlock(&_lock);
        }

        return _tp;
    }

private:
    //num表示创建的线程池的数量
    ThreadPool(int num = defaultnum)
        :_threads(num)
        {
            pthread_mutex_init(&_mutex, nullptr);
            pthread_cond_init(&_cond, nullptr);
        }
    
    ~ThreadPool()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);
    }

    //?
    ThreadPool(const ThreadPool<T> &) = delete;
    const ThreadPool<T> &operator=(const ThreadPool<T> &) = delete;

private:
    //管理线程 -- 因为线程池会创建很多线程
    vector<ThreadInfo> _threads;
    //管理生产者产生的任务
    queue<T> _tasks;

    pthread_mutex_t _mutex;
    pthread_cond_t _cond;  //条件变量，用于实现线程间的同步

    //ThreadPool<T>* 类型的指针，用于实现线程池的单例模式
    static ThreadPool<T> *_tp;
    //静态的互斥锁，确保在创建单例实例时的线程安全
    static pthread_mutex_t _lock;
};

template<class T>
ThreadPool<T>* ThreadPool<T>::_tp = nullptr;

template<class T>
pthread_mutex_t ThreadPool<T>::_lock = PTHREAD_MUTEX_INITIALIZER;