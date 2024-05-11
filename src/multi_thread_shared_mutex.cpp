/*
    * 头文件#include<shared_mutex>
    * 用于实现共享和独占访问的互斥锁
    * 提供了更加灵活的机制，允许多个线程在共享模式下读取数据，
      但只允许单个线程在独占模式下写入或修改数据
    * 与std::mutex相比，具有以下额外特性“
        1. 多个线程可以同时共享模式持有锁，允许并发读取操作
        2. 只有一个线程可以以独占模式持有锁，允许写入或修改操作
        3. 当一个线程以独占模式持有锁时，其他线程无法以共享模式持有锁，
            他们必须等待独战模式的线程释放锁
        4. 当一个线程以共享模式持有锁时，其他线程可以以共享模式持有锁，允许并发读取操作

    lock：锁定互斥，若互斥不可用则阻塞。

try_lock：尝试锁定互斥，若互斥不可用则返回。
unlock：解锁互斥。
lock_shared：以共享模式锁定互斥，若互斥不可用则阻塞。
try_lock_shared：尝试以共享模式锁定互斥，若互斥不可用则返回。
unlock_shared：解锁以共享模式锁定的互斥。

推荐使用方法：
    当使用共享模式时，配合std::shared_lock
    当使用独占模式时，配合 std::lock_guard，std::unique_lock 或者 std::scoped_lock 使用。
*/

#include<iostream>
#include<shared_mutex>
#include<unordered_map>
#include<string>
#include<thread>
#include<vector>
#include<boost/thread/shared_mutex.hpp>
class SharedData{
    private:
        mutable boost::shared_mutex mutex_;
        std::unordered_map<std::string,std::string>data_;
    public:
        void read(const std::string& key)const{
            boost::shared_lock<boost::shared_mutex>lock(mutex_);
            auto it=data_.find(key);
            if(it!=data_.end()){
                std::cout<<"Thread:"<<std::this_thread::get_id()<<"read"<<it->second<<std::endl;
            }else{
                std::cout<<"Key not find"<<std::endl;
            }
        }

        void write(const std::string& key,const std::string& value){
            boost::unique_lock<boost::shared_mutex>lock(mutex_);
            data_[key]=value;
            std::cout<<"Thread:"<<std::this_thread::get_id()<<"wrote"<<value<<std::endl;
        }
};

void reader(SharedData& data,const std::string& key){
    for(int i=0;i<10;i++){
        data.read(key);
    }
}

void writer(SharedData& data,const std::string& key,const std::string& value){
    for(int i=0;i<5;++i){
        data.write(key,value);
    }
}

int main(){
    SharedData data;
    std::vector<std::thread>readerThreads,writerThreads;
    for(int i=0;i<2;++i){
        writerThreads.emplace_back(writer,std::ref(data),"key1","value"+std::to_string(i));
    }
    for(int i=0;i<3;++i){
        readerThreads.emplace_back(reader,std::ref(data),"key1");
    }

    for(auto& t:readerThreads){
        t.join();
    }
    for(auto& t:writerThreads){
        t.join();
    }
    return 0;
}