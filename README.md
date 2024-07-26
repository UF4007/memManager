## memManager

[English](README-en.md)  
* 统一实现了C++结构体的内存RAII、磁盘存储、跨文件引用、静态反射、二进制序列化、json序列化。
* headonly库，方便使用。  
* 使用的库：rapidJson  
* 目前适用：Windows64/32，ESP32  

### 如何使用

#### 示例代码

```C++
#include "memManager/memManager.h"

struct testU : public mem::memUnit {

	//定义成员变量
	int id;
	std::string name;

	//实现纯虚函数save_fetch，把所有要保存的变量写进去
	void save_fetch(mem::memPara para) override {
		GWPP("id", id, para);
		GWPP("name", name, para);
	}

	//实现构造函数并传递memManager指针
	testU(mem::memManager* m) :memUnit(m) {}

	//权限宏(如果是public可以不写)
	MEM_PERMISSION
};

struct testM : public mem::memManager {

	//此库能正确序列化指针关系
	std::vector<mem::memPtr<testU>> vec;
	
	void save_fetch(mem::memPara para) override {
		GWPP("vec", vec, para);
	}
};

int main(){
	testM* a = new testM();

	a->vec.push_back(new testU(a));
	a->vec.push_back(new testU(a));

	a->vec[0]->id = 42;
	a->vec[0]->name = "Hello world";
	a->vec[1]->id = 36;

	a->setUrl("D:\\test");	//设置一个有效的目录
	a->download();

	testM* b = new testM();
	b->setUrl("D:\\test");
	b->upload();

	if (b->vec.size())
	{
		std::cout << b->vec[0]->id << std::endl;
		std::cout << b->vec[0]->name << std::endl;
		std::cout << b->vec[1]->id << std::endl;
	}
	else
	{
		std::cout << "failed." << std::endl;
	}
}
```

---  

### 目录

- [memUnit](#memUnit)
- [memManager](#memManager)
- [支持的数据类型](#支持的数据类型)
  - [算术类型与枚举](#算术类型与枚举)
  - [原生数组](#原生数组)
  - [STL容器](#stl容器)
  - [字符串](#字符串)
  - [智能指针](#智能指针)
  - [文件出入口](#文件出入口)
  - [variant](#variant)
  - [pair](#pair)
  - [tuple](#tuple)
  - [pFunction](#pfunction)
  - [任意结构体的内存直接序列化](#任意结构体的内存直接序列化)
  - [自定义序列化](#自定义序列化)
- [智能指针](#智能指针)
  - [dumbPtr](#dumbPtr)
  - [impPtr](#impPtr)
  - [memPtr](#memptr)
- [两种二进制序列化](#两种二进制序列化)
- [序列化原理](#序列化原理)
- [反序列化构造原理](#反序列化构造原理)
- [析构原理](#析构原理)
- [自定义格式](#自定义格式)
- [JSON序列化与反序列化](#json序列化与反序列化)
- [静态反射](#静态反射)
- [跨文件引用：出入口机制](#跨文件引用出入口机制)
  - [pEgress](#pegress)
- [线程安全](#线程安全)

---

### memUnit
- 继承`memUnit`类
- 实现构造函数 `anyClass(memManager* m):memUnit(m){}`
- 实现纯虚函数 `void save_fetch(memPara para) override{}`
- 可选：实现虚函数 `fetchInit(){}` 并标记 `isFetchInit` 为 `true`
- `GWPP(const WCHAR* key, Types& varST, memPara para);` 在 `save_fetch` 函数内部，填写想要管理的变量
- 每个 `memUnit` 在创建时需指定一个 `memManager` 以隶属。`memManager` 内部维护了一个列表，用以记录所有的 `memUnit`
- `memUnit` 子类之间禁止成员组合，即 `memUnit` 类中禁止存在另一个 `memUnit`

### memManager
- 继承 `memManager` 类
- 实现纯虚函数 `void save_fetch(memPara para) override{}`
- `setUrl(const WCHAR* wcptr)` 设置文件路径
- `upload()` 上传文件到内存
- `download()` 下载文件到硬盘

### 支持的数据类型
- 算术类型与枚举
- 原生数组
- STL容器
- 字符串
- 智能指针
- 文件出入口
- variant
- pair
- tuple
- pFunction
- 任意结构体的内存直接序列化
- 自定义序列化

### 智能指针

#### memPtr
- 基本内存单元的智能指针，类似于 `shared_ptr`
- 此指针只能指向 `memUnit` 派生类
- 每个 `memUnit` 内部设置了一个指针，用于指回 `memPtr` 所创建的中间体
- 使用 `isFilled()` 和 `isEmpty()` 判断其是否为空
- 此指针不支持多态，因最终存档中不包含类型信息，依靠此指针编译期的模板类型正确进行反序列化
- `memUnit` 中成员 `memPtr` 的指向，不能跨越 `memUnit` 的 `memManager`

### 两种二进制序列化
（根据提供的信息，此处略）

### 序列化原理
（根据提供的信息，此处略）

### 反序列化构造原理

- 从硬盘中读取并构造时，执行顺序如下：
  - 执行构造函数M
  - 执行 `save_fetch`，读取各变量
  - 若 `isFetchInit` 为 `true`，则其他 `memUnit` 在构造时，每次通过指针读取到此 `memUnit` 时，执行一次

### 析构原理

- `memManager` 内部记录所有隶属此类的 `memUnit`
- 在析构时，所有记录的 `memUnit` 都会被析构，无论其是否悬空
- 各个 `memUnit` 的析构顺序是其指针的绝对值大小，因此 `memUnit` 的析构顺序无法保证
- 在 `memManager` 整体析构时，会先将子 `memUnit` 的 `mngr` 属性清空，再执行子 `memUnit` 的析构函数

### 自定义格式
（根据提供的信息，此处略）

### JSON序列化与反序列化
（根据提供的信息，此处略）

### 静态反射
（根据提供的信息，此处略）

### 跨文件引用：出入口机制

#### pEgress

- 用于 `memManager` 之间的通信
- `pEgress<>` 模板中，填写想要指向的对方 `memManager` 类中成员 `memUnit` 的类名
- 本质是一个智能指针，指向一个内部类 `Egress`
- `Egress` 记录了对方 `memManager` 的文件名、键名和类类型
- 使用 `makeEIPair()` 后，会在自身 `memManager` 内创建一个 `Egress`，并让 `pEgress` 指向这个 `Egress`，并在对方 `memManager` 内创建一个 `Ingress`，`Ingress` 指向目标 `memUnit`
- 这个 `Egress-Ingress Pair` 将拥有相同的键名和类型
- 使用 `getTarget()` 方法尝试通过 `Egress` 的文件名、键名和类型数据获取对方 `Ingress` 所指向的 `memUnit`
- 此库维护了一个全局文件表（带锁），记录了当前加载的所有 `memManager`
- `getTarget()` 中，`Egress` 将会尝试从全局文件表中找到这个文件名。因此，调用 `getTarget()` 前，要确保对方 `memManager` 已经以某种形式载入到内存
- `getTarget()` 返回值意义见文件开头的宏定义 `MEM_` 系列

### 线程安全

- 以 `memManager` 为单位加锁后，能保证此 `memManager` 及其下属 `memUnit` 是线程安全的
- `memUnit` 的反射、序列化等操作，需要借助 `memManager` 共享部分数据
- 因此对 `memUnit` 加锁，不能保证其安全
