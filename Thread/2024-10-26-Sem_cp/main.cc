//基于环形队列的生产者消费者模型
#include "RingQueue.hpp"
#include "Task.hpp"
#include <ctime>
#include <unistd.h>

using namespace std;

struct ThreadData
{
    RingQueue<Task> *rq;
    string threadname;
};

void* Consumer(void* args)
{
    ThreadData* td = static_cast<ThreadData*>(args);
    RingQueue<Task>* rq = td->rq;
    string Threadname = td->threadname;

    while(true)
    {
        //1.消费数据
        Task t;
        rq->pop(&t);
        t();  //通过调用operator()执行run()

        cout << "消费了一个数据 : "<< t.GetTask() << 
        " 消费者 : " << Threadname << " 结果 : " << t.GetResult() << endl;

        sleep(1);
    }

    return nullptr;
}

void* Productor(void* args)
{
    ThreadData* td = static_cast<ThreadData*>(args);
    RingQueue<Task>* rq = td->rq;
    string Threadname = td->threadname;

    int data1 = 10, data2 = 20;
    while(true)
    {
        data1 = rand() % 10 + 1;
        usleep(10);
        data2 = rand() % 10 + 1;
        char op = opers[rand() % opers.size()];

        //构建任务（商品）
        Task t(data1, data2, op);
        rq->push(t);

        cout << "生产了一个数据 : " << t.GetTask() << " 生产者 : " << Threadname << endl;

        sleep(1);
    }

    return nullptr;
}

int main()
{
    RingQueue<Task> *rq = new RingQueue<Task>(3);

    pthread_t c[3], p[5];

    //消费者
    for (int i = 0; i < 3; ++ i)
    {
        ThreadData* td = new ThreadData();
        td->rq = rq;
        td->threadname = "Consumer-" + to_string(i);

        pthread_create(c + i, nullptr, Consumer, td);
    }

    //生产者
    for (int i = 0; i < 5; ++ i)
    {
        ThreadData* td = new ThreadData();
        td->rq = rq;
        td->threadname = "Productor-" + to_string(i);

        pthread_create(p + i, nullptr, Productor, td);
    }

    for (int i = 0; i < 3; ++ i)
    {
        pthread_join(c[i], nullptr);
    }

    for (int i = 0; i < 5; ++ i)
    {
        pthread_join(p[i], nullptr);
    }

    return 0;
}