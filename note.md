# 线程池

## 应用场景

* 在进行创建线程任务时，如果需要频繁的创建线程，销毁线程，这样会极大降低效率，因为创建线程也需要时间，一个完整的线程
  处理时间包括线程的创建时间，运行时间，销毁时间

* 对于频繁创建线程的业务场景，可以预先创建多个线程，在创建线程任务时，可以直接将线程函数添加到预先创建的线程中，这样就可以避免多次创建线程的时间，提高代码运行效率

## 设计思路

设计多线程主要实现的功能有：
1. 预设创建线程的数量

2. 存储运行一定数量线程任务容器变量（可以是vector或其他容器，元素类型是std::thread），该容器一直检测任务队列中是否有待执行的任务，有则取出，无则等待

3. 用于存储运行任务的变量Task（数据类型是queue，数据类型是std::function<T>）,改变量用于存储待运行线程函数func

# 异步编程Futures

Futures功能是并发编程机制，旨在简化多线程编程和异步操作的处理。Futures提供了一种在一个线程中计算值或执行任务，并在另一个线程中获取结果的办法。

核心组件是`std::future`和`std::promise`。`std::future`对象与`std::promise`对象之间共享一个状态。通过`sd::promise`对象设置的值可以通过与其他关联的`std::future`对象来获取，这样就可以使一个线程使用`std::promise`对象来设置异步操作的结果，而另一个线程使用对应的`std::future`对象来获取结果

## std::future
`std:future`提供了一种访问异步操作结果的机制，通过std::async,std::packaged_task或std::promise创建的异步操作可以向该异步操作的创建者提供一个std::future对象。异步操作的创建者可以使用各种方法来查询、等待或从std::future中提取值。

* std::future::get获取结果，如果调用过程中，任务尚未完成，则主线程阻塞至任务完成。

* std::future::wait_for等待结果返回，wait_for可设置超时时间，如果在超时时间之内任务完成，则返回std::future_status::ready状态；如果在超时时间之内任务尚未完成，则返回std::future_status::timeout状态。
  

## std::async()
模板函数`std::async`异步地运行函数f（可能在一个独立的线程中，该线程可能是线程池的一部分），并返回一个`std::future`，最终将保存该函数调用的结果。它可以根据系统情况自动选择是独立启动一个线程运行，还是在对应的future调用get时运行；也可以由调用者指定任务调用策略。

任务调用策略：async(A,B,C...)

* 其中这个A是

    std::launch::async; 开启一个独立线程执行任务
    std::launch::deferred; 表示该函数会延迟启动，直到future上调用wait()或get()

* B和C：分别是需要传入的函数和函数的参数，和thread一样，函数的参数是通过副本传入，如果需要传入引用则需要使用std::ref类。

# 可变参数模板：template<class... Args>

常见的模板类似
```c++
template<class T>
void show_list(T value)
```

在这个定义中，有两个参数列表，模板参数列表只包含T，函数参数列表只包含value，将模板特化的过程show_list(2.51)就是将模板参数列表T换成double，将函数参数列表的value换成2.15

C++11中提供了一个用省略号表示的元运算符，可以声明模板参数包（类型列表）和函数参数包（值列表），其语法如下
```c++
template<class... Args>
void show_list(Args... args)
```

其中Args是一个模板参数包，与T的区别在与T与一种类型匹配，Args与若干个类型匹配（包括0）。与普通模板相同的是，Args中的类型列表与args的值列表匹配，无论是类型还是数量。

* ...的作用

1. 声明一个参数包，这个参数包中可以包含0到任意个模板参数

2. 在模板定义的右边，可以将参数包展开成一个一个独立的参数

* 那么如何访问这些包的内容呢？

C++11规定将省略号放在函数参数包名的右边可以将参数包展开，也就是说args...代表了函数的第一个参数。到这里可以使用递归来将函数的所有参数展开。首先来看一种错误的做法：

```c++
template<class... Args>
void show_list(Args... args){
  show_list(args...);
}
```

