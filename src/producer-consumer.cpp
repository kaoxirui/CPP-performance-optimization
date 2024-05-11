/*
    `sem_t`是POSIX线程库提供的一个信号量类型，用于同步多个线程对共享资源的访问。
     它可以用于线程之间的同步，进程之间的同步以及进程和线程之间的同步
    `sem_t`类型的变量被称为信号量，它通常是一个非负整数，并且具有以下两个基本操作：
        `sem_wait()`：请求信号量，如果信号量的值大于0，则将其减1，并继续执行，否则当前线程将阻塞知道信号量的值大于0
        `sem_post()`：释放信号量，将信号量的值加1

    pthread_mutex_t 是 POSIX（Portable Operating System Interface）线程库中的数据类型，通常简称为 Pthreads。
    它提供了一种标准化的方式，用于多线程程序同步访问共享资源，以防止数据损坏和竞争条件。

    pthread_mutex_t 实际上是一个互斥锁（mutex）对象。它用于创建和管理多线程程序中的互斥锁。
    互斥锁是同步原语，允许多个线程协调工作，确保在任何时刻只有一个线程可以访问关键代码段或共享资源。
    这可以防止在多个线程同时尝试访问相同资源时发生冲突和数据损坏。
*/

#include<iostream>
#include<semaphore.h>
#include<pthread.h>
#include<list>
#include<unistd.h>
#define BUFFER_SIZE 10

sem_t emptySlots;
sem_t fullSlots;
pthread_mutex_t mutex;
pthread_cond_t cond_consumer;
std::list<int> buffers;

void* producer(void* arg){
    for(int i=0;i<20;i++){
        sem_wait(&emptySlots);//等待有空位,请求信号量，若信号量的值大于0，则将其减1并继续执行，否则当前线程将被阻塞值扫信号量的值变为大于0
        pthread_mutex_lock(&mutex);
        buffers.push_back(i);
        std::cout<<"Produced:"<<i<<std::endl;
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond_consumer);//通知消费者有新的数据
        sem_post(&fullSlots);//释放信号量，将信号量的值加1(数据加一)
        usleep(1000000);
    }
    return nullptr;
}

void* consumer(void* arg){
    for(int i=0;i<20;i++){
        sem_wait(&fullSlots);
        pthread_mutex_lock(&mutex);
        while(buffers.empty()){
            pthread_cond_wait(&cond_consumer,&mutex);
        }
        int item=buffers.front();
        buffers.pop_front();
        std::cout<<"Consumed:"<<item<<std::endl;
        pthread_mutex_unlock(&mutex);
        sem_post(&emptySlots);//增加空槽位
        usleep(150000);
    }
    return nullptr;
}

int main(){
    sem_init(&emptySlots,0,BUFFER_SIZE);
    sem_init(&fullSlots,0,0);
    pthread_mutex_init(&mutex,nullptr);
    pthread_cond_init(&cond_consumer,nullptr);

    pthread_t prod,cons;

    pthread_create(&prod,nullptr,producer,nullptr);
    pthread_create(&cons,nullptr,consumer,nullptr);
    /*
    请注意，pthread_create 函数的第一个参数是指向线程标识符的指针，
    第二个参数是线程属性（通常使用 nullptr 表示默认属性），
    第三个参数是线程的入口函数，最后一个参数是传递给线程入口函数的参数。
    */
   pthread_join(prod,nullptr);
   pthread_join(cons,nullptr);

   sem_destroy(&emptySlots);
   sem_destroy(&fullSlots);

   pthread_mutex_destroy(&mutex);
   pthread_cond_destroy(&cond_consumer);

   return 0;
}