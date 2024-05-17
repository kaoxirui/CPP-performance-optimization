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

`std::function<int(int,int)`>定义一个返回类型为int，形参类型为int,int的函数对象

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



# 左值引用和右值引用

https://blog.csdn.net/m0_59938453/article/details/125858335?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171573914416800185874809%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fblog.%2522%257D&request_id=171573914416800185874809&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~blog~first_rank_ecpm_v1~rank_v31_ecpm-1-125858335-null-null.nonecase&utm_term=%E5%B7%A6%E5%80%BC&spm=1018.2226.3001.4450

## 左值和右值

### 左值（出现在赋值语句左边的表达式）

左值是一个表示数据的表达式，比如：变量名，解引用的指针变量。一般的，我们可以获取他的地址和对他复制，但被const修饰后的左值，不能给他赋值，但是仍然可以获取他的地址

**总体而言，可以取地址的对象就是左值**

```c++
int a=3;
int* p=&a;
*p;
const int b=2;
//a,p,*p,b都是左值
```

### 右值（出现在赋值语句的右边）

右值也是一个表示数据的表达式，比如：字面常量，表达式返回值，传值返回函数的返回值（是传值返回，而不是传引用返回），**右值不能出现在赋值符号的坐标且不能获取地址**

**总体而言，不可以取地址的对象就是右值**

```c++
double x=1.3,y=3.8;
//一下几个是常见的右值
10;
x+y;
fmin(x+y);
```

>以下写法均不能通过编译：
>
>    1. 10 = 4;、x + y = 4;、fmin(x, y) = 4;，VS2015 编译报错：error C2106: “=”: 左操作数必须为左值。原因：右值不能出现在赋值符号的左边。
>```
>2. &10;、&(x + y);、&fmin(x, y);，VS2015 编译报错：error C2102: “&” 要求左值。原因：右值不能取地址。
>```

## 左值引用和右值引用

### 左值引用

左值引用就是对左值的引用，给左值区别名

```c++
int& ra=a;
int*& rp=p;
int& r=*p;
const int& rb=b;
```

### 右值引用

右值引用就是对右值的引用，给右值取别名。

> 右值引用的表示是在具体的变量名称后家两个&，如`int&& rr=4`

```C++
// 以下几个是对上面右值的右值引用
int&& rr1 = 10;
double&& rr2 = x + y;
double&& rr3 = fmin(x, y);
```



> 注意：
>
> 右值引用引用右值，会使右值被存储到特定的位置
>
> 也就是说，**右值引用变量其实是左值**，可以对他取地址和赋值（const右值引用变量可以取地址但不可以赋值，因为 const 在起作用）。
>
> 当然，取地址是指取变量空间的地址（右值是不能取地址的）。
>
> 比如：
>
>     double&& rr2 = x + y;
>     &rr2;
>     rr2 = 9.4;
>     右值引用 rr2 引用右值 x + y 后，该表达式的返回值被存储到特定的位置，不能取表达式返回值 x + y 的地址，但是可以取 rr2 的地址，也可以修改 rr2 。
>     const double&& rr4 = x + y;
>     &rr4;
>     可以对 rr4 取地址，但不能修改 rr4，即写成rr4 = 5.3;会编译报错。

## 对比与总结

### 左值引用总结

1. 左值引用只能引用左值，不能直接引用右值
2. 但是`const左值引用`既可以引用左值，也可以引用右值

```c++
//左值引用只能引用左值
int t=8;
int& rt1=t;
//int& rt2 = 8;  // 编译报错，因为8是右值，不能直接引用右值

//但是const左值引用可以引用左值和右值
const int& rt3=t;
const int& rt4=8;
const double& r1=x+y;
const double& r2=fmin(x+y);
```

>问：为什么const左值引用也可以引用右值？
>答：在 C++11标准产生之前，是没有右值引用这个概念的，当时如果想要一个类型既能接收左值也能接收右值的话，需要用const左值引用，比如标准容器的 push_back 接口：void push_back (const T& val)。
>也就是说，如果const左值引用不能引用右值的话，有些接口就不好支持了。