当调用函数`show_list(5,'L',1.1)`的时候5,'L',1.1被封装到函数参数包args中，而int,char,double则被封装到Args中，但这种做法函数调用与原来的函数始终相同，因此造成一个无限递归。于是想到一种改进的理念，对函数参数列表的第一项进行处理，再将剩下的内容传递给递归调用，知道传递的函数参数列表为空，这样来实现它：

```c++
template<class T,class... Args>
void show_list(T value,Args... args){
  cout<<value<<endl;
  show_list(args...);
}
```

这里的T和value表示函数第一个参数的类型和值。其他参数依旧放到Args和args中，每次递归调用都将显示一个值，并传递减少了一个参数的列表包args给下一次调用，直到列表为空为止。比如调用show_list(5,‘L’,1.1)：第一次调用导致T为int，vlaue为5，剩下的参数类型(char，double)放入Args中，(‘L’,1,1)放入args中，下次调用相当于执行show_list(‘L’,1.1)；

完整版：
```c++
void show_list(){
  cout<<"empty"<<endl;
}

template<class T,class... Args>
void show_list(T value,Args... args){
  cout<<value<<endl;
  show_list(args...);
}
```

```c++
template<typename T,typename... Types>
```
https://oorik.blog.csdn.net/article/details/127292419?spm=1001.2101.3001.6650.1&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-1-127292419-blog-127080405.235%5Ev43%5Epc_blog_bottom_relevance_base9&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-1-127292419-blog-127080405.235%5Ev43%5Epc_blog_bottom_relevance_base9&utm_relevant_index=2

对于可变参数模板来说...可接纳的模板参数个数是0个及以上的任意数量，也就是说“... Types”可以接受0到任意的参数

因为多了个T类型的参数，所以调用它的时候最少也得接纳一个参数，因为...Types是0到任意

*简单的实例化代码如下*

>**模板参数列表中**：声明一个名为T的类型参数，和一个名为Args的模板参数包（这个包表示零个或多个额外的类型参数）
>
>**函数参数列表中**：声明一个const&类型的参数，指向T的类型，还包含一个名为reset的函数参数包（这个包表示零个或多个函数参数）
>
>```c++
>//Args是一个模板参数包；reset是一个函数参数包
>//Args表示零个或多个模板类型参数
>//reset表示零个或多个函数参数
>template<typename T,typename... Args>
>void fun(const T& t,const Args&... reset){
>    //...
>}
>```
>
>给出基本调用
>
>```c++
>int main() {
> 
>		int i = 1;
>		double d = 3.30;
>		string s = "happy birthday";
> 
>		fun(i, s, 55, d); //包含三个参数
>		fun(s, 55, "hello"); //包含二个参数
>		fun(d, s);        //包含一个参数
>		fun("hello");       //空包，传的其实是T类型的参数
>		fun(10);      //空包，传的其实是T类型的参数
> 
>		return 0；	
>}
>```
>
>```c++
>//第一个T的类型从第一个实参推断出来，剩余的实参从提供的参数包中推断出
>//第一个参数都是T推演过来的，后面都是...Args推演过来的
>		void fun(const int&, const string&, const int&, const double&);
>		void fun(const string&, const int&, const char[6]&);
>		void fun(const double&, const string&);
>		void fun(const char[6]&);
>		void fun(const int &);
>```

为什么模板定义的时候"..."在类型前，写函数参数列表的时候"..."在类型后了

```c++
void fun(){
//这里加一个函数是为了编译可以通过，否则编译期间调用就会找不到void fun<char[6],>(const T (&))可匹配的函数
}
template<typename T,typename... Args>
void fun(const T& t,const Args&... reset){
	cout<<t<<endl;
    fun(reset...);
}
```

1. 模板参数包

   第一个typename...  Args，写在template的<>中，因此在使用模板函数是可以指定任意类型（在main函数中的fun调用）

2. 函数参数类型包（扩展包）

   第二个Args... reset，用在fun函数的参数中，表示任意类型的任意参数

