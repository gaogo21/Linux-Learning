#include "BlockQueue.hpp"
#include "Task.hpp"
#include <unistd.h>
#include <ctime>

void* Consumer(void *args)
{
    BlockQueue<Task> *bq = static_cast<BlockQueue<Task>*>(args);
    
    while(true)
    {
        Task t = bq->pop();

        std::cout << "消费了一个任务 : " << t.GetTask() << " 运算结果是 : " 
        << t.GetResult() << "thread id : " << pthread_self() << std::endl;

        sleep(1);
    }

}

void* Productor(void *args)
{
    BlockQueue<Task> *bq = static_cast<BlockQueue<Task>*>(args);

    int x = 10, y = 20;
    while(true)
    {
        int data1 = rand() % 10 + 1;  //控制data1为[1,10]之间
        usleep(10);
        int data2 = rand() % 10 + 1;  //控制data2为[1,10]之间
        char op = opers[rand() % opers.size()];  //随机选取一个运算符

        //构建任务
        Task t(data1, data2, op);
        bq->push(t);
        std::cout << "生产了一个任务 : "  << t.GetTask() << "thread id : " << pthread_self() << std::endl;
        sleep(1);
    }
}

int main()
{
    srand(time(nullptr));
    BlockQueue<Task> *bq = new BlockQueue<Task>();
    pthread_t c[3], p[5];

    for (int i = 0; i < 3; ++ i)
    {
        pthread_create(c + i, nullptr, Consumer, bq);
    }

    for (int i = 0; i < 5; ++ i)
    {
        pthread_create(p + i, nullptr, Productor, bq);
    }

    for (int i = 0; i < 3; ++ i)
    {
        pthread_join(c[i], nullptr);
    }

    for (int i = 0; i < 5; ++ i)
    {
        pthread_join(p[i], nullptr);
    }

    delete bq;
    return 0;
}