> gpt:
>
> ```C++
> void foo(const std::string& s) {
>     // 处理字符串 s
> }
> ```
>
> 你可以使用 `foo("hello")` 这样的右值来调用该函数,因为 `const` 左值引用可以绑定到右值。如果使用非 `const` 左值引用,就无法这样调用函数,需要先创建一个临时对象。
>
> 总之,`const` 左值引用可以引用右值,是因为它可以延长临时对象的生命周期、避免额外的拷贝操作,并且增加函数的灵活性和可复用性。这是 C++ 中一个非常有用的特性。

### 右值引用总结：

1. 右值引用只能引用右值，不能直接引用左值。
2. 但是右值引用可以引用被`move`的左值。

> std::moce(C++11)，作用是将一个左值强制转化为右值，以实现移动语义，左值被move后变为右值，于是右值引用可以引用

```c++
//右值引用只能引用右值
int&& rr1 = 10;
double&& rr2 = x + y;
const double&& rr3 = x + y;

int t = 10;
//int&& rrt = t;  // 编译报错，不能直接引用左值
// 2.但是右值引用可以引用被move的左值
int&& rrt = std::move(t);
int*&& rr4 = std::move(p);
int&& rr5 = std::move(*p);
const int&& rr6 = std::move(b);
```

> gpt:
>
> 使用 `std::move()` 可以将一个左值转换为右值引用,从而让编译器知道我们希望对这个对象执行移动操作,而不是拷贝操作。这样做的好处包括:
>
> 1. **避免不必要的拷贝**:
>    - 当函数接受一个右值引用参数时,我们可以使用 `std::move()` 将左值传递给它,从而触发移动构造而不是拷贝构造。这可以显著提高性能。
> 2. **简化代码**:
>    - 使用 `std::move()` 可以让我们在代码中明确地表达我们的意图,即我们希望对一个对象执行移动操作而不是拷贝操作。这可以使代码更加清晰易懂。
> 3. **支持 RVO 和 NRVO**:
>    - 当我们返回一个右值引用时,编译器可以进行返回值优化(RVO 和 NRVO),进一步提高性能。使用 `std::move()` 可以帮助编译器识别这些优化的机会。

## 左值引用使用场景及意义

```C++
// 1.左值引用做参数
void func1(string s)
{...}

void func2(const string& s)
{...}


int main()
{
	string s1("Hello World!");
	func1(s1);  // 由于是传值传参且做的是深拷贝，代价较大
	func2(s1);  // 左值引用做参数减少了拷贝，提高了效率
	
	return 0;
}

```

传值传参和传值返回都会产生拷贝，有的甚至是深拷贝，代价很大。而左值引用的实际意义在于做参数和做返回值都可以减少拷贝，从而提高效率。

* 短板

左值引用虽然较完美地解决了大部分问题，但对于有些问题仍然不能很好地解决。

当对象出了函数作用域以后仍然存在时，可以使用左值引用返回，这是没问题的。

```c++
string& operator+=(char ch)
{
	push_back(ch);
	return *this;
}
```

但当对象（对象是函数内的局部对象）出了函数作用域以后不存在时，就不可以使用左值引用返回了。

```C++
string operator+(const string& s, char ch)
{
	string ret(s);
	ret.push_back(ch);
	return ret;
}

// 拿现在这个函数来举例：ret是函数内的局部对象，出了函数作用域后会被析构，即被销毁了
// 若此时再返回它的别名（左值引用），也就是再拿这个对象来用，就会出问题
```

## 右值引用

为解决上述传值返回的拷贝问题，，C++11标准就增加了**右值引用**和**移动语义**。

### 移动语义

将一个对象中的资源**移动**到另一个对象（资源控制权的转移）。移动语义是一种可以将资源（如内存）从一个对象转移到另一个对象的方式，而不是进行资源的复制。移动操作通常比复制操作更高效，对于大型的对象（如容器/字符串等）可以带来很大的优势



可以将对象从左值变为右值（变成右值可以赋值来），避免拷贝构造，只是将对象状态或者所有权从一个对象转移到另一个对象，没有涉及内存的搬迁或者内存拷贝，从而极大地提高代码效率。

**但需要注意，使用 std::move 后原对象（如上面的obj6）的状态是不确定的，不应再对其进行操作，否则程序运行时可能出现Segmentation fault (core dumped)报错！！！**

#### 移动构造

转移参数右值的资源来构造自己