3. 函数参数包

   第三个reset...，用在fun函数的递归调用中，实际上是实参

1）省略号出现在形参名左侧，声明了一个参数包。使用这个参数包，可以绑定0个或多个模板实参给这个可变模板形参参数包。参数包也可以用于非类型的模板参数（也就是参数数量不固定，但是每个参数的类型是一样的，即将Args reset替换成 int reset）。

```c++
fun<int,char,double,string>(1,'a',1.1,"hi");
```

2）省略号出现包含参数包的表达式右侧，则把这个参数包解开为一组实参，也就是rest...=reset1,reset2,reset3,....。这也就是为什么要递归调用



# 函数对象模板`function<T>`,通用函数适配器`std::bind`和`lambda`

## 函数对象模板

`std::function`和`std::bind`的头文件都为`#include<functional>`

1. function是一个模板，跟使用其他模板一样，只不过一般用来定义函数类型模板。function操作有以下形式：

   （1）function<T> f;        f是用来存储可调用对象的空function

   （2）function<T>f(nullptr)        显示构造一个空fucntioin

   （3）function<T>f(obj)             在f中存储可调用对象obj的副本   

   （4）f(args)                调用f中的对象，参数是args

上述T为函数类型，即返回类型和参数类型

`std::function<int(int,int)`定义一个返回类型为int，形参类型为int,int的函数对象

```c++
#include <iostream>
#include <functional> // 包含 std::function 的头文件

// 示例函数：加法函数
int add(int a, int b) {
    return a + b;
}

// 示例函数对象类
struct Multiply {
    int operator()(int a, int b) const {
        return a * b;
    }
};

int main() {
    // 使用 std::function 封装函数指针
    std::function<int(int, int)> func1 = add;
    int result1 = func1(2, 3); // 调用函数指针封装的函数
    std::cout << "Result1: " << result1 << std::endl;

    // 使用 std::function 封装函数对象
    Multiply mul;
    std::function<int(int, int)> func2 = mul;
    int result2 = func2(2, 3); // 调用函数对象封装的函数调用运算符
    std::cout << "Result2: " << result2 << std::endl;

    // 使用 std::function 封装 Lambda 表达式
    std::function<int(int, int)> func3 = [](int a, int b) {
        return a - b;
    };
    int result3 = func3(5, 3); // 调用 Lambda 表达式封装的函数
    std::cout << "Result3: " << result3 << std::endl;

    return 0;
}
```

## std::bind

std::bind可以看作一个通用的函数适配器，**它接受一个可调用对象，生成一个新的可调用对象来适应原对象的参数列表**

std::bind将可调用对象与其参数一起进行绑定，绑定后的结果可以使用std::function保存。std::bind主要有以下两个作用：

- **将可调用对象和其参数绑定成一个仿函数**；
- **只绑定部分参数，减少可调用对象传入的参数**。

调用bind的一般形式：

```c++
auto newCallable=bind(callable,arg_list);
```

即当调用newCallable时，会调用callable，并传给它arg_list中的参数

