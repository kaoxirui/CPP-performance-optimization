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
        std::queue<std::function<void()>>tasks;//std::function<void()>可以封装无参数和无返回值的函数或可调用对象。void是返回类型，（）是参数

        std::mutex queue_mutex;
        std::condition_variable condition;
        bool stop;
    public:
        ThreadPool(size_t);
        /*
            enqueue函数模板是一个用于将任务添加到线程池的成员函数
            接收一个可调用对象f和一系列参数args...，并返回一个与f调用结果类型相对应的std::future对象
            目的：
            将传递给线程池的任务包装成一个可以异步执行的任务，并返回一个std::future对象，以便可以在需要的时候获取任务的返回值

        */
        template<class F,class... Args>
        auto enqueue(F&& f,Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;
        /*
            std::result_of<F(Args...)>是一个类型萃取表达式，用于推导出f调用的返回类型，
            会返回F在给定参数Args...下的调用结果类型
            这意味着 enqueue 函数可以支持传递任意可调用对象，并根据传递的参数类型自动推导返回类型。
            返回类型为std::future且std::future的模板参数是typenamestd::result_of<F(Args...)>::type
            即F在参数Args... 下的调用结果类型

            ->是尾置返回类型，在 C++11 中引入了尾置返回类型的语法，
            允许在函数定义的末尾使用 auto 关键字和尾置返回类型来指定函数的返回类型。
        */
        ~ThreadPool();
};

inline ThreadPool::ThreadPool(size_t threads):stop(false){
    for(size_t i=0;i<threads;++i)//使用循环创建指定数量的工作线程
        workers.emplace_back([this]{//lambda表达式捕获了this指针，以便在线程函数内部访问ThreadPool对象的成员
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
/*
    :stop(false)为使用成员初始化列表的方式对成员变量进行初始化，stop 是 ThreadPool 类的一个成员变量，它在成员初始化列表中被初始化为 false。
    使用成员初始化列表的优点是可以直接为成员变量提供初始值，而不需要在构造函数体内使用赋值操作符来初始化成员变量。
    这样可以提高代码的效率，并避免一些潜在的问题。
*/
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