```cpp
// 这是一个模拟string类的实现的移动构造
string(string&& s)
	:_str(nullptr)
	, _size(0)
	, _capacity(0)
{
	swap(s);
}
```

拷贝构造函数和移动构造函数都是构造函数的重载函数，所不同的是：

1. 拷贝构造函数的参数是 const左值引用，接收左值或右值；
2. 移动构造函数的参数是右值引用，接收右值或被 move 的左值。



### 完美转发（perfect forward）

* 引入原因：

  在此之前，需要知道什么是万能引用

  **确定类型**的&&表示右值引用（int&&，string&&）

  但函数模板中的&&不表示右值引用，而是万能引用，模板类型必须通过推断才能确定，其接受**左值**后才会被推导为**左值引用**，接受**右值**后会被推导为**右值引用**

注意区分右值引用和万能引用：下面的函数的 T&& 并不是万能引用，因为 T 的类型在模板实例化时已经确定。

```c++
template<typename T>
class A
{
	void func(T&& t);  // 模板实例化时T的类型已经确定，调用函数时T是一个确定类型，所以这里是右值引用
};
```

让我们通过下面的程序来认识万能引用：

```C++
template<typename T>
void f(T&& t)  // 万能引用
{
	//...
}

int main()
{
	int a = 5;  // 左值
	f(a);  // 传参后万能引用被推导为左值引用
	
	const string s("hello");  // const左值
	f(s);  // 传参后万能引用被推导为const左值引用
	
	f(to_string(1234));  // to_string函数会返回一个string临时对象，是右值，传参后万能引用被推导为右值引用

	const double d = 1.1;
	f(std::move(d));  // const左值被move后变成const右值，传参后万能引用被推导为const右值引用
	
	return 0;
}
```

```c++
void Func(int& x) {	cout << "左值引用" << endl; }

void Func(const int& x) { cout << "const左值引用" << endl; }

void Func(int&& x) { cout << "右值引用" << endl; }

void Func(const int&& x) { cout << "const右值引用" << endl; }


template<typename T>
void f(T&& t)  // 万能引用
{
	Func(t);  // 根据参数t的类型去匹配合适的重载函数
}

int main()
{
	int a = 4;  // 左值
	f(a);
	
	const int b = 8;  // const左值
	f(b);
	
	f(10); // 10是右值
	
	const int c = 13;
	f(std::move(c));  // const左值被move后变成const右值
	
	return 0;
}

```

运行程序后，我们本以为打印的结果是：
 左值引用
 const左值引用
 右值引用
 const右值引用

但实际的结果却是

 左值引用
 const左值引用

 左值引用
 const左值引用

其实在本文的前面已经讲过了，**右值引用变量其实是左值**，所以就有了上面的运行结果。

> 具体解释：
>
>     f(10);
>     10是右值，传参后万能引用被推导为右值引用，但该右值引用变量其实是左值，因此实际调用的函数是void Func(int& x)。
>     f(std::move(c));
>     const左值被move后变成const右值，传参后万能引用被推导为const右值引用，但该const右值引用变量其实是const左值，因此实际调用的函数是void Func(const int& x)。

也就是说，**右值引用失去了右值的属性**。

但我们希望的是，在传递过程中能够保持住它的原有的左值或右值属性，于是 C++11标准提出完美转发。

#### 概念

完美转发是指在函数模板中，完全依照模板的参数类型，将参数传递给当前函数模板中的另外一个函数。

因此，为了实现完美转发，除了使用万能引用之外，我们还要用到std::forward（C++11），它在传参的过程中保留对象的原生类型属性。

这样右值引用在传递过程中就能够保持右值的属性。
```C++
void Func(int& x) { cout << "左值引用" << endl; }

void Func(const int& x) { cout << "const左值引用" << endl; }

void Func(int&& x) { cout << "右值引用" << endl; }

void Func(const int&& x) { cout << "const右值引用" << endl; }


template<typename T>
void PerfectForward(T&& t)  // 万能引用
{
	Func(std::forward<T>(t));  // 根据参数t的类型去匹配合适的重载函数
}

int main()
{
	int a = 4;  // 左值
	PerfectForward(a);

	const int b = 8;  // const左值
	PerfectForward(b);

	PerfectForward(10); // 10是右值

	const int c = 13;
	PerfectForward(std::move(c));  // const左值被move后变成const右值

	return 0;
}

```

