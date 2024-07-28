# memManager

[English(pending)](README-en.md)  
### 统一实现了C++结构体的内存RAII、磁盘存储、跨文件引用、静态反射、二进制序列化、json序列化。 

#### 支持平台
- Windows64/32
- ESP32  

#### 如何使用
这是一个headonly库。把memManager文件夹复制到源目录下，在源代码中`#include "memManager/memManager.h"` 即可

C++标准：17或更高

#### 示例代码

```C++
#include "memManager/memManager.h"

struct testU : public mem::memUnit {			//继承memUnit

	int id;						//定义成员变量
	std::string name;

	void save_fetch(mem::memPara para) override {	//实现纯虚函数save_fetch，把所有要保存的变量写进去
		GWPP("id", id, para);
		GWPP("name", name, para);
	}

	testU(mem::memManager* m) :memUnit(m) {}	//实现构造函数并传递memManager指针

	MEM_PERMISSION					//权限宏(如果是public可以不写)
};

struct testM : public mem::memManager {			//继承memManager

	std::vector<mem::memPtr<testU>> vec;		//此库能正确序列化指针关系
	
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

	a->setUrl("D:\\test");		//设置一个有效的目录
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

**详细Demo兼单元测试 :** 

```C++
#include "memManager/demo.h"

mem_testmain();
```

---  

### 文档目录

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
  - [tuple](#tuple)	（拟支持）
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
  - [Ingress](#Ingress)
  - [pIngress](#pIngress)
  - [Egress](#Egress)
  - [pEgress](#pEgress)
- [线程安全](#线程安全)
- [使用的库](#使用的库)

---

## memUnit  
- `memUnit` 是内存管理的基础单元，封装了内存操作的通用逻辑。
- 每个 `memUnit` 对象都与一个 `memManager` 对象关联，后者管理着内存的分配和释放。
- `memUnit` 在创建时需指定一个 `memManager` 以隶属。
- `memManager` 内部维护了一个列表，用以记录所有的 `memUnit`。
- `memManager` 析构时，其下属所有的 `memUnit` 都会被析构。

#### 使用方式
- 用户类继承 `memUnit`
- 实现纯虚函数 `void save_fetch(mem::memPara para) override {}`
- 实现构造函数 `user(mem::memManager* m) :memUnit(m) {}`
- 若上面的两个函数不可见，则需添加权限宏 `MEM_PERMISSION`

#### 限制
- `memUnit` 不能分配在栈上
- `memUnit` 之间禁止成员组合，即 `memUnit` 类中禁止存在成员 `memUnit`

#### 方法与属性
- ***`memManager* getManager();`***
	- 获取管理该 `memUnit` 对象的 `memManager` 指针。
<br>

 - ***`void serialize(std::vector<uint8_t> bc);`***
 	- 二进制序列化函数，将单个 `memUnit` 对象保存到字节流中。
  	- **信息有损：** 忽视所有指针。
<br>

 - ***`bool deserialize(uint8_t Ptr, uint32_t StringSize);`***
 	- 二进制反序列化函数，从字节流恢复 `memUnit` 对象。
<br>

 - ***`void serializeJson(std::string* bc);`***
 	- JSON序列化函数。
  	- **信息有损：** 其中的 **每个** 指针都会转化成一个子 `memUnit` 对象节点或 `null` ，而无视指针间的指向关系；同时若有循环引用，则节点变为字符串 "Recurring Object"。
<br>

 - ***`bool deserializeJson(const char* Ptr, uint32_t StringSize);`***
 	- JSON反序列化函数。
<br>

- ***`void GWPP_Any(const char* key, Type var, mem::memPara para)`***
	- 简单地序列化任意结构体。
 	- 将结构体的内存直接复制，以进行序列化。Json将保存为base64格式。
<br>

- ***`void GWPP(const char* key, Type var, mem::memPara para)`***
	- 按指定规律，序列化保存。
 	- 无论数组、指针、泛型如何嵌套，都能进行正确的序列化。
  	- 对于不支持的数据类型，SFINAE将不通过。[支持的数据类型](#支持的数据类型)

## memManager  
- 一个 `memManager` 类的实例，就相当于一个磁盘上的文件，可以下载（保存）数据到磁盘，上传（加载）数据到内存。
- `memManager` 类继承了 `memUnit` 类，在其 `memManager` 上保存的指针，将充当根索引。
- 沿着根索引查找不到的指针，将视为悬空指针。序列化时将不会保存这些指针。

#### 使用方式
- 继承 `memManager` 类
- 实现纯虚函数 `void save_fetch(memPara para) override{}`
- 若纯虚函数不可见，则需添加权限宏 `MEM_PERMISSION`

#### 方法与属性
- ***`void setUrl(const WCHAR* wcptr);`***
	- 设置文件路径。
<br>

- ***`char* getFileName();`***
	- 获取路径中的文件名。
<br>

- ***`void serialize(std::vector<uint8_t> bc);`***
 	- 二进制序列化函数，将 `memManager` 及其下属所有 `memUnit` 对象保存到字节流中。
  	- **信息无损：** 能正确处理所有根索引可及的内存单元，能正确指示指针间的指向、嵌套、多态（需要结构设计合理，具体见[impPtr](#impPtr)）。
<br>

- ***`bool deserialize(uint8_t Ptr, uint32_t StringSize);`***
 	- 二进制反序列化函数，从字节流恢复 `memManager` 对象及其所有下属 `memUnit`。
<br>

- ***`upload()`***
	- 根据 `setUrl` 设置的路径，上传文件到内存
<br>

- ***`download()`***
	- 根据 `setUrl` 设置的路径，下载文件到硬盘
<br>

- ***`memPtr<Subfile> findSubfile(const char* fileName);`***
	- 在全局范围内，查找该管理器依赖的子文件（子管理器）。
<br>

- ***`memPtr<Egress> findEgress(const memPtr<Subfile> subfile, const char* kw, const char* type);`***
	- 在指定的子管理器中，根据关键字和类型名，查找“出口”
<br>

- ***`impPtr<Ingress> findIngress(const char* kw, const char* type);`***
	- 根据关键字和类型名，查找该管理器的“入口”

# (以下的文档还没有写完)

## 支持的数据类型
#### 算术类型与枚举
#### 原生数组
#### STL容器
- vector
- list
- deque
- array
- forward_list
- set
- unordered_set
- multiset
- unordered_multiset
- map
- unordered_map
- multimap
- unordered_multimap

#### 字符串
#### 智能指针
#### 文件出入口
#### variant
#### pair
#### tuple	（拟支持）
#### pFunction
#### 任意结构体的内存直接序列化
#### 自定义序列化

## 智能指针

#### dumbPtr

#### impPtr

#### memPtr
- 基本内存单元的智能指针，类似于 `shared_ptr`。
- 此指针只能指向 `memUnit` 派生类。
- 每个 `memUnit` 内部设置了一个指针，用于指回 `memPtr` 所创建的中间体。
- 使用 `isFilled()` 和 `isEmpty()` 判断其是否为空。
- `memUnit` 中成员 `memPtr` 的指向，不能跨越 `memUnit` 的 `memManager`。当序列化时检测到跨越行为，断言将失败。

## 两种二进制序列化

#### memUnit的序列化

#### memManager的序列化

## 序列化原理

## 反序列化构造原理

- 从硬盘中读取并构造时，执行顺序如下：
  - 执行构造函数
  - 执行 `save_fetch`，读取各变量

## 析构原理

- `memManager` 内部记录所有隶属此类的 `memUnit`
- 在析构时，所有记录的 `memUnit` 都会被析构，无论其是否悬空
- 各个 `memUnit` 的析构顺序是其指针的绝对值大小，因此 `memUnit` 的析构顺序无法保证
- 在 `memManager` 整体析构时，会先将子 `memUnit` 的 `mngr` 属性清空，指针信息清除，再执行子 `memUnit` 的析构函数

## 自定义格式

## JSON序列化与反序列化

## 静态反射
（此功能目前还在实现中）

## 跨文件引用：出入口机制

#### Ingress

#### pIngress

#### Egress

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

## 线程安全

- 以 `memManager` 为单位加锁后，能保证此 `memManager` 及其下属 `memUnit` 是线程安全的
- `memUnit` 的反射、序列化等操作，需要借助 `memManager` 共享部分数据。因此对 `memUnit` 加锁，不能保证其安全
  
### 使用的库：
* **rapidJson**  ：用于JSON支持  