```c++
#include <iostream>
#include <functional>

class A {
public:
    void fun_3(int k,int m) {
        std::cout << "print: k = "<< k << ", m = " << m << std::endl;
    }
};

void fun_1(int x,int y,int z) {
    std::cout << "print: x = " << x << ", y = " << y << ", z = " << z << std::endl;
}

void fun_2(int &a,int &b) {
    ++a;
    ++b;
    std::cout << "print: a = " << a << ", b = " << b << std::endl;
}

int main(int argc, char * argv[]) {
    //f1的类型为 function<void(int, int, int)>
    auto f1 = std::bind(fun_1, 1, 2, 3); 					//表示绑定函数 fun 的第一，二，三个参数值为： 1 2 3
    f1(); 													//print: x=1,y=2,z=3

    auto f2 = std::bind(fun_1, std::placeholders::_1, std::placeholders::_2, 3);
    //表示绑定函数 fun 的第三个参数为 3，而fun 的第一，二个参数分别由调用 f2 的第一，二个参数指定
    f2(1, 2);												//print: x=1,y=2,z=3
 
    auto f3 = std::bind(fun_1, std::placeholders::_2, std::placeholders::_1, 3);
    //表示绑定函数 fun 的第三个参数为 3，而fun 的第一，二个参数分别由调用 f3 的第二，一个参数指定
    //注意： f2  和  f3 的区别。
    f3(1, 2);												//print: x=2,y=1,z=3

    int m = 2;
    int n = 3;
    auto f4 = std::bind(fun_2, std::placeholders::_1, n); //表示绑定fun_2的第一个参数为n, fun_2的第二个参数由调用f4的第一个参数（_1）指定。
    f4(m); 													//print: a=3,b=4
    std::cout << "m = " << m << std::endl;					//m=3  说明：bind对于不事先绑定的参数，通过std::placeholders传递的参数是通过引用传递的,如m
    std::cout << "n = " << n << std::endl;					//n=3  说明：bind对于预先绑定的函数参数是通过值传递的，如n
    
    A a;
    //f5的类型为 function<void(int, int)>
    auto f5 = std::bind(&A::fun_3, &a, std::placeholders::_1, std::placeholders::_2); //使用auto关键字
    f5(10, 20);												//调用a.fun_3(10,20),print: k=10,m=20

    std::function<void(int,int)> fc = std::bind(&A::fun_3, a,std::placeholders::_1,std::placeholders::_2);
    fc(10, 20);   											//调用a.fun_3(10,20) print: k=10,m=20 

    return 0; 
}
```

* **预绑定的参数是以值传递的形式，不预绑定的参数要用std::placeholders(占位符)的形式占位，从_1开始，依次递增，是以引用传递的形式**；

* **std::placeholders表示新的可调用对象的第几个参数，而且与原函数的该占位符所在位置的进行匹配**；

* **bind绑定类成员函数时，第一个参数表示对象的成员函数的指针，第二个参数表示对象的地址，这是因为对象的成员函数需要有this指针**。并且编译器不会将对象的成员函数隐式转换成函数指针，需要通过&手动转换；

* **std::bind的返回值是可调用实体，可以直接赋给std::function**。

## lambda表达式

当函数体较小时，可定义lambda表达式。形式如下

`[capture list](parameter list)->return type{function body}`

* capture list为lambda表达式所在上下文环境中变量的捕获，可以捕获单个变量[var1]，也可以捕获整个变量[this];捕获的值用于{}函数体语句中的变量访问（如算术运算访问）。捕获列表方式有三种：值捕获[var1]、引用捕获[&var1]、值与引用捕获混合使用eg: [&,identifier_list]。

当然也可不用捕获lambda表达式所在上下文变量，即[]。注意书写时，捕获列表符号“[]”不能省略！！！

* (parameter list)->return type{function body}与其他普通函数一样，但是lambda必须使用尾置返回类型。

  (parameter list)的函数形参为空时，可以省略！！！

  return type函数返回类型可以推断时，可以省略！！！

```c++\
std::vector<int>vec(1,2,3,4,5);
std::transform(vec.begin(),vec.end(),vec.begin(),[](int i){return i<0?-1:i});
```



# 內联函数

函数调用是有时间和空间开销的。程序在执行一个函数之前需要做一些准备工作，要将实参、局部变量、返回地址以及若干寄存器都压入栈中，然后才能执行函数体中的代码；函数体中的代码执行完毕后还要清理现场，将之前压入栈中的数据都出栈，才能接着执行函数调用位置以后的代码。



如果函数体代码比较多，需要执行较长的时间，呢么函数调用机制占用的时间可以忽略；如果函数只有一两条语句，那么大部分时间都会花在函数调用机制上，这种时间开销不容忽视

C++提供一种高效率的方法，即在编译时将函数调用处用函数体替换，类似于c语言中的宏展开。**这种在函数调用处直接嵌入函数体的函数称为内联函数（Inline Function），又称内嵌函数或者内置函数。**
