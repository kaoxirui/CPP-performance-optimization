#include<iostream>
#include<thread>
#include<vector>
#include<queue>
#include<functional>
#include<stdexcept>
#include<future>
#include<condition_variable>
class ThreadPool{
    private:
        std::vector<std::thread>workers;
        std::queue<std::function<void()>>tasks;

        std::mutex queue_mutex;
        std::condition_variable condition;
        bool stop;
    public:
        ThreadPool(size_t);
        template<class F,class... Args>
        auto enqueue(F&& f,Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;
        ~ThreadPool();
};

inline ThreadPool::ThreadPool(size_t threads):stop(false){
    for(size_t i=0;i<threads;++i)
        workers.emplace_back([this]{
            for(;;){
                std::function<void()>task;
                {
                    std::unique_lock<std::mutex>lock(this->queue_mutex);
                    this->condition.wait(lock,[this]{
                        return this->stop || !this->tasks.empty();
                    });
                    if(this->stop&&this->tasks.front())
                        return;
                    task=std::move(this->tasks.front());
                    this->tasks.pop();
                }
            task();
            }
        });
}
//add new work item to the pool

template<class F,class... Args>
auto ThreadPool::enqueue(F&& f,Args&&... args)
->std::future<typename std::result_of<F(Args...)>::type>{
    using return_type=typename std::result_of<F(Args...)>::type>;
    auto task=std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f),std::forward<Args>(args)...)
    );
    std::future<return_type>res=task->get_future();
    {
        std::unique_lock<std::mutex>lock(queue_mutex);
        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");
        tasks.emplace([task](){(*task)();});
    }
    condition.notify_one();
    return res;
}

inline ThreadPool::~ThreadPool(){
    {
        std::unique_lock<std::mutex>lock(queue_mutex);
        stop=true;
    }
    condition.notify_all();
    for(std::thread&worker:workers){
        worker.join();
    }
}
int some_function(int arg1, int arg2)
{
    // 执行一些操作
    return arg1 + arg2;
}
int main() {
    ThreadPool pool(4);

    // 提交任务到线程池
    std::future<int> fut1 = pool.enqueue([]() {
        // 任务1的代码
        // 返回一个int类型的结果
        return 42;
    });

    std::future<void> fut2 = pool.enqueue([]() {
        // 任务2的代码
        // 无返回值
    });

    // 获取任务的返回值或等待任务完成
    int result = fut1.get(); // 阻塞等待任务1完成并获取返回值
    fut2.get(); // 阻塞等待任务2完成

    return 0;
}