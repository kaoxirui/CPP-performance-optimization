#include<atomic>
#include<memory>
#include<iostream>

template<typename T>
struct Node
{
    std::shared_ptr<T>data;//共享指针存储节点的数据
    std::atomic<Node<T>*>next;//原子指针，用于存储链表中的下一个节点的地址，使用std::atomic可以确保指针的访问和修改是线程安全的
                                //表示指向Node<T>对象的原子指针。这意味着指针可以被多个线程安全访问和修改，不会出现竞争条件
    Node(T value):data(std::make_shared<T>(value)),next(nullptr){}
};

template<typename T>
class LockFreeQueue
{
private:
    std::atomic<Node<T>*>head;
    std::atomic<Node<T>*>tail;
public:
    LockFreeQueue():head(new Node<T>(T())),tail(head.load()){}
    ~LockFreeQueue(){
        while(head.load()!=tail.load()){//指向同一个节点。
                                        //head.load()：读取head原子指针当前指向的Node<T>*对象 
                                        //这个操作是原子的，能确保在多线程环境下的线程安全性
            Node<T>*oldHead=head.load();//获取当前指针指向的节点
            head.store(oldHead->next.load());//将head原子指针更新为刚刚读取到的oldHead->next
            delete oldHead;
        }
        delete tail.load();
    }

    void Enqueue(T value){
        Node<T>* newNode=new Node<T>(value);
        newNode->next.store(nullptr);

        while(true){
            Node<T>* oldTail=tail.load();//读取tail指针值
            Node<T>* next=oldTail->next.load();//读取oldtail节点的next指针值
            if(oldTail==tail.load()){//检查oldtail时候仍然指向当前tail节点，如果不是，说明tail指针已被其他线程更新来，需要重新尝试
                if(next==nullptr){//next指针为nullptr，说明oldtail是队列的最后一个节点
                    if(oldTail->next.compare_exchange_strong(next,newNode)){//尝试使用CAS（compare-and-swap）操作将oldtail
                                                                            //的next指针更新为新创建的newnode
                                                                            //如果成功说明节点已经成功添加到节点中
                        tail.compare_exchange_strong(oldTail,newNode);//尝试使用 CAS 操作将 tail 指针更新为新添加的 newNode。
                                                                    //如果成功,说明入队操作完成。
                        return;
                    }
                }
                else{
                    tail.compare_exchange_strong(oldTail,next);//如果next指针不为nullptr，说明oldtail已经不再是最后一个节点，
                                                                //需要先更新tail为next

                }
            }
        }
    }

    std::shared_ptr<T>Dequeue(){
        while(true){
            Node<T>* oldHead=head.load();//获取头指针
            Node<T>* oldTail=tail.load();//获取尾指针
            Node<T>* next=oldHead->next.load();//头指针的next指针值存储在next变量中
            
            if(oldHead==head.load()){//检查 oldHead 是否仍然指向当前的 head 节点。
                                    //如果不是,说明 head 指针已经被其他线程更新了,需要重新尝试。
                if(oldHead==oldTail){//指向同一个节点，说明队列为空或只有一个节点
                    if(next==nullptr){
                        return std::shared_ptr<T>();//队列为空，返回一个空的std::shared_ptr<T>()
                    }
                    tail.compare_exchange_strong(oldTail,next);
                    //如果 next 不为 nullptr，说明队列只有一个节点,需要更新 tail 指针为 next。
                }
                else{
                    if(head.compare_exchange_strong(oldHead,next)){
                        return oldHead->data;//返回从头部节点中取出的数据值。
                    }
                }
            }
        }
    }
};

int main(){
    LockFreeQueue<int>queue;
    queue.Enqueue(1);
    queue.Enqueue(2);
    queue.Enqueue(3);

    std::shared_ptr<int>result1=queue.Dequeue();
    if(result1){
        std::cout<<"Dequeued: "<<*result1<<std::endl;
    }else{
        std::cout<<"queue is empty"<<std::endl;
    }

    std::shared_ptr<int>result2=queue.Dequeue();
    if(result2){
        std::cout<<"Dequeued: "<<*result2<<std::endl;
    }else{
        std::cout<<"queue is empty"<<std::endl;
    }

    std::shared_ptr<int>result3=queue.Dequeue();
    if(result3){
        std::cout<<"Dequeued: "<<*result3<<std::endl;
    }else{
        std::cout<<"queue is empty"<<std::endl;
    }
    return 0;
}