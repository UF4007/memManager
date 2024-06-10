Headonly库，硬盘与内存统一管理库，同时有反射、序列化等功能。  
目前仅能用于Windows64/32位环境  
**用例见demo暨功能测试函数 mem::testmain()**  
此库不保证线程安全，使用自行加锁。  
## 各类型简介  
### memUnit => 基本内存单元  
- 用法：  
    - 继承memUnit类  
    - 实现一个构造函数anyClass(memManager* m):memUnit(m){}，**下文记为构造函数M**   
    - 实现纯虚函数void save_fetch(memPara para) override{}  
    - （可选）实现虚函数fetchInit(){},并标记isFetchInit为true  
    - GWPP(const WCHAR* key, Types& varST, memPara para);在反射函数内部，填写想要管理的变量。具体见demo。

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

因此，在上传或下载时，所有悬空于memManager之外的指针将会被忽略。  
memManager继承了memUnit，并在内部记录了所有隶属此类的memUnit。  
除此之外，内部还存储了依赖文件表、入口表、出口表。  
### memPtr => 基本内存单元的智能指针  
智能指针，与shared_ptr不同的是，它可以主动release()。  
使用isFilled()和isEmpty()判断其是否为空。  
**memUnit中成员memPtr的指向，不能跨越这个memUnit的memManager。**  
### memVector => vector内存单元  
是一个同时继承了std::vector和memUnit的类。  
这个vector中保存的是memPtr<>，不是类实例。  
有template<> using pmemVector = memPtr<memVector<>>;  
### memStruct => 任意可存储结构体  
任意结构体，只要拥有save_fetch_struct(memUnit* mu, const WCHAR* key, memPara para)成员函数。  
save_fetch_struct中，使用GWPP_Struct(memUnit* mu, const WCHAR* key1, const WCHAR* key2, Types& var, memPara para)来填写想要管理的变量。  
### pGeneric => 泛型智能指针  
模板内填写所有该指针可能指向的memUnit子类。  
### pEgress => 跨越内存单元管理器的“出口”  
用于memManager之间的通信。  
本质是一个智能指针，指向一个Egress（内部类）。  
模板中，填写想要指向的其他memManager类中memUnit成员的类名。  
### 具体细节、反射、序列化等其他内容见demo。  
