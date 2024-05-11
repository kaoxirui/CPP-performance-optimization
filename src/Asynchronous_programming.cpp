#include<iostream>
#include<future>
#include<thread>
#include<chrono>

//模拟一个长时间运行的函数
int longRunningFunction(int input){
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return input*input;
}

int main(){
    //使用std::async启动异步任务
    std::future<int>resultFuture=std::async(std::launch::async,longRunningFunction,4);
    //这里主线程可以执行其他任务，而不会被LongRunningFunction阻塞
    std::cout<<"runnning other tasks...\n"<<std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));//模拟其他任务
    //当需要longRunningFunction的结果时，使用std::future::get()获取
    int result=resultFuture.get();//这里会阻塞，直到结果可用
    std::cout<<"the result of longRunningFunction is:"<<result<<std::endl;
    return 0;

}