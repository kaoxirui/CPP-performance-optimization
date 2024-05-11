#include<iostream>
#include<vector>
#include<thread>
#include<mutex>

class SharedCounter{
    private:
        mutable std::mutex mutex_;//当一个成员变量声明为mutable时，意味着成员变量可以被任何方法修改，即使这个方法是const类型的
                                    //这是因为mutable关键字可以打破const类型的限制，使得const类型的方法也可以修改成员变量的值。
        int count_=0;
    public:
        void inrement(){
            std::lock_guard<std::mutex>lock(mutex_);
            ++count_;
        }
        int getCount()const{
            std::lock_guard<std::mutex>lock(mutex_);
            return count_;
        }
};

void worker(SharedCounter& counter,int id){
    for(int i=0;i<1000;i++){
        counter.inrement();
    }
    std::cout<<"Thread"<<id<<"finished.Count:"<<counter.getCount()<<std::endl;
}

int main(){
    SharedCounter counter;
    std::vector<std::thread>threads;
    for(int i=0;i<5;++i){
        threads.emplace_back(worker,std::ref(counter),i);
        /*
            接收多个参数，包括线程函数，线程函数的参数以及其他可选参数
        */
    }
    for(auto& t:threads){
        t.join();
    }
    std::cout<<"Final count"<<counter.getCount()<<std::endl;
    return 0;
}