**实现完美转发需要用到万能引用和 std::forward 。**



# 信号量

`sem_t`是POSIX线程库提供的一个信号量类型，用于同步多个线程对共享资源的访问。它可以用于线程之间的同步，进程之间的同步以及进程和线程之间的同步

* `sem_t`类型的变量被称为信号量，它通常是一个非负整数，并且具有以下两个基本操作：

*  `sem_wait()`：请求信号量，如果信号量的值大于0，则将其减1，并继续执行，否则当前线程将阻塞知道信号量的值大于0

* `sem_post()`：释放信号量，将信号量的值加1

   pthread_mutex_t 是 POSIX（Portable Operating System Interface）线程库中的数据类型，通常简称为 Pthreads。它提供了一种标准化的方式，用于多线程程序同步访问共享资源，以防止数据损坏和竞争条件。

   pthread_mutex_t 实际上是一个互斥锁（mutex）对象。它用于创建和管理多线程程序中的互斥锁。互斥锁是同步原语，允许多个线程协调工作，确保在任何时刻只有一个线程可以访问关键代码段或共享资源。这可以防止在多个线程同时尝试访问相同资源时发生冲突和数据损坏。



# 互斥量与锁

## 总结

1. 常用的互斥对象：`std::mutex`（互斥对象），`std::shared_mutex`(读写互斥锁)
2. 三个用于代替互斥对象的成员函数，管理互斥对象的锁（都是构造加锁，析构解锁）：`std::lock_guard`用于管理`std::mutex`，`std::unique_lock`。`std::shared_lock`管理`std::shared_mutex`

## lock_guard

lock_guard：这是C++11中一个简单的 RAII（Resource Acquisition Is Initialization）风格的锁，用于在作用域内自动管理互斥量的锁定和解锁。当 lock_guard 对象被创建时，它会自动锁定互斥量，当对象离开作用域时，它会自动解锁互斥量。lock_guard 不支持手动锁定和解锁，也不支持条件变量。

lock_guard取代了mutex的lock()和unlock();

unique_lock比lock_guard灵活很多，效率上差一点，内存占用多一点。

```c++
std::mutex mtx;
std::lock_guard<std::mutex> lck (mtx);  // 构造时加锁，离开作用域析构解锁
```

## unique_lock

https://blog.csdn.net/u012507022/article/details/85909567?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522171578033016800188583903%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=171578033016800188583903&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~top_positive~default-1-85909567-null-null.142^v100^pc_search_result_base4&utm_term=unique_lock&spm=1018.2226.3001.4187

unique_lock：这是C++11中一个更灵活的锁，它允许手动锁定和解锁互斥量，以及与条件变量一起使用（是lock_guard的进阶版）。与 lock_guard 类似，unique_lock 也是一个 RAII 风格的锁，当对象离开作用域时，它会自动解锁互斥量。unique_lock 还支持延迟锁定、尝试锁定和可转移的锁所有权。
## shared_lock

shared_lock：C++14引入的锁，这是一个用于共享互斥量（如 std::shared_mutex 或 std::shared_timed_mutex）的锁，允许多个线程同时读取共享数据，但在写入数据时仍然保证互斥。shared_lock 也是一个 RAII 风格的锁，当对象离开作用域时，它会自动解锁共享互斥量。shared_lock 支持手动锁定和解锁，以及尝试锁定。
共享锁，也叫多线程锁，当data被一个线程读取时，仍允许其它线程读取数据，但是不能写入操作。



* 头文件 #include <shared_mutex>。
* 用于实现共享和独占访问的互斥锁。
* 提供了一种更加灵活的机制，允许多个线程在共享模式下读取数据，但只允许单个线程在独占模式下写入或修改数据。
* 与 std::mutex 相比，具有以下额外特性：

    多个线程可以同时以共享模式（shared mode）持有锁，允许并发读取操作。
    只有一个线程可以以独占模式（exclusive mode）持有锁，允许写入或修改操作。
    当一个线程以独占模式持有锁时，其他线程无法以共享模式持有锁，它们必须等待独占模式的线程释放锁。
    当一个线程以共享模式持有锁时，其他线程可以以共享模式持有锁，允许并发读取操作。
### 成员函数

