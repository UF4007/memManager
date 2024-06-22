硬盘与内存统一管理库(Headonly)，同时有反射、序列化等功能。  
目前仅能用于Windows64/32位环境  
**用例见demo暨功能测试函数 mem::testmain()**  
此库不保证线程安全，按需自行加锁。  
## 各类型简介  
### memUnit => 基本内存单元  
- 用法：  
    - 继承memUnit类  
    - 实现一个构造函数anyClass(memManager* m):memUnit(m){}，**下文记为构造函数M**   
    - 实现纯虚函数void save_fetch(memPara para) override{}  
    - （可选）实现虚函数fetchInit(){},并标记isFetchInit为true  
    - GWPP(const WCHAR* key, Types& varST, memPara para);在save_fetch函数内部，填写想要管理的变量。具体见demo。

每个memUnit在创建时需指定一个memManager以隶属。memManager内部维护了一个列表，用以记录所有的memUnit。  
memUnit子类之间禁止成员组合，即memUnit类中禁止存在另一个memUnit。  
- 从硬盘中读取并构造时，执行顺序如下：  
    - 执行构造函数M  
    - 执行save_fetch，读取各变量  
    - 若isFetchInit为true，则其他memUnit在构造时，**每次**通过指针读取到此memUnit时，执行一次

### memManager => 内存单元管理器  
**一个此类，代表一个硬盘文件**  
- 用法：  
    - 继承memManager类  
    - 实现纯虚函数void save_fetch(memPara para) override{}

setUrl(const WCHAR* wcptr)设置文件路径  
upload()上传文件到内存  
download()下载文件到硬盘  
- 构造原理：
    - memManager执行自身的save_fetch()，并执行其中的GWPP()来检视指针指向的下属memUnit是否被构造。  
    - 若没被构造，则构造并执行其save_fetch；若构造过了，则直接赋指针。  
    - 如此嵌套，直到memManager的save_fetch()被执行完。  

因此，在上传或下载时，所有悬空于memManager之外的指针（内存单元）将会被忽略。  
不过，在析构时，所有记录的memUnit都会被析构，无论其是否悬空。  
memManager继承于memUnit。在内部记录所有隶属此类的memUnit。  
此外，memManager内部还存储了依赖文件表、入口表、出口表。（详见pEgress）  
### memPtr => 基本内存单元的智能指针  
智能指针，类似于shared_ptr。与之不同的是，此指针只能指向memUnit派生类。  
不同点之二：每个memUnit内部设置了一个指针，用于指回memPtr所创建的中间体。这么做有两个好处，1是可以主动release()掉智能指针；2是让原始指针与memPtr在一定程度上能够混用，而不会出现计数错误的问题。  
使用isFilled()和isEmpty()判断其是否为空。  
此指针不支持多态，因为在最终的存档中不包含类型信息，读取存档时，依靠的是此指针编译期的模板类型来正确进行反序列化。  
**memUnit中成员memPtr的指向，不能跨越这个memUnit的memManager。**  
### memVector => vector内存单元  
是一个同时继承了std::vector和memUnit的类。  
这个vector中保存的是memPtr<>，不是类实例。  
有template<> using pmemVector = memPtr<memVector<>>;  
### memStruct => 任意可存储结构体  
拥有save_fetch_struct()成员函数的任意结构体。  
save_fetch_struct中，使用GWPP_Struct()来填写想要管理的变量。  
### pVariant => 泛型智能指针  
模板内填写所有该指针可能指向的memUnit子类。  
本质上，保存了一个memPtr智能指针和一个表示类型的UINT。  
### pEgress => 跨越内存单元管理器的“出口”  
用于memManager之间的通信。  
pEgress<>模板中，填写想要指向的对方memManager类中成员memUnit的类名。  
本质是一个智能指针，指向一个内部类Egress。  
Egress记录了对方memManager的文件名、键名和类类型。  
在使用makeEIPair()后，会在自身memManager内创建一个Egress，并让pEgress指向这个Egress，并在对方memManager内创建一个Ingress，Ingress指向目标memUnit。这个Egress-Ingress Pair将拥有相同的键名和类型。  
使用getTarget()方法尝试通过Egress的文件名、键名和类型数据获取对方Ingress所指向的memUnit。  
此库维护了一个全局文件表（带锁），记录了当前加载的所有memManager。getTarget()中，Egress将会尝试从全局文件表中找到这个文件名。因此，调用getTarget()前，要确保对方memManager已经以某种形式载入到内存。    
getTarget()返回值意义见文件开头的宏定义MEM_系列。  
### pFunction -> 选调函数  
使用宏INITIALIZE_PFUNCTION()填写函数参数类型、分组号、所有的函数指针。   
参数类型与分组号引导pFunction模板实例化，所有的函数指针则用于初始化这个实例化模板的一个常量数组。  
本质上，pFunction只保存了一个表示类型的UINT。  
通过函数指针来赋值这个UINT；通过这个UINT来选择调用常量数组中的函数指针。  
### 具体细节、反射、序列化等其他内容见demo。  
