# memManager

[English(pending)](README-en.md)  
### 统一实现了C++结构体的内存RAII、磁盘存储、跨文件引用、静态反射、二进制序列化、json序列化。 

#### 如何使用
这是一个headonly库。把memManager文件夹复制到源目录下，在源代码中`#include "memManager/memManager.h"` 即可

**C++标准：** 17或更高

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

	std::vector<mem::memPtr<testU>> vec;		//此库能正确序列化memPtr系列智能指针的指向关系
	
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
  - [原子量](#原子量)
  - [字符串](#字符串)
  - [memPtr系列指针](#memPtr系列指针)
  - [文件出入口](#文件出入口)
  - [variant](#variant)
  - [pair](#pair)
  - [tuple](#tuple)	（暂不，拟支持）
  - [optional](#optional)	（暂不，拟支持）
  - [pFunction](#pfunction)
  - [非侵入式，任意结构体的内存直接序列化](#非侵入式，任意结构体的内存直接序列化)
  - [自定义序列化](#自定义序列化)
- [memPtr系列](#memPtr系列)
  - [dumbPtr](#dumbPtr)
  - [memPtr](#memptr)
  - [impPtr](#impPtr)
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
- [文件读写接口](#文件读写接口)
- [线程安全](#线程安全)
- [使用的库](#使用的库)
- [联系作者](#联系作者)

---

## memUnit  
- `memUnit` 是内存管理的基础单元，封装了内存操作的通用逻辑。
- 每个 `memUnit` 对象都与一个 `memManager` 对象关联， `memManager` 记录着每一块 `memUnit` 的分配和释放。
- `memUnit` 在创建时需指定一个 `memManager` 以隶属。
- `memManager` 内部维护了一个列表，用以记录所有的 `memUnit`。
- `memManager` 析构时，其下属所有的 `memUnit` 都会被析构。

#### 使用方式
- 用户类继承 `memUnit`
- 实现纯虚函数 `void save_fetch(mem::memPara para) override {}`
- 实现构造函数 `userClass(mem::memManager* m) :memUnit(m) {}`
- 若上面的两个函数不为public，则需添加权限宏 `MEM_PERMISSION`

#### 限制
- `memUnit` 不能分配在栈上
- `memUnit` 之间禁止成员组合，即 `memUnit` 类中禁止存在成员 `memUnit`

#### 方法与属性
- ***`memManager* getManager();`***
	- 获取管理该 `memUnit` 对象的 `memManager` 指针。
<br>

 - ***`void serialize(std::vector<uint8_t> bc);`***
 - ***`bool deserialize(uint8_t* Ptr, uint32_t StringSize);`***
 	- 二进制序列化/反序列化，单一序列化：仅序列化此memUnit。
  	- **信息有损：** 忽视所有指针。
   	- 详见：[两种二进制序列化](#两种二进制序列化)
<br>

 - ***`void serializeJson(std::string* bc);`***
 - ***`bool deserializeJson(const char* Ptr, uint32_t StringSize);`***
 	- JSON序列化/反序列化。
  	- **信息有损：** 其中的 **每个** memPtr指针都会转化成一个子 `memUnit` 对象节点或 `null` ，而无视指针重复的指向关系；同时若有循环引用，则节点变为字符串 "Recurring Object"。
   	- 详见：[JSON序列化与反序列化](#json序列化与反序列化)
<br>

- ***`void GWPP_Any(const char* key, Type var, mem::memPara para)`***
	- 在save_fetch中使用，简单地序列化任意结构体。
 	- 将结构体的内存直接复制，以进行序列化。在Json序列化时输出base64格式。
<br>

- ***`void GWPP(const char* key, Type var, mem::memPara para)`***
	- 在save_fetch中使用，按变量类型序列化。
 	- 无论数组、指针、泛型如何嵌套，都能进行正确的序列化。
  	- 对于不支持的数据类型，SFINAE将不通过。[支持的数据类型](#支持的数据类型)

## memManager  
- 一个 `memManager` 类的实例，就相当于一个磁盘上的文件，可以下载（保存）数据到磁盘，上传（加载）数据到内存。
- `memManager` 类继承了 `memUnit` 类，"memManager is memUnit."。在 `memManager` 上保存的其它 `memUnit` 的指针，将充当根索引。
- 沿着根索引触及不到的 `memUnit` 类，将视为悬空类。整体序列化时将不会保存这些类。
- 虽然悬空类不会被序列化，但是它们仍然被 `memManager` 记录， `memManager` 析构时会析构这些悬空类。

#### 使用方式
- 继承 `memManager` 类
- 实现纯虚函数 `void save_fetch(memPara para) override{}`
- 若纯虚函数不为public，则需添加权限宏 `MEM_PERMISSION`

#### 方法与属性
- ***`std::string url;`***
	- 文件路径。
<br>

- ***`void setUrl(const std::string& str);`***
	- 设置文件路径。
<br>

- ***`char* getFileName();`***
	- 获取路径中的文件名。
<br>

- ***`void serialize(std::vector<uint8_t>* bc);`***
- ***`bool deserialize(uint8_t* Ptr, uint32_t StringSize);`***
 	- 二进制序列化/反序列化，整体序列化，序列化整个memManager。
  	- **信息无损：** 能正确处理所有根索引可及的内存单元，能正确指示指针间的指向、嵌套、多态（多态需要结构设计合理，具体见[impPtr](#impPtr)）。
<br>

- ***`download()`***
- ***`upload()`***
	- 根据 `url` 属性指示的路径，进行硬盘操作。
 	- `upload()` 与 `download()` 使用fopen系列函数，详见：[文件读写接口](#文件读写接口)
<br>

- ***`uint32_t statusBadValue;`***
	- 上一次反序列化时，从字节流中读取到的“坏值”。
<br>

- ***`memPtr<Subfile> findSubfile(const char* fileName);`***
	- 在全局范围内，查找该管理器依赖的子文件（子管理器）。
<br>

- ***`memPtr<Egress> findEgress(const memPtr<Subfile> subfile, const char* kw, const char* type);`***
	- 在指定的子管理器中，根据关键字和类型名，查找“出口”
<br>

- ***`impPtr<Ingress> findIngress(const char* kw, const char* type);`***
	- 根据关键字和类型名，查找该管理器的“入口”

## memPtr系列

- `memPtr` 系列指针，是只能指向 `memUnit` 派生类的智能指针，其控制块使用一个内存池分配。 `memUnit` 派生类与控制块是独立分配的。

#### dumbPtr

- 基本指针，无法写在 `save_fetch` 中进行序列化。
- 指针只能指向 `memUnit` 派生类。
- 每个 `memUnit` 内部设置了一个指针，用于指回 `dumbPtr` 的控制块。
- ***方法与属性：***
	- `operaotr =(T mu)` 指针赋值，将一个 `memUnit` 派生类绑定到此指针下。若此 `memUnit` 未绑定控制块，则分配一个新的控制块；否则赋给此 `memUnit` 绑定的控制块。
	- `precision_cast(T* mu)`  `precision_cast()` 基类与派生类指针之间的类型转换。派生类转换为基类总是成功；在不开启RTTI时，根据虚表指针是否相同，来匹配从基类到派生类的转换。前一个重载用于转入，成功返回 `true` ，失败返回 `false` ；后一个重载用于转出，成功返回 `*` 原始指针，失败返回 `nullptr` 。
	- `isFilled()`  `isEmpty()` 判断其是否为空。
 	- `swap()` 交换两个指针。
  	- `operaotr ==()` 比较两个指针所指向的 `memUnit` 是否为同一个，或者是否都为空。
  	- `operator*()` `operator->()` 取原始指针，空指针返回 `nullptr` 。
  	- `release()` 释放指针所指的 `memUnit` ，其他指向此 `memUnit` 的 `memPtr` 则会变为类似空指针的行为逻辑，但是依然指向控制块，且控制块需等到引用计数为0后才能回收。类似于 `weak_ptr` 。
  	- `moveIn()` `operator<<()` 移入，将入参 `memPtr` 移入此指针，入参 `memPtr` 则变为空指针。
  	- `supplantIn()` 代换，使用一个新的 `memUnit` 来代替控制块所指向的 `memUnit` ，此时所有指向旧 `memUnit` 的 `memPtr` 系列指针都将指向这个新的 `memUnit` ，旧的 `memUnit` 将被释放。

#### memPtr

- 序列化指针，**继承了 `dumpPtr` 的所有功能**。可以在 `save_fetch` 中参与序列化。
- 在序列化时，会尝试在 `memManager` 内查找此指针指向的 `memUnit` 是否已经被构造。若没有，则构造；若有，则指向这个构造了的实例。
- 因此，序列化能正确处理此指针的多次引用与循环引用。
- 对于STL容器、 `std::variant` 中的指针，序列化能够正常处理。
- ***限制：***
	- `memUnit` 中成员 `memPtr` 的指向，不能够跨越此 `memUnit` 的 `memManager` 。即 `memManager A` 下属的 `memUnit` 中的成员 `memPtr` 无法指向 `memManager B` 下属的 `memUnit` 。
	- 序列化时检测到跨越行为，断言将失败。若要跨越，使用 `dumpPtr` 或 [跨文件引用：出入口机制](#跨文件引用：出入口机制)

#### impPtr

- 多态指针，继承了 `dumpPtr` 的所有功能。可以写在 `save_fetch` 中参与序列化。
- 在序列化时，只尝试从已经构造了的 `memUnit` 实例中查找，找不到时，不会主动构造一个新内存单元（因为丢失了派生类的类型信息）。
- 因此，在 `save_fetch` 反序列化时，某个 `memPtr` 必须**先**构造出派生类，基类指针 `impPtr` 之**后**才能指向。

--

- 将 `memPtr` 转换为 `impPtr` ， `impPtr` 转换为 `dumpPtr` 是合法的。反之， `dumpPtr` 无法转换为 `impPtr` ， `impPtr` 无法转换为 `memPtr` 。
- 若要强制转换，可以提取出 `*` 原始指针，再赋值给目标。

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
- stack(暂不)
- queue(暂不)
- priority_queue(暂不)

#### 原子量
#### 字符串
#### memPtr系列指针
#### 文件出入口
#### variant
#### pair
#### tuple	（暂不，拟支持）
#### optional	（暂不，拟支持）
#### pFunction
#### 非侵入式，任意结构体的内存直接序列化
#### 自定义序列化

## 静态反射
（此功能目前还在实现中）

## 跨文件引用：出入口机制

#### Ingress

#### pIngress

#### Egress

#### pEgress

- 用于 `memManager` 之间的通信
- `pEgress<>` 模板中，填写想要指向的对方 `memManager` 类中成员 `memUnit` 的类名
- 本质是一个impPtr指针，指向一个内部类 `Egress`
- `Egress` 记录了对方 `memManager` 的文件名、键名和类类型
- 使用 `makeEIPair()` 后，会在自身 `memManager` 内创建一个 `Egress`，并让 `pEgress` 指向这个 `Egress`，并在对方 `memManager` 内创建一个 `Ingress`，`Ingress` 指向目标 `memUnit`
- 这个 `Egress-Ingress Pair` 将拥有相同的键名和类型
- 使用 `getTarget()` 方法尝试通过 `Egress` 的文件名、键名和类型数据获取对方 `Ingress` 所指向的 `memUnit`
- 此库维护了一个全局文件表（带锁），记录了当前加载的所有 `memManager`
- `getTarget()` 中，`Egress` 将会尝试从全局文件表中找到这个文件名。因此，调用 `getTarget()` 前，要确保对方 `memManager` 已经以某种形式载入到内存

## 文件读写接口

- 在 `HAL\` 目录下 `fileHAL.h` 文件中。
- 目前使用C标准库 `fopen` 系列。可修改此接口，以实现文件IO多路复用，或移植到其他平台。

## 线程安全

- 以 `memManager` 为单位加锁后，能保证整个 `memManager` 和下属 `memUnit` 在这个线程是安全的。
- `memUnit` 的构造析构、反射、单一序列化、整体序列化等操作，均需要使用并修改 `memManager` 内部数据。因此对同一 `memManager` 的不同 `memUnit` 加锁，不能保证这些操作的线程安全。
- 对 `memPtr` 系列指针及其控制块的任何修改操作均是线程不安全的；只读取则是线程安全的。
- 通过放弃 `memPtr` 与 `memUnit` 的线程安全，换取了其高度的数据操作灵活性。对于热点代码，亦有多种简便方法来保证同一 `memManager` 下的多线程安全。
  
### 使用的库：
* **rapidJson**  ：用于JSON支持

### 联系作者：
* 在此项目中提出issue
* 或+QQ：2219647655
