#include "ThreadPool.hpp"
#include <ctime>

pthread_spinlock_t slock;


int main()
{
    std::cout << "process run..." << std::endl;
    sleep(3);

    ThreadPool<Task>::GetInstance()->start();
    srand(time(nullptr) ^ getpid());

    while(true)
    {
        //1.构建任务，创建data1 ,data2, oprs;
        int x = rand() % 10 + 1;
        usleep(10);
        int y = rand() % 5;
        char op = opers[rand() % opers.size()];

        Task t(x, y, op);
        ThreadPool<Task>::GetInstance()->Push(t);

        //2.交给线程池处理
        std::cout << "main thread make task: " << t.GetTask() << std::endl;

        sleep(1);
    }
}