* lock：锁定互斥，若互斥不可用则阻塞。
* try_lock：尝试锁定互斥，若互斥不可用则返回。
* unlock：解锁互斥。
* lock_shared：以共享模式锁定互斥，若互斥不可用则阻塞。
* try_lock_shared：尝试以共享模式锁定互斥，若互斥不可用则返回。
* unlock_shared：解锁以共享模式锁定的互斥。

## shared_mutex

C++17中引入std::shared_mutex
 std::shared_mutex用于管理可转移和共享所有权的互斥对象，适用场景比较特殊：一个或多个读线程同时读取共享资源，且只有一个写线程来修改这个资源，这种情况下才能从shared_mutex获取性能优势。.



# 智能指针

## shared_ptr

资源可以被多个指针共享，它使用计数机制来表明资源被几个指针共享。可以通过成员函数use_count()来查看资源的所有者个数。出了可以通过new来构造，还可以通过传入auto_ptr, unique_ptr,weak_ptr来构造。当我们调用release()时，当前指针会释放资源所有权，计数减一。当计数等于0时，资源会被释放。

```c++
int main()
{
    shared_ptr<Test> ptest(new Test("123"));
    shared_ptr<Test> ptest2(new Test("456"));
    cout<<ptest2->getStr()<<endl;
    cout<<ptest2.use_count()<<endl;
    ptest = ptest2;//"456"引用次数加1，“123”销毁
    ptest->print();
    cout<<ptest2.use_count()<<endl;//2
    cout<<ptest.use_count()<<endl;//2
    ptest.reset();
    ptest2.reset();//此时“456”销毁
    cout<<"done !\n";
    return 0;
}
```



# 原子操作

std::atomic

## 相关接口

`atomic a=val`以val为a的初始值（这个不是atomic的操作）

`a.store(val)`赋值val（返回void）

`a.load()`返回a的copy

`a.exchange(val)`赋值val并返回旧值a的拷贝

`a.compare_exchange_strong(exp,des)`cas操作

`a.compare_exchange_weak(exp,des)`weak cas操作

## CAS(比较并替换)接口的使用

compare_exchange_strong(T& expected,T desired,

std::memory_order success,

std::memory_order failure) 和

 compare_exchange_weak(T& expected,T desired,

std::memory_order success,

std::memory_order failure)

这两个接口都是CAS操作（compare and swap）。CPU常常提供这个atomic操作用以比较“某内存内容”和“某给定值”，并且唯有在他们相同时才将该内存区内容更新为另一给定的新值。这可以保证新值是根据最新信息计算出来的。

这些函数会原子地执行这些操作：**对比对象的值和expected的值，如果相等就会把对象的值替换为desired的值；否则，就把expected的值更新为原子对象的最新值**

>expected：是一个引用，表示所期待的原子对象的值。如果比对失败，就把expected的值更新为原子对象的最新值
>
>desired：如果对比成功，保存到原子对象中的值
>
>success：指定对比成功时的执行读-修改-写操作的内存同步顺序
>
>failure：  指定对比失败时的执行读-修改-写操作的内存同步顺序；不能是std::memory_order_release or std::memory_order_acq_rel

 如果修改成功就返回true，否则返回false。

```C++
#include<atomic>
#include<iostream>

std::atomic<int> ai;

int tst_val=4;
int new_val=5;
bool exchanged=false;

void valsout()
{
    std::cout << "ai= " << ai
	      << "  tst_val= " << tst_val
	      << "  new_val= " << new_val
	      << "  exchanged= " << std::boolalpha << exchanged
	      << "\n";
}

int main(){
    ai=3;
    valsout();
    
    //tst_val!=ai ==> tst_val is modified
    //这个时候由于ai为3，tst_val为4，所以对比就会失败，那么tst_val就会被修改为3
    exchanged=ai.compare_exchange_strong(tst_val,new_val);
    valsout();
    //这个时候对比就会成功,那么ai的就会被修改为new_val
    //tst_val==ai ==> ai is modified
    exchanged=ai.compare_exchange_strong(tst_val,new_val);
    valsout();
}
```

ai= 3  tst_val= 4  new_val= 5  exchanged= false
ai= 3  tst_val= 3  new_val= 5  exchanged= false
ai= 5  tst_val= 3  new_val= 5  exchanged= true
