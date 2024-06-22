//内存与存档管理系统
//Super Super fast json!			------Head-Only------

#pragma once
#define MEM_MANAGER_DISABLE_WARINIGS 4244 4267
#pragma warning(disable: MEM_MANAGER_DISABLE_WARINIGS)
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <cassert>
#include <sstream>
#include <locale>
#include <codecvt>
#include "memUnitMacro.h"
#include <type_traits>
#include <windows.h>

#define NOMINMAX 0

#define ORD_SAVE 0				//把数据从内存保存到硬盘
#define ORD_FETCH 1				//从硬盘中拿数据到内存
#define ORD_REFLECTION_R 2		//反射读变量
#define ORD_REFLECTION_W 3		//反射写变量（单个）
#define ORD_SERIALIZE 4			//序列化
#define ORD_DESERIALIZE 5		//反序列化

#define MEM_EMPTY_EGRESS		3	//出口是空的
#define MEM_NOTFOUND_FILE		2	//与出口绑定的入口memManager没有加载
#define MEM_NOTFOUND_KEYWORDS	1	//没有在入口memManager找到出口关键字
#define MEM_EMPTY_INGRESS		4	//入口memManager找到，关键字找到，但是入口所指向的内存是空的
#define MEM_SUCCESS				0	//成功从出口中获取变量

#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__) || defined(__powerpc64__) || defined(__s390x__)
#define POINTER_L long long
#define _wtop _wtoll
#define _ptow_s _lltow_s
#else
#define POINTER_L long
#define _wtop _wtol
#define _ptow_s _ltow_s
#endif

inline namespace mem {
inline errno_t _lltow_s(long long value, wchar_t* str, size_t sizeOfstr, const int radix) {
	assert(radix == 10 || !("radix must be 10"));

	std::wstringstream wss;
	wss << value;

	std::wstring wstr = wss.str();
	if (wstr.size() + 1 > sizeOfstr)
		return 1; // Error: Insufficient buffer size

	wcsncpy_s(str, sizeOfstr, wstr.c_str(), sizeOfstr);
	return 0; // Success
}
template <size_t size>
inline errno_t _lltow_s(long long value, wchar_t(&str)[size], const int radix) {
	assert(radix == 10 || !("radix must be 10"));

	std::wstringstream wss;
	wss << value;

	std::wstring wstr = wss.str();
	if (wstr.size() + 1 > size)
		return 1; // Error: Insufficient buffer size

	wcscpy_s(str, size, wstr.c_str());
	return 0; // Success
}

	//全局变量
	const UINT maxURL = MAX_PATH;
	const UINT maxKey = 25;
	const UINT maxKW = 25;
	const UINT defaultWcs = 50;

	//全局·文件加载表
	class memManager;
	inline HANDLE global_mutex = NULL;
	inline std::vector<memManager*> global_load = {};

	//两级前向声明
	struct memApp;
	class ReflectResultKeyValue;
	struct ReflectResult;
	struct memPara {
		union {
			std::vector<memApp*>* appSegment;
			ReflectResult* reflection;
			ReflectResultKeyValue* reflection_single;
		};
		SHORT order;
		inline bool isConstruct()
		{
			switch (order)
			{
			case ORD_FETCH:
			case ORD_REFLECTION_R:
			case ORD_DESERIALIZE:
				return true;
			}
			return false;
		}
	};
	struct memPtrCorr;
	class memUnit;
	class memWcs;
	template<class mu, bool releaseable = true> class memVector;
	class Egress;
	class Ingress;
	class Subfile;
	template<typename First, typename...Args> class pVariant;
	template<class cast> class memVectorEgress;
	template<typename... Args> class memVectorVariant;
	struct memPtrComm {
		int count;
		memUnit* content;
		inline memPtrComm(memUnit* m)
		{
			count = 1;
			content = m;
		}
	};
	//若某类要继承memPtr，则使用vector时必须重写一份
	template<typename mu, bool releaseable = true>
	class memPtr{
		memPtrComm* ptr;
		inline memPtr* getPtr()
		{
			return this;
		}
		inline void cdd() {
			if (ptr)
				if (ptr->count)
				{
					if (ptr->count == 1)
					{
						if (ptr->content)
							if (ptr->content->mngr)
								delete(ptr->content);
						delete ptr;
						ptr = NULL;
					}
					else
						(ptr->count)--;
				}
		}
		inline void cdd(memPtrComm* ptrOperation);
		//退化，将此指针引用次数自减，但当减为0时，不会进行memUnit析构，而是使memUnit回到没有挂载智能指针的状态。
		inline void degeneracy()
		{
			if (ptr)
				if (ptr->count)
				{
					if (ptr->count == 1)
					{
						if (ptr->content)
							ptr->content->sharedPtr = nullptr;
						delete ptr;
					}
					else
						(ptr->count)--;
				}
			ptr = NULL;
		}
		template<class any, bool _r>
		inline void equalAny(const memPtr<any, _r>& mp)
		{
			cdd();
			if (mp.isEmpty())
			{
				ptr = NULL;
			}
			else
			{
				ptr = mp.ptr;
				(ptr->count)++;
			}
		}
	public:
		inline memPtr()
		{
			static_assert(std::is_base_of_v<memUnit, mu>, "template is not Base of memUnit");
			ptr = NULL;
		}
		inline memPtr(mu* pmu)
		{
			static_assert(std::is_base_of_v<memUnit, mu>, "template is not Base of memUnit");
			if (pmu)
			{
				if (((memUnit*)pmu)->sharedPtr)
				{
					ptr = ((memUnit*)pmu)->sharedPtr;
					(ptr->count)++;
				}
				else
				{
					ptr = new memPtrComm((memUnit*)pmu);
					((memUnit*)pmu)->sharedPtr = ptr;
				}
			}
			else
			{
				ptr = NULL;
			}
		}
		inline memPtr(const memPtr<mu, releaseable>& mp)
		{
			static_assert(std::is_base_of_v<memUnit, mu>, "template is not Base of memUnit");
			if (mp.isEmpty())
			{
				ptr = NULL;
			}
			else
			{
				ptr = mp.ptr;
				(ptr->count)++;
			}
		}
		inline ~memPtr(){
			cdd();
		}
		friend class memUnit;
		friend class memManager;
		template<typename First, typename...Args> friend class pVariant;
		template<class cast> friend struct pEgress;
		template<class any, bool _r> friend class memPtr;
		void operator=(const memPtr<mu, releaseable>& mp);
		void operator=(mu* pmu);
		void swap(memPtr& sw);
		inline bool isEmpty() const
		{
			return !(ptr && ptr->content);
		}
		inline bool isFilled() const
		{
			return (ptr && ptr->content);
		}
		inline bool operator==(const memPtr& right) const
		{
			if (isFilled())
				return ptr == right.ptr;
			else if (right.isEmpty())
				return true;
			else
				return false;
		}
		inline bool operator!=(const memPtr& right) const
		{
			return !(this->operator==(right));
		}
		inline mu* operator*() const
		{
			if (isFilled())
				return (mu*)(ptr->content);
			else
				return NULL;
		}
		inline mu* operator->() const
		{
			if (isFilled())
				return (mu*)(ptr->content);
			else
				return NULL;
		}
		inline void release()
		{
			static_assert(releaseable == true, "this memPtr was definded to cannot be release");
			if (isFilled())
			{
				memUnit* i = ptr->content;	//这里不能优化。虽然在memUnit基类的析构函数中也会执行相同的操作，但是C++会先执行派生类的析构函数。而如果在派生类的析构函数中，此指针未被擦除，将导致未定义行为。
				ptr->content = NULL;
				delete(i);
			}
		}
	};
	template<typename T>
	struct is_template : std::false_type {};
	template<template<typename...> class T, typename... Args>
	struct is_template<T<Args...>> : std::true_type {};
	template<typename T>
	struct extract_template_memUnit { using type = void; };
	template<typename T, bool releaseable>
	struct extract_template_memUnit<memPtr<T, releaseable>> { using type = T; };
	template<typename T>
	struct is_based_on_memPtr {
	private:
		template <class U>
		static std::true_type test(const memPtr<U>&);
		static std::false_type test(...);
	public:
		static constexpr bool value = decltype(test(std::declval<T>()))::value;
	};
	class ReflectResultKeyValue {
		friend class memUnit;
		template<bool _void>
		inline void MatchVariant(void* vtptr, UINT& type, memPtr<memUnit>& ptr)
		{
			type = 0;
			ptr = NULL;
		}
		template<bool _void, typename IterFirst, typename... IterArgs>
		inline void MatchVariant(void* vtptr, UINT& type, memPtr<memUnit>& ptr)
		{
			if (memUnit::get_vtable_ptr<IterFirst>::ptr() == vtptr)
			{
				key[0] = L'\0';
				ptr = value.ptr;
			}
			else
			{
				type++;
				MatchVariant<false, IterArgs...>(vtptr, type, ptr);
			}
		}
		template<bool isSet, class _T, class e_T>
		inline void SetOrGetType(_T& thisSide, _T& memUnitSide, e_T et) {
			if constexpr (isSet)
			{
				thisSide = memUnitSide;
				type = et;
			}
			else
			{
				key[0] = L'\0';
				memUnitSide = thisSide;
			}
		}
		template <bool isSet> inline void TypeFunc(bool& va) { SetOrGetType<isSet>(value._bool, va, bool_); };
		template <bool isSet> inline void TypeFunc(char& va) { SetOrGetType<isSet>(value._char, va, char_);};
		template <bool isSet> inline void TypeFunc(uint8_t& va) { SetOrGetType<isSet>(value.u8, va, u8);};
		template <bool isSet> inline void TypeFunc(int8_t& va) { SetOrGetType<isSet>(value.i8, va, i8);};
		template <bool isSet> inline void TypeFunc(uint16_t& va) { SetOrGetType<isSet>(value.u16, va, u16);};
		template <bool isSet> inline void TypeFunc(int16_t& va) { SetOrGetType<isSet>(value.i16, va, i16);};
		template <bool isSet> inline void TypeFunc(uint32_t& va) { SetOrGetType<isSet>(value.u32, va, u32);};
		template <bool isSet> inline void TypeFunc(int32_t& va) { SetOrGetType<isSet>(value.i32, va, i32);};
		template <bool isSet> inline void TypeFunc(unsigned long& va) { SetOrGetType<isSet>(value.u32, (uint32_t&)va, u32);};
		template <bool isSet> inline void TypeFunc(long& va) { SetOrGetType<isSet>(value.i32, (int32_t&)va, i32);};
		template <bool isSet> inline void TypeFunc(uint64_t& va) { SetOrGetType<isSet>(value.u64, va, u64);};
		template <bool isSet> inline void TypeFunc(int64_t& va) { SetOrGetType<isSet>(value.i64, va, i64);};
		template <bool isSet> inline void TypeFunc(float& va) { SetOrGetType<isSet>(value.f32, va, f32);};
		template <bool isSet> inline void TypeFunc(double& va) { SetOrGetType<isSet>(value.f64, va, f64);};
		template <bool isSet, class _MU, bool re> inline void TypeFunc(memPtr<_MU, re>& va) {
			if constexpr (isSet)
			{
				type = p_memUnit;
				value.ptr = reinterpret_cast<memUnit*>(*va);
			}
			else
			{
				//if (typeid(_MU) == typeid(*value.ptr))
				if(memUnit::get_vtable_ptr<_MU>::ptr() == *(void**)std::addressof(*value.ptr))		//若禁用了RTTI，则使用虚表指针来判断。
				{
					key[0] = L'\0';
					va = reinterpret_cast<_MU*>(value.ptr);			//硬赋值在多重继承的时候可能会报错，所以没写重载让memUnit*赋值memPtr。等到报错的情况再处理。
				}
			}
		};
		template<bool isSet, class First, class ...Args>inline void TypeFunc(pVariant<First, Args...>& va) {
			if constexpr (isSet)
			{
				type = p_variant;
				value.ptr = reinterpret_cast<memUnit*>(*(va.ptr));
			}
			else
			{
				va.type = 1;
				MatchVariant<false, First, Args...>(*(void**)std::addressof(*value.ptr), va.type, va.ptr);
			}
		}
		template<class _T>
		inline void WriteMU(const WCHAR* muProperty_key, _T& muProperty){
			if (wcscmp(muProperty_key, key) == 0)
			{
				TypeFunc<false>(muProperty);
			}
		}
		inline void WriteMU(const WCHAR* muProperty_key, WCHAR* muProperty, UINT size) {
			if (wcscmp(muProperty_key, key) == 0)
			{
				key[0] = L'\0';
				wcscpy_s(muProperty, size, value.wchar);
			}
		}
	public:
		wchar_t key[maxKey];
		enum {
			void_,
			p_wchar,
			bool_,
			char_,
			u8,
			i8,
			u16,
			i16,
			u32,
			i32,
			u64,
			i64,
			f32,
			f64,
			enum_,
			p_memUnit,
			p_variant
			/*p_memVector,
			p_generic,
			p_memVectorVariant,
			p_egress,
			p_egressVector*/
		}type;
		union {
			wchar_t* wchar;
			bool _bool;
			char _char;
			unsigned char u8;
			signed char i8;
			unsigned short u16;
			short i16;
			unsigned int u32;
			int i32;
			unsigned long long u64;
			long long i64;
			float f32;
			double f64;
			memUnit* ptr;
		}value;
		inline ReflectResultKeyValue() {
			key[0] = L'\0';
			type = void_;
			value.u64 = 0;
		}
		template<class _T, typename std::enable_if<!std::is_convertible<_T, const wchar_t*>::value && !std::is_convertible<_T, memUnit*>::value, int>::type = 0>
		inline ReflectResultKeyValue(const WCHAR* _key, _T& va) {
			wcscpy_s(key, _key);
			TypeFunc<true>(va);
		}
		inline ReflectResultKeyValue(const WCHAR* _key, const wchar_t* va) {
			wcscpy_s(key, _key);
			value.wchar = (wchar_t*)va;
			type = p_wchar;
		}
		inline ReflectResultKeyValue(const WCHAR* _key, memUnit* va) {
			wcscpy_s(key, _key);
			value.ptr = va;
			type = p_memUnit;
		}
	};
	struct ReflectResult {
		std::vector<ReflectResultKeyValue> context;
	};
	class memUnit {
		memPtrComm* sharedPtr;
		memManager* mngr;
	public:
		template<class mu, bool releaseable> static mu* memCopy(const memPtr<mu, releaseable>& varMU);
		template<typename T>
		struct get_vtable_ptr {
			static inline void* ptr() {
				static_assert(std::is_polymorphic<T>::value, "T must be a polymorphic type");
				static void* ret = []() {
					if constexpr (std::is_default_constructible<T>::value)	//memManager
					{
						T temp = T();
						return *(void**)std::addressof(temp);
					}
					else													//memUnit
					{
						T temp = T(NULL);
						return *(void**)std::addressof(temp);
					}
				}();
				return ret;
			}
		};
		friend class Ingress;
		friend class Egress;
		friend class Subfile;
		template<class T> friend void GWPP_Struct(memUnit* mu, const WCHAR* key1, const WCHAR* key2, T& var, memPara para);
		friend class memWcs;
		template<class mu, bool releaseable> friend class memPtr;
		template<typename First, typename...Args> friend class pVariant;
		friend class memManager;
		inline memUnit(memManager* manager);
		inline memManager* getManager()
		{
			return mngr;
		}
		inline void reflectionRead(ReflectResult* output)
		{
			memPara para;
			para.reflection = output;
			para.order = ORD_REFLECTION_R;
			this->save_fetch(para);
		}
		//反射写入单个，true 表示成功，false 表示失败
		inline bool reflectionWrite(ReflectResultKeyValue inputKW)
		{
			memPara para;
			para.reflection_single = &inputKW;
			para.order = ORD_REFLECTION_W;
			this->save_fetch(para);
			return !(inputKW.key[0]);		//如果写入成功，GWPP会将key的第一个字符置为'\0'
		}
		bool deserialize(WCHAR* Ptr, UINT StringSize);
		void serialize(std::vector<WCHAR>* bc);
		virtual ~memUnit();
	protected:
		virtual void save_fetch(memPara para) = 0;
		//fetchInit 与 构造函数的区别：
		//fetchInit每被指一次就执行一次，构造函数只会执行一次。
		static constexpr BOOL isFetchInit = FALSE;
		virtual void fetchInit() {};
		void GWPP(const WCHAR* key, WCHAR* varWC, UINT size, memPara para);
		void GWPP(const WCHAR* key, bool& var, memPara para);
		void GWPP(const WCHAR* key, char& var, memPara para);
		void GWPP(const WCHAR* key, INT8& var, memPara para);
		void GWPP(const WCHAR* key, unsigned char& var, memPara para);
		void GWPP(const WCHAR* key, short& var, memPara para);
		void GWPP(const WCHAR* key, unsigned short& var, memPara para);
		void GWPP(const WCHAR* key, INT32& var, memPara para);
		void GWPP(const WCHAR* key, UINT32& var, memPara para);
		void GWPP(const WCHAR* key, LONG& varLONG, memPara para);
		void GWPP(const WCHAR* key, ULONG& varLONG, memPara para);
		void GWPP(const WCHAR* key, INT64& varLONG, memPara para);
		void GWPP(const WCHAR* key, UINT64& varLONG, memPara para);
		void GWPP(const WCHAR* key, FLOAT& var, memPara para);
		void GWPP(const WCHAR* key, double& var, memPara para);
		template<class _enum>
		std::enable_if_t<std::is_enum<_enum>::value, void>
		GWPP(const WCHAR* key, _enum& var, memPara para);
		template<typename memStruct>
		std::enable_if_t<!is_based_on_memPtr<memStruct>::value && !std::is_enum<memStruct>::value, void>
		GWPP(const WCHAR* key, memStruct& varMU, memPara para);
		template<class mu, bool releaseable>
		void GWPP(const WCHAR* key, memPtr<mu, releaseable>& varMU, memPara para);
		template<typename First, typename...Args>
		void GWPP(const WCHAR* key, pVariant<First, Args...>& varGe, memPara para);
		template<class ptr_in_Vec>
		void GWPP_Array(std::vector<ptr_in_Vec>* vec, memPara para);
	private:
		template<class mu, bool releaseable>
		void GWPP_Passive(const WCHAR* key, memPtr<mu, releaseable>& varMU, memPara para);
		inline memUnit* operator&()
		{
			return NULL;
		}
		//如果pVariant指定了memManager或非memUnit为参数，这里就会报错										The foolish package manager in C++ forced me to write this foolish function.
		template<class mu> inline static mu* getNewClass(memManager* m) { return new mu(m); }
	};
	class memManager :public memUnit {
	public:
		template<class mu, bool releaseable> friend class memPtr;
		friend class Ingress;
		friend class Subfile;
		friend class memUnit;
		memManager();
		virtual ~memManager();
		WCHAR url[maxURL];
		BOOL download();
		BOOL upload();
		template<class mu> memPtr<Egress> makeEgress_IngressPair(const memPtr<mu>& target, const WCHAR* kw);
		memPtr<Subfile> findSubfile(const WCHAR* url);
		memPtr<Egress> findEgress(const memPtr<Subfile> subfile, const WCHAR* kw, const WCHAR* type);
		memPtr<Ingress> findIngress(const WCHAR* kw, const WCHAR* type);
		memPtr<Ingress> findIngressDangling();
		inline void setUrl(const WCHAR* wcptr) {
			wcscpy_s(url, wcptr);
		}
		// 如果url为空则返回nullptr
		inline WCHAR* getFileName()
		{
			if (url[0])
				return &wcsrchr(url, L'\\')[1];
			return nullptr;
		}
		template<SHORT para_order = ORD_DESERIALIZE>
		bool deserialize(WCHAR* Ptr, UINT StringSize);
		template<SHORT para_order = ORD_SERIALIZE>
		void serialize(std::vector<WCHAR>* bc);
	private:
		void thisCons();
		void thisDest();
		virtual void save_fetch(memPara para) = 0;
		std::set<memUnit*>* memUnits;
		memVector<Subfile>* subFiles;
		memVector<Ingress>* ingressInte;
		//
		std::set<memPtrCorr>* ptrTable = NULL;
		std::vector<memApp*>* findPtr(memUnit* ptrINI);
		std::vector<memApp*>* findPtr(memUnit* ptrINI, memPtr<memUnit>* self);
		std::vector<memApp*>* findPtr(memUnit* ptrINI, memPtrCorr** ptr);
		std::vector<memApp*>* pushPtr(memUnit* ptrINI);
		VOID deletePtrTable();
		void RawSerialize(std::vector<WCHAR>* bc);
		void RawDeserialize(WCHAR* seriStr, UINT size);
	};
	template<class mu, bool releaseable = true> using pmemVector = memPtr<memVector<mu, releaseable>>;
	template<class cast> using pmemVectorEgress = memPtr<memVectorEgress<cast>>;
	template<typename... Args> using pmemVectorVariant = memPtr<memVectorVariant<Args...>>;
	//基本数据单元的vector
	//与STL的vector有区别：memVector中，在template中填基于memUnit的派生类的名称，容器中存的是此派生类的memPtr
	template<class mu, bool releaseable>
	class memVector :public std::vector<memPtr<mu, releaseable>> ,public memUnit {
	protected:
		inline void save_fetch(memPara para)override {
			GWPP_Array(this, para);
		}
		friend class memUnit;
		friend class memManager;
	public:
		inline memVector(memManager* manager) :memUnit(manager) {
			static_assert(std::is_base_of_v<memUnit, mu>, "template is not Base on memUnit");
		}
	};
	class memWcs {
		void save_fetch_struct(memUnit* mu, const WCHAR* key, memPara para);
		friend class memUnit;
		std::vector<WCHAR> wc;
	public:
		memWcs() { wc = std::vector<WCHAR>(defaultWcs); }
		inline virtual ~memWcs(){}
		inline memWcs(const WCHAR* init, UINT initSize)
		{
			wc = std::vector<WCHAR>(initSize);
			wcscpy_s(&wc[0], initSize, init);
		}
		inline void operator=(const WCHAR* wcptr) {
			if (wcslen(wcptr) + 1 > wc.size())
			{
				wc.resize(wcslen(wcptr) + 1);
			}
			wcscpy_s(&wc[0], wc.capacity(), wcptr);
		}
		//此指针不得长期保存
		inline WCHAR* operator*()
		{
			return &wc[0];
		}
		inline WCHAR* getWcharPtr()
		{
			return &wc[0];
		}
		inline size_t capacity()
		{
			return wc.size();
		}
		inline VOID resize(const size_t size)
		{
			wc.resize(size);
		}
	};
	struct memPtrCorr {
		memUnit* ptrINI;
		memPtr<memUnit>* ptrRUN;
		std::vector<memApp*>* appSegment;
		inline bool operator<(const memPtrCorr& ptr) const
		{
			return ptr.ptrINI < ptrINI;
		}
	};
	struct memApp
	{
		//千万不要改成map，会慢
		WCHAR key[maxKey];
		std::vector<WCHAR>* value;
	};
	template<class cast> struct pEgress;
	class Egress :public memUnit {
		friend class memUnit;
		friend class memManager;
		friend class Subfile;
		friend class Ingress;
		template<class cast> friend struct pEgress;
		inline Egress(memManager* m) :memUnit(m) {};
		inline Egress(memManager* m, Subfile* upLevel) :memUnit(m) {
			ptrUpLevel = upLevel;
		};
		~Egress();
		WCHAR keywords[maxKW];
		WCHAR type[maxKW];
		void* ptrDirect = NULL;
		Ingress* ptrInge = NULL;
		Subfile* ptrUpLevel;	//不要担心fetch时未定义，其会在SubFile.save_fetch中载入值。
		inline void save_fetch(memPara para) {
			GWPP(L"keyword", keywords, maxKW, para);
			GWPP(L"type", type, maxKW, para);
		}
		template<typename cast>
		UINT getTarget(cast*& varReturn);
	};
	class Ingress :public memUnit {
		friend class memUnit;
		friend class memManager;
		friend class Egress;
		inline Ingress(memManager* m) :memUnit(m) {};
		inline ~Ingress()
		{
			for (auto i : pointedByEgress)
			{
				i->ptrDirect = NULL;
				i->ptrInge = NULL;
			}
			memManager* m = this->mngr;
			if (this->getManager())
			{
				auto i = find(this->getManager()->ingressInte->begin(), this->getManager()->ingressInte->end(), this);
				if (*i == this)
				{
					this->mngr = NULL;
					m->ingressInte->erase(i);
				}
			}
			this->mngr = m;
		}
		WCHAR keywords[maxKW];
		WCHAR type[maxKW];
		memPtr<memUnit> mu;
		std::vector<Egress*> pointedByEgress;
		inline void save_fetch(memPara para)
		{
			//GWPP(L"keyword", keywords, maxKW, para);
			//GWPP(L"type", type, maxKW, para);
			GWPP_Passive(L"mu", mu, para);
		}
	};
	class Subfile :private memVector<Egress> {
		friend class memUnit;
		friend class memManager;
		friend class Egress;
		inline Subfile(memManager* m) :memVector<Egress>(m) {};
		inline ~Subfile()
		{
			memManager* m = this->mngr;
			if (this->getManager())
			{
				auto i = find(this->getManager()->subFiles->begin(), this->getManager()->subFiles->end(), this);
				if (*i == this)
				{
					this->mngr = NULL;
					m->subFiles->erase(i);
				}
			}
			this->mngr = m;
			for (int i = 0; i < this->size(); i++)
			{
				memVector<Egress>::operator[](i).release();
			}
		}
		WCHAR fileName[maxURL];
		memPtr<memManager> ptrManager = NULL;
		inline void save_fetch(memPara para) override{
			memVector<Egress>::save_fetch(para);
			if(para.order == ORD_FETCH)
				for (auto i : *this)
					i->ptrUpLevel = this;
			GWPP(L"fileName", fileName, maxURL, para);
		}
		//1 for success, 0 for fail
		inline int findGlobalManager()
		{
			WaitForSingleObject(global_mutex, INFINITE);
			for (auto i : global_load)
			{
				WCHAR* fn = i->getFileName();
				if (fn == nullptr)
					continue;
				if (wcscmp(fn, fileName) == 0)
				{
					ptrManager = i;
					ReleaseMutex(global_mutex);
					return 1;
				}
			}
			ReleaseMutex(global_mutex);
			return 0;
		}
	};
	//egress smart pointer wrapped by template
	template<class cast>
	struct pEgress :public memPtr<Egress> {
		inline void getFileName(WCHAR* wc, UINT length)
		{
			UINT i = length > maxURL ? maxURL : length;
			Subfile& subf = *(Egress::ptrUpLevel);
			wcscpy_s(wc, i, subf.fileName);
			return;
		}
		//getTarget在全局查找目标memManager。需要事先将memManager upload到内存，才能get
		inline UINT getTarget(cast*& varReturn) {
			if (memPtr<Egress>::isFilled())
			{
				Egress& egr = *(Egress*)(memPtr<Egress>::ptr->content);
				return egr.getTarget(varReturn);
			}
			varReturn = NULL;
			return MEM_EMPTY_EGRESS;
		}
		//被指的memUnit以memPtr形式传参，实际被Egress指向的是memPtr所指的memUnit。
		inline void makeEIPair(memManager* egressMngr, const memPtr<cast>& target, const WCHAR* kw) {
			memPtr<Egress> pegr = egressMngr->makeEgress_IngressPair(target, kw);
			this->memPtr<Egress>::operator=(pegr);
		}
		inline pEgress() {}
		inline pEgress(memManager* egressMngr, const memPtr<cast>& target, const WCHAR* kw) {
			this->makeEIPair(egressMngr, target, kw);
		}
	};
	template<class cast>
	class memVectorEgress :public std::vector<pEgress<cast>>, public memUnit {
	protected:
		inline void save_fetch(memPara para) override {
			GWPP_Array(this, para);
		}
		friend class memUnit;
		friend class memManager;
	public:
		inline memVectorEgress(memManager* manager) :memUnit(manager) {
			static_assert(std::is_base_of_v<memUnit, cast>, "template is not Base on memUnit");
		}
	};
	template<typename First, typename...Args>
	class pVariant final {				//不会有人要继承pVariant吧？想被模板搞死吗？
		UINT type;							//从1开始，不是从0开始
		memPtr<memUnit> ptr;
		template<typename Target, UINT i>
		constexpr inline UINT getTypeIter()
		{
			assert(!("template not match."));
			return 0;
		}
		template<typename Target, UINT i, typename IterFirst, typename... IterArgs>
		constexpr inline UINT getTypeIter()
		{
			static_assert(std::is_base_of_v<memUnit, IterFirst>, "template is not Base of memUnit");
			if constexpr (std::is_same_v<IterFirst, Target>)
			{
				return i;
			}
			else
			{
				return getTypeIter<Target, i + 1, IterArgs...>();
			}
		}
		template<typename Target>
		inline void setType()
		{
			static const UINT targetType = getTypeIter<Target, 1, First, Args...>();
			type = targetType;
		}
		template<bool _void>
		inline void createIter(UINT i, memManager* mngr)
		{
			assert(!("An error has happened when creating pVariant from an archive file. \n Matched type not found."));
		}
		template<bool _void, typename IterFirst, typename... IterArgs>
		inline void createIter(UINT i, memManager* mngr)
		{
			static_assert(std::is_base_of_v<memUnit, IterFirst>, "template is not Base of memUnit");
			if (i == 1)
			{
				ptr = memUnit::getNewClass<IterFirst>(mngr);
			}
			else
			{
				createIter<false, IterArgs...>(i - 1, mngr);
			}
			return;
		}
		inline void createClassByTypeID(memManager* mngr)
		{
			createIter<false, First, Args...>(type, mngr);
		}
	public:
		friend class memUnit;
		friend class ReflectResultKeyValue;
		inline pVariant()
		{
			type = 0;
			ptr = nullptr;
		}
		inline pVariant(std::nullptr_t) {
			type = 0;
			ptr = nullptr;
		}
		template<typename mu> inline pVariant(mu* pmu)
		{
			static_assert(std::is_base_of_v<memUnit, mu>, "template is not Base of memUnit");
			setType<mu>();
			ptr = memPtr<memUnit>(pmu);
		}
		template<typename mu, bool releaseable> inline pVariant(const memPtr<mu, releaseable>& mp)
		{
			setType<mu>();
			ptr.equalAny(mp);
		}
		//ptr will execute its destructor automatically
		template<typename mu, bool releaseable> inline void operator=(const memPtr<mu, releaseable>& mp)
		{
			setType<mu>();
			ptr.cdd();
			if (mp.isEmpty())
			{
				ptr.ptr = NULL;
			}
			else
			{
				ptr.ptr = mp.ptr;
				(ptr.ptr->count)++;
			}
		}
		template<typename mu> inline void operator=(mu* pmu)
		{
			static_assert(std::is_base_of_v<memUnit, mu>, "template is not Base of memUnit");
			setType<mu>();
			ptr.cdd();
			if (pmu)
			{
				if (((memUnit*)pmu)->sharedPtr)
				{
					ptr.ptr = ((memUnit*)pmu)->sharedPtr;
					(ptr.ptr->count)++;
				}
				else
				{
					ptr.ptr = new memPtrComm((memUnit*)pmu);
					((memUnit*)pmu)->sharedPtr = ptr.ptr;
				}
			}
			else
			{
				ptr.ptr = NULL;
			}
		}
		inline void operator=(const UINT i)
		{
			assert(i == 0 || !("The parameter cannot be non-zero int. This overload is used to clean the class."));
			type = 0;
			ptr = NULL;
		}
		inline bool operator==(const pVariant<First, Args...> mp)const {
			return ptr == mp.ptr;
		}
		inline bool isEmpty() const
		{
			return ptr.isEmpty();
		}
		inline bool isFilled() const
		{
			return ptr.isFilled();
		}
		template<typename Target> inline bool getPtr(Target*& target)
		{
			const UINT pointerType = getTypeIter<Target, 1, First, Args...>();
			if (type && pointerType == type)
			{
				target = (Target*)ptr.operator*();
				return 1;
			}
			else
			{
				return 0;
			}
		}
		inline void release()
		{
			type = 0;
			ptr.release();
		}
	};
	//tuple
	template<typename... Args> class memVectorVariant :public std::vector<pVariant<Args...>>, public memUnit {
	protected:
		inline void save_fetch(memPara para) override{
			GWPP_Array(this, para);
		}
		friend class memUnit;
	public:
		inline memVectorVariant(memManager* manager) :memUnit(manager) {}
	};
	//variant of function pointer
	template<typename illg, int ID = 0> class pFunction { static_assert(!std::is_same_v<illg, illg>, "not a function"); };
	template<typename Ret, typename ...Args, int ID> class pFunction<Ret(Args...), ID> {
		friend class memUnit;
		using funcPtr = Ret(*)(Args...);
		UINT type;
		inline void save_fetch_struct(memUnit* mu, const WCHAR* key, memPara para)
		{
			GWPP_Struct(mu, key, L"f:", type, para);
		}
		inline static constexpr UINT getSizeofFunctions()
		{
			//Functions相当于此pFunction的variant参数。 
			//如果没有初始化此类模板实例的Functions，那么这里将会报错
			static constexpr UINT ret = sizeof(Functions);
			return ret;
		}
	public:
		static const funcPtr Functions[];
		inline Ret operator()(Args...args)const {
			assert(type < getSizeofFunctions() / sizeof(funcPtr) || !("pFunction is empty or invaild"));
			return Functions[type](std::forward<Args>(args)...);
		}
		inline pFunction() { type = 0xffffffff; }
		inline pFunction(funcPtr newPtr) { this->operator=(newPtr); }
		inline void operator=(funcPtr newPtr) {
			//函数指针居然没法constexpr，造孽
			for (UINT i = 0; i < getSizeofFunctions() / sizeof(funcPtr); i++)
			{
				if (newPtr == Functions[i]) {
					type = i;
					return;
				}
			}
			assert(false || !("this function is not match in function list"));
		}
		inline bool operator==(const pFunction<Ret(Args...), ID> mp)const {
			if (type == mp.type)
				return true;
			return false;
		}
		inline bool operator==(funcPtr ptr)const {
			assert(type < getSizeofFunctions() / sizeof(funcPtr) || !("pFunction is empty or invaild"));
			if (ptr == Functions[type])
				return true;
			return false;
		}
		inline bool isFilled()const {
			return type < getSizeofFunctions() / sizeof(funcPtr);
		}
		inline bool isEmpty()const {
			return !this->isFilled();
		}
	};
	//用于初始化pFunction内部的variant的宏
#define INITIALIZE_PFUNCTION(Signature,ID, ...) template<> const mem::pFunction<Signature,ID>::funcPtr mem::pFunction<Signature,ID>::Functions[] = { __VA_ARGS__ };
	inline BOOL GetPrivateProfileStringW(std::vector<memApp*>* lpAppSegment, LPCTSTR lpKeyName, DWORD nSize, LPCTSTR lpString);
	inline BOOL GetPrivateProfileStringW(std::vector<memApp*>* lpAppSegment, LPCTSTR lpKeyName, std::vector<WCHAR>& wc);
	inline BOOL WritePrivateProfileStringW(std::vector<memApp*>* lpAppSegment, LPCTSTR lpKeyName, LPCTSTR lpString);
	inline void PushWcharVector(std::vector<WCHAR>* bc, const WCHAR* wc, UINT len);
	
	//基本数据单元
	//所有需要存储到硬盘的类，必须继承自此类。
	inline memUnit::memUnit(memManager* manager) {
		//if manager is null the memUnit is a memManager
		if (manager)
		{
			manager->memUnits->insert(this);
			mngr = manager;
		}
		sharedPtr = NULL;
	}
	inline memUnit::~memUnit()
	{
		if (mngr)
		{
			auto i = mngr->memUnits->find(this);
			if (*i == this)
				mngr->memUnits->erase(i);
		}
		if (sharedPtr)
			sharedPtr->content = NULL;
	}



	//读写/反射/序列化一体函数
	inline void memUnit::GWPP(const WCHAR* key, WCHAR* varWC, UINT size, memPara para)
	{
		assert(wcslen(key) < maxKey || !("variable key is too long."));
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:
			GetPrivateProfileStringW(para.appSegment, key, size, varWC);
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:
			WritePrivateProfileStringW(para.appSegment, key, varWC);
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, varWC);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, varWC, size);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, bool& var, memPara para)
	{
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_SERIALIZE:
		case ORD_FETCH:
		case ORD_SAVE:
			GWPP(key, (char&)var, para);
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, char& var, memPara para)
	{
		assert(wcslen(key) < maxKey || !("variable key is too long."));
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		{
			WCHAR wc[5];
			GetPrivateProfileStringW(para.appSegment, key, 5, wc);
			var = _wtoi(wc);
		}
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:
		{
			WCHAR wc[5];
			_itow_s(var, wc, 10);
			WritePrivateProfileStringW(para.appSegment, key, wc);
		}
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, unsigned char& var, memPara para)
	{
		switch (para.order)
		{
		case ORD_SERIALIZE:
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		case ORD_SAVE:
			GWPP(key, (char&)var, para);
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, INT8& var, memPara para)
	{
		switch (para.order)
		{
		case ORD_SERIALIZE:
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		case ORD_SAVE:
			GWPP(key, (char&)var, para);
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, short& var, memPara para)
	{
		assert(wcslen(key) < maxKey || !("variable key is too long."));
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		{
			WCHAR wc[7];
			GetPrivateProfileStringW(para.appSegment, key, 7, wc);
			var = _wtoi(wc);
		}
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:
		{
			WCHAR wc[7];
			_itow_s(var, wc, 10);
			WritePrivateProfileStringW(para.appSegment, key, wc);
		}
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, unsigned short& var, memPara para)
	{
		switch (para.order)
		{
		case ORD_SERIALIZE:
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		case ORD_SAVE:
			GWPP(key, (short&)var, para);
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, INT32& var, memPara para)
	{
		assert(wcslen(key) < maxKey || !("variable key is too long."));
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		{
			WCHAR wc[12];
			GetPrivateProfileStringW(para.appSegment, key, 12, wc);
			var = _wtoi(wc);
		}
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:
		{
			WCHAR wc[12];
			_itow_s(var, wc, 10);
			WritePrivateProfileStringW(para.appSegment, key, wc);
		}
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, UINT32& var, memPara para)
	{
		switch (para.order)
		{
		case ORD_SERIALIZE:
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		case ORD_SAVE:
			GWPP(key, (INT32&)var, para);
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, LONG& var, memPara para)
	{
		switch (para.order)
		{
		case ORD_SERIALIZE:
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		case ORD_SAVE:
			GWPP(key, (INT32&)var, para);
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, ULONG& var, memPara para)
	{
		switch (para.order)
		{
		case ORD_SERIALIZE:
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		case ORD_SAVE:
			GWPP(key, (INT32&)var, para);
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, INT64& varLONG, memPara para)
	{
		switch (para.order)
		{
		case ORD_SERIALIZE:
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		case ORD_SAVE:
			GWPP(key, (UINT64&)varLONG, para);
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, varLONG);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, varLONG);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, UINT64& varLONG, memPara para)
	{
		assert(wcslen(key) < maxKey || !("variable key is too long."));
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		{
			WCHAR wc[22];
			GetPrivateProfileStringW(para.appSegment, key, 22, wc);
			varLONG = _wtoll(wc);
		}
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:
		{
			WCHAR wc[22];
			_lltow_s(varLONG, wc, 10);
			WritePrivateProfileStringW(para.appSegment, key, wc);
		}
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, varLONG);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, varLONG);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, float& var, memPara para)
	{
		assert(wcslen(key) < maxKey || !("variable key is too long."));
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		{
			WCHAR wc[12];
			GetPrivateProfileStringW(para.appSegment, key, 12, wc);
			int a = _wtoi(wc);
			var = reinterpret_cast<float&>(a);
		}
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:
		{
			WCHAR wc[12];
			_itow_s(reinterpret_cast<int&>(var), wc, 10);
			WritePrivateProfileStringW(para.appSegment, key, wc);
		}
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, double& var, memPara para)
	{
		assert(wcslen(key) < maxKey || !("variable key is too long."));
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		{
			WCHAR wc[22];
			GetPrivateProfileStringW(para.appSegment, key, 22, wc);
			long long a = _wtoll(wc);
			var = reinterpret_cast<double&>(a);
		}
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:
		{
			WCHAR wc[22];
			_itow_s(reinterpret_cast<long long&>(var), wc, 10);
			WritePrivateProfileStringW(para.appSegment, key, wc);
		}
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
		}
	}
	//读写枚举
	template<class _enum>
	inline std::enable_if_t<std::is_enum<_enum>::value, void>
	memUnit::GWPP(const WCHAR* key, _enum& var, memPara para) {
		GWPP(key, reinterpret_cast<std::underlying_type<_enum>::type&>(var), para);
	}
	//读写自定义结构体。要求该结构体实现save_fetch_struct方法
	template<typename memStruct>
	//检查函数是否为memPtr的子类，若是则禁用此模板函数
	inline std::enable_if_t<!is_based_on_memPtr<memStruct>::value && !std::is_enum<memStruct>::value, void>
	memUnit::GWPP(const WCHAR* key, memStruct& varST, memPara para)
	{
		//自定义结构体若没有save_fetch_struct()方法，这里会报错。
		varST.save_fetch_struct(this, key, para);
	}
	//读写指针
	template<class mu, bool releaseable>
	inline void memUnit::GWPP(const WCHAR* key, memPtr<mu, releaseable>&varMU, memPara para)
	{
		assert(wcslen(key) < maxKey || !("variable key is too long."));
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:				//读
		{
			WCHAR wc[20];
			if (GetPrivateProfileStringW(para.appSegment, key, 20, wc) == 0)
			{
				varMU = memPtr<mu, releaseable>();
				return;
			}
			memUnit* ptrINI = (memUnit*)_wtop(wc);
			memPtrCorr* ptr;
			para.appSegment = this->mngr->findPtr(ptrINI, &ptr);
			if (ptr->ptrRUN)
			{
				varMU = *((memPtr<mu, releaseable>*)ptr->ptrRUN);
			}
			else
			{
				varMU = new mu(this->mngr);
				ptr->ptrRUN = (memPtr<memUnit>*)varMU.getPtr();
				varMU->save_fetch(para);
			}
			if (varMU->isFetchInit)
				varMU->fetchInit();
		}
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:				//写
		{
			if (varMU.isEmpty())return;
			assert(varMU->mngr == this->mngr || !("False: memUnit`s memManager cannot be cross."));
			WCHAR wc[20];
			_ptow_s((POINTER_L)(varMU.ptr->content), wc, 10);
			WritePrivateProfileStringW(para.appSegment, key, wc);
			if (this->mngr->findPtr(varMU.ptr->content) == NULL)
			{
				para.appSegment = this->mngr->pushPtr(varMU.ptr->content);
				varMU->save_fetch(para);
			}
		}
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, varMU);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, varMU);
			break;
		}
	}
	//读写泛型指针
	template<typename First, typename...Args>
	inline void memUnit::GWPP(const WCHAR* key, pVariant<First, Args...>& varGe, memPara para)
	{
		assert(wcslen(key) < maxKey || !("variable key is too long."));
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		{
			WCHAR wc[20];
			if (GetPrivateProfileStringW(para.appSegment, key, 20, wc) == 0)
			{
				varGe = NULL;
				return;
			}
			WCHAR* wcPtr = wcschr(wc, L'g') + 1;
			memUnit* ptrINI = (memUnit*)_wtop(wcPtr);
			*(wcPtr - 1) = L'\0';
			memPtrCorr* ptr;
			para.appSegment = this->mngr->findPtr(ptrINI, &ptr);
			varGe.type = _wtol(wc);
			if (ptr->ptrRUN)
			{
				varGe.ptr = *(ptr->ptrRUN);
			}
			else
			{
				varGe.createClassByTypeID(this->mngr);
				ptr->ptrRUN = (memPtr<memUnit>*)varGe.ptr.getPtr();
				varGe.ptr->save_fetch(para);
			}
			if (varGe.ptr->isFetchInit)
				varGe.ptr->fetchInit();
		}
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:
		{					//写
			if (varGe.isEmpty())return;
			assert(varGe.ptr->mngr == this->mngr || !("False: memUnit`s memManager cannot be cross."));
			WCHAR wc[20];
			WCHAR typeWc[20];
			_ptow_s((POINTER_L)(varGe.ptr.ptr->content), wc, 10);
			_ltow_s((UINT)(varGe.type), typeWc, 10);
			UINT8 len = wcslen(typeWc);
			typeWc[len] = L'g';
			typeWc[len + 1] = L'\0';
			wcscat_s(typeWc, wc);
			WritePrivateProfileStringW(para.appSegment, key, typeWc);
			if (this->mngr->findPtr(varGe.ptr.ptr->content) == NULL)
			{
				para.appSegment = this->mngr->pushPtr(varGe.ptr.ptr->content);
				varGe.ptr->save_fetch(para);
			}
		}
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, varGe);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, varGe);
			break;
		}
	}
	//读写数组
	template<class ptr_in_Vec>
	inline void memUnit::GWPP_Array(std::vector<ptr_in_Vec>* vec, memPara para)
	{
		WCHAR wc[10];
		unsigned int k = 0;
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		{
			this->GWPP(L"size", k, para);
			vec->resize(k);
			for (unsigned int i = 0; i < k; i++)
			{
				_itow_s(i, wc, 10);
				this->GWPP(wc, vec->at(i), para);
			}
		}
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:
		{
			for (auto i : *vec)
			{
				if (i.isFilled())
				{
					_itow_s(k, wc, 10);
					this->GWPP(wc, i, para);
					k++;
				}
			}
			this->GWPP(L"size", k, para);
		}
			break;
		case ORD_REFLECTION_R:
		{
			for (auto i : *vec)
			{
				if (i.isFilled())
				{
					_itow_s(k, wc, 10);
					para.reflection->context.emplace_back(wc, i);
					k++;
				}
			}
			para.reflection->context.emplace_back(L"size", k);
		}
			break;
		case ORD_REFLECTION_W:
		{
			for (unsigned int i = 0; i < vec->size(); i++)
			{
				_itow_s(i, wc, 10);
				para.reflection_single->WriteMU(wc, vec->at(i));
			}
		}
			break;
		}
	}
	//基本数据单元的wchar[]
	inline void memWcs::save_fetch_struct(memUnit* mu, const WCHAR* key, memPara para) {
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:
			GetPrivateProfileStringW(para.appSegment, key, wc);
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:
			mu->GWPP(key, &wc[0], wc.size(), para);
			break;
		case ORD_REFLECTION_R:
			mu->GWPP(key, &wc[0], wc.size(), para);
			break;
		case ORD_REFLECTION_W:
		{
			if (wcscmp(para.reflection_single->key, key) == 0)
			{
				UINT lenWc = wcslen(para.reflection_single->value.wchar) + 1;
				if (lenWc > wc.size())
				{
					wc.resize(lenWc);
				}
				para.reflection_single->key[0] = L'\0';
				wcscpy_s(&wc[0], wc.size(), para.reflection_single->value.wchar);
			}
		}
			break;
		}
	}
	//（内部函数）被动式读写指针：只在现有数据中找，找不到就赋空值。
	template<class mu, bool releaseable>
	inline void memUnit::GWPP_Passive(const WCHAR* key, memPtr<mu, releaseable>& varMU, memPara para)
	{
		assert(wcslen(key) < maxKey || !("variable key is too long."));
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		{
			WCHAR wc[20];
			if (GetPrivateProfileStringW(para.appSegment, key, 20, wc) == 0)
			{
				varMU = memPtr<mu, releaseable>();
				return;
			}
			memUnit* ptrINI = (memUnit*)_wtop(wc);
			memPtrCorr* ptr;
			para.appSegment = mngr->findPtr(ptrINI, &ptr);
			if (ptr->ptrRUN)
			{
				varMU.ptr = ptr->ptrRUN->ptr;
				varMU.ptr->count++;
				if (varMU->isFetchInit)
					varMU->fetchInit();
			}
		}
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:
		{
			if (varMU.isEmpty())return;
			if (mngr->findPtr(varMU.ptr->content))
			{
				WCHAR wc[20];
				_ptow_s((POINTER_L)(varMU.ptr->content), wc, 10);
				WritePrivateProfileStringW(para.appSegment, key, wc);
			}
		}
			break;
		case ORD_REFLECTION_R:
			break;
		case ORD_REFLECTION_W:
			break;
		}
	}



	//此全局函数用于复制单个memUnit，且不能跨越memManager
	template<class mu, bool releaseable>
	inline mu* memUnit::memCopy(const memPtr<mu, releaseable>& varMU)
	{
		static_assert(std::is_base_of_v<memUnit, mu>, "template is not Base of memUnit");
		static_assert(std::is_base_of_v<memManager, mu> == false, "cannot be Base of memManager. Please use memManager->copy() to copy whole memManager.");
		mu& varMuReference = **varMU;
		mu* ret = new mu(varMuReference);
		ret->sharedPtr = NULL;
		varMU->getManager()->memUnits->insert(ret);
		return ret;
	}

	//可主动release的智能指针
	//不带线程锁，需要时再写
	template<class mu, bool releaseable>
	inline void memPtr<mu, releaseable>::cdd(memPtrComm* ptrOperation)
	{
		if (ptrOperation)
			if (ptrOperation->count)
			{
				if (ptrOperation->count == 1)
				{
					if (ptrOperation->content)
						if (ptrOperation->content->mngr)
							delete(ptrOperation->content);
					delete ptrOperation;
					ptrOperation = NULL;
				}
				else
					(ptrOperation->count)--;
			}
	}
	template<class mu, bool releaseable>
	inline void memPtr<mu, releaseable>::operator=(const memPtr<mu, releaseable>& mp)
	{
		//static_assert(&mp != this, "Equals self results undefined behavior.");
		cdd();
		if (mp.isEmpty())
		{
			ptr = NULL;
		}
		else
		{
			ptr = mp.ptr;
			(ptr->count)++;
		}
	}
	//赋memUnit裸指针
	template<class mu, bool releaseable>
	inline void memPtr<mu, releaseable>::operator=(mu* pmu)
	{
		cdd();
		if (pmu)
		{
			if (((memUnit*)pmu)->sharedPtr)
			{
				ptr = ((memUnit*)pmu)->sharedPtr;
				(ptr->count)++;
			}
			else
			{
				ptr = new memPtrComm((memUnit*)pmu);
				((memUnit*)pmu)->sharedPtr = ptr;
			}
		}
		else
		{
			ptr = NULL;
		}
	}
	//交换
	template<class mu, bool releaseable>
	inline void memPtr<mu, releaseable>::swap(memPtr& sw)
	{
		memPtrComm* i = ptr;
		ptr = sw.ptr;
		sw.ptr = i;
	}

	//内存与硬盘管理器
	//一个此类实例 == 一个硬盘文件
	//其本身亦充当基本数据单元。
	inline memManager::memManager() :memUnit(NULL)
	{
		thisCons();
		memset(this->url, 0, sizeof(this->url));
		this->mngr = this;
		if(global_mutex == NULL)
			global_mutex = CreateMutex(nullptr, NULL, nullptr);
		WaitForSingleObject(global_mutex, INFINITE);
		global_load.push_back(this);
		ReleaseMutex(global_mutex);
	}
	inline memManager::~memManager()
	{
		WaitForSingleObject(global_mutex, INFINITE);
		for (auto i = global_load.begin(); i != global_load.end(); i++)
		{
			if (*i == this)
			{
				global_load.erase(i);
				break;
			}
		}
		ReleaseMutex(global_mutex);
		this->mngr = NULL;
		for (auto i : *memUnits)
		{
			i->mngr = NULL;
			if (i->sharedPtr)						//这里不能优化。理由见memPtr的release
				i->sharedPtr->content = NULL;
			delete i;
		}
		thisDest();
	}
	inline void memManager::thisCons()
	{
		memUnits = new std::set<memUnit*>();
		subFiles = new memVector<Subfile>(this);
		ingressInte = new memVector<Ingress>(this);
	}
	inline void memManager::thisDest()
	{
		delete memUnits;
		//subFiles and ingressInte are also memUnit that will be destructed when all of memUnit are destructed.
	}
	inline VOID memManager::deletePtrTable()
	{
		for (auto i : *ptrTable)
		{
			if (i.appSegment)
			{
				for (auto k : *i.appSegment)
				{
					if (k)
					{
						if (k->value)
							delete k->value;
						delete k;
					}
				}
				delete i.appSegment;
			}
		}
		delete ptrTable;
		ptrTable = NULL;
		return;
	}
	//save，下载到硬盘，成功返回true，失败返回false
	inline BOOL memManager::download()
	{
		DeleteFileW(url);

		std::vector<WCHAR> bc = {};
		bc.reserve(1000);

		this->serialize<ORD_SAVE>(&bc);

		HANDLE hFile = CreateFile(url, FILE_GENERIC_READ | FILE_GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (!hFile)
		{
			return 0;
		}
		UINT dwErr = GetLastError();

		WORD unicode_identifier = 0xfeff;
		WriteFile(hFile, &bc[0], bc.size() * sizeof(WCHAR), NULL, NULL);

		CloseHandle(hFile);

		return 1;
	}
	//fetch，上传到内存，成功返回true，失败返回false，若失败不会影响原内容。
	inline BOOL memManager::upload()
	{
		HANDLE hFile = CreateFile(url, FILE_GENERIC_READ | FILE_GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (!hFile)
		{
			return 0;
		}
		DWORD fileSizeH, fileSizeL, sizeRead = 0;
		fileSizeL = GetFileSize(hFile, &fileSizeH);
		UINT64 fileSize = fileSizeH;
		fileSize = (fileSize << 32);
		fileSize += fileSizeL;
		if (!fileSize)
		{
			CloseHandle(hFile);
			return 0;
		}

		WCHAR* bytes = (WCHAR*)calloc(1, fileSize + 512);
		BOOL ret = false;
		if (ReadFile(hFile, bytes, fileSize, &sizeRead, NULL))
		{
			CloseHandle(hFile);
			ret = this->deserialize<ORD_FETCH>(bytes, fileSize / sizeof(WCHAR));
		}
		free(bytes);
		return ret;
	}
	//根据url查找subfile
	inline memPtr<Subfile> memManager::findSubfile(const WCHAR* fileUrl)
	{
		assert(fileUrl || !("target manager's url cannot be null"));
		for (auto i : *subFiles)
		{
			if (wcscmp(fileUrl, i->fileName) == 0)
				return i;
		}
		return NULL;
	}
	//根据subfile、keyword、type查找出口
	inline memPtr<Egress> memManager::findEgress(const memPtr<Subfile> subfile, const WCHAR* kw, const WCHAR* type)
	{
		for (auto k : **subfile)
		{
			if (wcscmp(k->keywords, kw) == 0 && wcscmp(k->type, type) == 0)
				return k;
		}
		return NULL;
	}
	//根据keyword、type查找入口
	inline memPtr<Ingress> memManager::findIngress(const WCHAR* kw, const WCHAR* type)
	{
		for (auto i : *ingressInte)
		{
			if (wcscmp(i->keywords, kw) == 0 && wcscmp(i->type, type) == 0)
				return i;
		}
		return NULL;
	}
	//查找悬空入口。在release掉任何有可能成为入口的memUnit后，都应该执行一次此方法。
	inline memPtr<Ingress> memManager::findIngressDangling()
	{
		for (auto i : *ingressInte)
		{
			if (i->mu.isEmpty())
				return i;
			if (i->mu.ptr->count == 1)
				return i;
		}
		return NULL;
	}
	//制造出口-入口对。由出口memManager执行该方法，在目标memManager中制造一个指向目标memUnit的入口，并返回一个出口
	template<class mu>
	[[nodiscard]] inline memPtr<Egress> memManager::makeEgress_IngressPair(const memPtr<mu>& target, const WCHAR* kw)
	{
		const char* typeName = typeid(mu).name();
		WCHAR type[maxKW];
		MultiByteToWideChar(CP_ACP, 0, typeName, -1, type, maxKW);
		assert(target.isFilled() || !("target memUnit is Null."));
		assert(wcslen(kw) < maxKW || !("keyword is too long."));
		assert(wcslen(type) < maxKW || !("type is too long."));

		memManager* targetManager = target->getManager();

		memPtr<Subfile> sf = this->findSubfile(targetManager->getFileName());
		memPtr<Egress> egr = NULL;
		memPtr<Ingress> ingr = targetManager->findIngress(kw, type);
		if (ingr.isEmpty())
		{
			ingr = new Ingress(targetManager);
			wcscpy_s(ingr->keywords, kw);
			wcscpy_s(ingr->type, type);
			targetManager->ingressInte->push_back(ingr);
		}
		else
		{
			ingr->mu.cdd();
		}
		ingr->mu.ptr = target.ptr;
		ingr->mu.ptr->count++;
		if(sf.isEmpty())
		{
			sf = new Subfile(this);
			wcscpy_s(sf->fileName, targetManager->getFileName());
			sf->ptrManager = targetManager;
			this->subFiles->push_back(sf);
		}
		else
		{
			egr = this->findEgress(sf, kw, type);
		}
		if (egr.isEmpty())
		{
			egr = new Egress(this, *sf);
			wcscpy_s(egr->keywords, kw);
			wcscpy_s(egr->type, type);
			sf->push_back(egr);
		}
		if (egr->ptrDirect == NULL)
		{
			egr->ptrInge = *ingr;
			egr->ptrInge->pointedByEgress.push_back(*egr);
			egr->ptrDirect = (void*)*ingr->mu;
		}

		return egr;
	}
	inline std::vector<memApp*>* memManager::findPtr(memUnit* ptrINI)
	{
		memPtrCorr i;
		i.ptrINI = ptrINI;
		auto k = ptrTable->find(i);
		if (k == ptrTable->cend())
			return NULL;
		return (*k).appSegment;
	}
	inline std::vector<memApp*>* memManager::findPtr(memUnit* ptrINI, memPtr<memUnit>* self)
	{
		memPtrCorr i;
		i.ptrINI = ptrINI;
		auto k = ptrTable->find(i);
		if (k == ptrTable->cend())
			return NULL;
		memPtr<memUnit>*& a = const_cast<memPtr<memUnit>*&>((*k).ptrRUN);
		a = self;
		return (*k).appSegment;
	}
	inline std::vector<memApp*>* memManager::findPtr(memUnit* ptrINI, memPtrCorr** ptr)
	{
		memPtrCorr i;
		i.ptrINI = ptrINI;
		auto k = ptrTable->find(i);
		if (k == ptrTable->cend())
			return NULL;
		*ptr = (memPtrCorr*)&(*k);
		return (*k).appSegment;
	}
	inline std::vector<memApp*>* memManager::pushPtr(memUnit* ptrINI)
	{
		memPtrCorr i;
		i.ptrINI = ptrINI;
		i.ptrRUN = NULL;
		i.appSegment = new std::vector<memApp*>();
		ptrTable->insert(i);
		return i.appSegment;
		
	}

	//供自定义结构体的save_fetch_struct使用的GWPP
	template<class T>
	inline void GWPP_Struct(memUnit* mu, const WCHAR* key1, const WCHAR* key2, T& var, memPara para)
	{
		assert(wcslen(key1) + wcslen(key2) < maxKey || !("variable key is too long."));
		WCHAR key[maxKey];
		wcscpy_s(key, key1);
		wcscat_s(key, key2);
		mu->GWPP(key, var, para);
	}

	//硬盘底层操作函数
	inline bool memUnit::deserialize(WCHAR* Ptr, UINT StringSize)
	{
		this->mngr->ptrTable = new std::set<memPtrCorr>();

		//读文件头
		memUnit* mu = (memUnit*)_wtop(Ptr);
		Ptr += wcslen(Ptr) + 1;

		if (!mu)
		{
			this->mngr->deletePtrTable();
			return false;
		}

		this->mngr->RawDeserialize(Ptr, StringSize);

		//进入读写迭代
		memPara mp;
		mp.order = ORD_FETCH;
		memPtr<memUnit> self = this;
		mp.appSegment = this->mngr->findPtr(mu, &self);
		this->save_fetch(mp);

		//清空指针对应表，并返回
		this->mngr->deletePtrTable();
		self.degeneracy();
		return true;
	}
	inline void memUnit::serialize(std::vector<WCHAR>* bc)
	{
		this->mngr->ptrTable = new std::set<memPtrCorr>();

		//写文件头
		WCHAR wc[20];
		_ptow_s((POINTER_L)this, wc, 10);
		PushWcharVector(bc, wc, wcslen(wc) + 1);

		//进入读写迭代
		memPara mp;
		mp.order = ORD_SAVE;
		mp.appSegment = this->mngr->pushPtr(this);
		this->save_fetch(mp);

		this->mngr->RawSerialize(bc);

		//清空指针对应表，并返回
		this->mngr->deletePtrTable();
	}
	template<SHORT para_order>
	inline bool memManager::deserialize(WCHAR* Ptr, UINT StringSize)
	{
		ptrTable = new std::set<memPtrCorr>();

		//分节分割
		memUnit* firstmu = NULL;
		//读文件头
		memUnit* subfile = (memUnit*)_wtop(Ptr);
		WCHAR* ptrByte = Ptr + wcslen(Ptr) + 1;
		memUnit* ingressIn = (memUnit*)_wtop(ptrByte);
		ptrByte += wcslen(ptrByte) + 1;
		firstmu = (memUnit*)_wtop(ptrByte);
		ptrByte += wcslen(ptrByte) + 1;

		this->RawDeserialize(ptrByte, StringSize);

		if (!firstmu || !subfile || !ingressIn)
		{
			deletePtrTable();
			return 0;
		}

		//清空此实例下的所有memUnit并重置此实例，为加载做准备。
		for (auto i : *memUnits)
		{
			i->mngr = NULL;
			if (i->sharedPtr)
				i->sharedPtr->content = NULL;
			delete i;
		}
		thisDest();
		thisCons();

		//进入读写迭代
		memPara mp;
		mp.order = para_order;
		memPtr<memUnit> self = this;
		mp.appSegment = findPtr(firstmu, &self);
		this->save_fetch(mp);

		//加载出、入口接口表
		mp.appSegment = findPtr(subfile);
		this->subFiles->save_fetch(mp);
		mp.appSegment = findPtr(ingressIn);
		this->ingressInte->save_fetch(mp);

		//清空指针对应表，并返回
		deletePtrTable();
		self.degeneracy();
		return 1;
	}
	template<SHORT para_order>
	inline void memManager::serialize(std::vector<WCHAR>* bc)
	{
		ptrTable = new std::set<memPtrCorr>();

		//进入读写迭代
		memPara mp;
		mp.order = para_order;
		mp.appSegment = pushPtr(this);
		this->save_fetch(mp);

		//加载出、入口接口表
		mp.appSegment = pushPtr(this->subFiles);
		this->subFiles->save_fetch(mp);
		mp.appSegment = pushPtr(this->ingressInte);
		this->ingressInte->save_fetch(mp);

		//写文件头
		WCHAR wc[20];
		_ptow_s((POINTER_L)this->subFiles, wc, 10);
		PushWcharVector(bc, wc, wcslen(wc) + 1);
		_ptow_s((POINTER_L)this->ingressInte, wc, 10);
		PushWcharVector(bc, wc, wcslen(wc) + 1);
		_ptow_s((POINTER_L)this, wc, 10);
		PushWcharVector(bc, wc, wcslen(wc) + 1);

		//整体写入
		this->RawSerialize(bc);

		//清空指针对应表，并返回
		deletePtrTable();
	}
	inline void memManager::RawSerialize(std::vector<WCHAR>* bc)
	{
		WCHAR wc[20];
		WCHAR null[2] = {};
		for (auto k : *this->ptrTable)
		{
			_ptow_s((POINTER_L)k.ptrINI, wc, 10);
			PushWcharVector(bc, null, 2);
			PushWcharVector(bc, wc, wcslen(wc) + 1);
			for (auto i : *(k.appSegment))
			{
				PushWcharVector(bc, i->key, wcslen(i->key) + 1);
				PushWcharVector(bc, &i->value->at(0), wcslen(&i->value->at(0)) + 1);
			}
		}
		PushWcharVector(bc, null, 2);
		PushWcharVector(bc, null, 2);
	}
	inline void memManager::RawDeserialize(WCHAR* ptrByte, UINT fileSize)
	{
		ptrByte += 2;
		memPtrCorr i;
		WCHAR* ptrByteEnd = ptrByte + fileSize;
		while (ptrByte < ptrByteEnd)
		{
			i.ptrINI = (memUnit*)_wtop(ptrByte);		//读小节名
			if (i.ptrINI == 0)
				break;
			i.ptrRUN = NULL;
			ptrByte += wcslen(ptrByte) + 1;
			i.appSegment = new std::vector<memApp*>();
			if (*ptrByte == L'\0')						//小节是否为空
			{
				ptrByte += 2;
				this->ptrTable->insert(i);				//小节里面没内容，将小节(memUnit)写入总表中
				if (*ptrByte == L'\0')					//文件读取结束
					break;
				continue;
			}
			while (ptrByte < ptrByteEnd)
			{
				UINT len;
				memApp* ma = new memApp();
				wcscpy_s(ma->key, ptrByte);				//读键
				ptrByte += wcslen(ptrByte) + 1;
				len = wcslen(ptrByte) + 1;
				if (len - 1)							//非空值
				{
					ma->value = new std::vector<WCHAR>(len);
					memcpy_s(&ma->value->at(0), len * sizeof(WCHAR), (void*)ptrByte, len * sizeof(WCHAR));
					ptrByte += len;

				}
				else
				{										//空值
					ma->value = NULL;
					ptrByte++;
				}
				i.appSegment->push_back(ma);
				if (*ptrByte == L'\0')
				{
					ptrByte += 2;
					break;								//小节读取结束
				}
			}
			this->ptrTable->insert(i);					//读完小节中的内容，将小节(memUnit)写入总表中
		}
	}
	inline BOOL GetPrivateProfileStringW(std::vector<memApp*>* lpAppSegment, LPCTSTR lpKeyName, DWORD nSize, LPCTSTR lpString)
	{
		for (auto i : *lpAppSegment)
		{
			if (i->key[0])
			{
				if (wcsstr(i->key, lpKeyName))
				{
					i->key[0] = L'\0';
					if (i->value)
					{
						int a = i->value->size() < nSize ? i->value->size() : nSize;
						a *= sizeof(WCHAR);
						memcpy_s((void*)lpString, a, &i->value->at(0), a);
						return 1;
					}
					else
						return 0;
				}
			}
		}
		return 0;
	}
	inline BOOL GetPrivateProfileStringW(std::vector<memApp*>* lpAppSegment, LPCTSTR lpKeyName, std::vector<WCHAR>& wc)
	{
		for (auto i : *lpAppSegment)
		{
			if (i->key[0])
			{
				if (wcsstr(i->key, lpKeyName))
				{
					i->key[0] = L'\0';
					if (i->value)
					{
						if (i->value->size() > wc.size())
						{
							wc.resize(i->value->size());
						}
						int a = wc.size();
						a *= sizeof(WCHAR);
						memcpy_s((void*)&wc[0], a, &i->value->at(0), a);
						return 1;
					}
					else
						return 0;
				}
			}
		}
		return 0;
	}
	inline BOOL WritePrivateProfileStringW(std::vector<memApp*>* lpAppSegment, LPCTSTR lpKeyName, LPCTSTR lpString)
	{
		memApp* i = new memApp();
		wcscpy_s(i->key, lpKeyName);
		UINT len = (wcslen(lpString) + 1);
		i->value = new std::vector<WCHAR>(len);
		len *= sizeof(WCHAR);
		memcpy_s(&i->value->at(0), len, (void*)lpString, len);
		lpAppSegment->push_back(i);
		return 0;
	}
	inline void PushWcharVector(std::vector<WCHAR>* bc,const WCHAR* wc, UINT len) {
		for (UINT i = 0; i < len; i++)
		{
			bc->push_back(wc[i]);
		}
	}

	//出入口相关函数
	inline Egress::~Egress()
	{
		if (ptrUpLevel)
		{
			auto i = find(ptrUpLevel->begin(), ptrUpLevel->end(), this);
			if (*i == this)
			{
				auto k = ptrUpLevel;
				ptrUpLevel = NULL;
				k->erase(i);
			}
		}
		if (ptrInge)
		{
			auto k = find(ptrInge->pointedByEgress.begin(), ptrInge->pointedByEgress.end(), this);
			if (*k == this)
				ptrInge->pointedByEgress.erase(k);
		}
	}
	template<typename cast>
	inline UINT Egress::getTarget(cast*& varReturn)
	{
		if (ptrDirect == NULL)
		{
			if (ptrUpLevel->ptrManager.isEmpty())
				if (ptrUpLevel->findGlobalManager() == 0)
					return MEM_NOTFOUND_FILE;
			memPtr<Ingress> inge = ptrUpLevel->ptrManager->findIngress(keywords, type);
			if (inge.isEmpty())
				return MEM_NOTFOUND_KEYWORDS;
			else
			{
				ptrInge = *inge;
				ptrInge->pointedByEgress.push_back(this);
				ptrDirect = (void*)*ptrInge->mu;
			}
		}
		varReturn = (cast*)ptrDirect;
		if (varReturn)
			return MEM_SUCCESS;
		else
			return MEM_EMPTY_INGRESS;
	}
	


	//Trial code(demo)
#ifdef MEM_DEBUG
	inline int testFoo(int a, int b) { return a + b; }
	inline int testFoo2(int a, int b) { return a * b; }
	inline int testFoo3(int a, int b) { return a - b; }
	INITIALIZE_PFUNCTION(int(int, int), 75342, testFoo, testFoo2, testFoo3);
	class testManager;
	class testUnit2;
	class testUnit :public memUnit {
		friend class memUnit;
		void save_fetch(memPara para) override
		{
			GWPP(L"anotherone", anotherone, para);
			GWPP(L"num", num, para);
			GWPP(L"wc", wc, para);
		}
	public:
		testUnit(memManager* manager) :memUnit(manager) {};
		memPtr<testUnit> anotherone;
		double num;
		memWcs wc;
	};
	class testManager :public memManager {
	public:
		memPtr<testUnit> anothert1;
		memPtr<testUnit> anothert2;
		memPtr<testUnit2> tu2;
		pmemVector<testUnit> vec;
		UINT num;
		CHAR n1;
		BYTE n2;
		SHORT n3;
		USHORT n4;
		INT n5;
		UINT n6;
		INT64 n7;
		UINT64 n8;
		float n9;
		double n10;
		memWcs wc;
		enum { ea, eb, ec }n11;
	private:
		void save_fetch(memPara para) override{
			GWPP(L"anothert1", anothert1, para);
			GWPP(L"anothert2", anothert2, para);
			GWPP(L"tu2", tu2, para);
			GWPP(L"vec", vec, para);
			GWPP(L"num", num, para);
			GWPP(L"n1", n1, para);
			GWPP(L"n2", n2, para);
			GWPP(L"n3", n3, para);
			GWPP(L"n4", n4, para);
			GWPP(L"n5", n5, para);
			GWPP(L"n6", n6, para);
			GWPP(L"n7", n7, para);
			GWPP(L"n8", n8, para);
			GWPP(L"n9", n9, para);
			GWPP(L"n10", n10, para);
			GWPP(L"wc", wc, para);
			GWPP(L"n11", n11, para);
		}
	};
	class testUnit2 :public memUnit {
		friend class memUnit;
		void save_fetch(memPara para) override {
			GWPP(L"egressTest", egressTest, para);
			GWPP(L"genetest", genetest, para);
			GWPP(L"funcTest", funcTest, para);
		}
	public:
		pEgress<testUnit> egressTest;
		pVariant<testUnit, testUnit2> genetest;
		pFunction<int(int, int),75342> funcTest;
		testUnit2(memManager* manager) :memUnit(manager) {};
	};
	inline void testmain()
	{
		WCHAR url[100] = L"C:\\Users\\R\\Desktop\\newsave.guma";	//如果在其他计算机执行，需要改为另一个有效的路径
		WCHAR url2[100] = L"C:\\Users\\R\\Desktop\\oldsave.guma";
		sizeof(memPtr<testUnit>);

		//内存泄漏测试项
		while(1){

		memPtr<testManager> testManagerA = new testManager();
		memPtr<testUnit> testUnitA = new testUnit(*testManagerA);
		memPtr<testUnit> testUnitB = new testUnit(*testManagerA);
		testUnitA->anotherone = testUnitB;
		testUnitA->num = -1000;
		testUnitA->wc = L"t1";
		testUnitB->anotherone = testUnitB;
		testUnitB->num = 2000;
		testUnitB->wc = L"t2";
		testManagerA->anothert1 = testUnitA;
		memPtr<testUnit> copyTest = memUnit::memCopy(testManagerA->anothert1);
		//testManagerA->anothert2 = memUnit::memCopy(testUnitA);
		testManagerA->anothert2 = testUnitB;
		testManagerA->num = 4000;
		testManagerA->setUrl(url);

		testManagerA->wc = L"t4";
		testManagerA->vec = new memVector<testUnit>(*testManagerA);
		testManagerA->vec->emplace_back(testUnitA);
		testManagerA->vec->emplace_back(*testUnitB);
		testManagerA->vec->emplace_back(*testUnitA);
		testManagerA->vec->emplace_back(new testUnit(*testManagerA));
		testManagerA->vec->emplace_back(new testUnit(*testManagerA));
		memPtr<testUnit> t5 = memUnit::memCopy(testUnitB);
		testManagerA->tu2 = new testUnit2(*testManagerA);
		memPtr<testUnit2> testUnitC = testManagerA->tu2;
		
		//泛型指针测试
		testManagerA->tu2->genetest = testUnitA;
		testUnit* geneGet1;
		testUnit2* geneGet3;
		testManagerA->tu2->genetest.getPtr(geneGet1);
		testManagerA->tu2->genetest.getPtr(geneGet3);
		testManagerA->tu2->genetest = NULL;
		testManagerA->tu2->genetest = new testUnit2(*testManagerA);
		//泛型指针速度测试
		//MSVC/C++无法对变参模板展开函数推导constexpr，故读写速度都为o(n). 理论应是o(1)
		/*for (int i = 0; i < 10000000; i++)
		{
			testManagerA->tu2->genetest.getPtr(geneGet3);
		}
		for (int i = 0; i < 10000000; i++)
		{
			testManagerA->tu2->genetest.getPtr(geneGet1);
		}*/

		//最大最小值测试（存档文件数值是不准确的，以内存存取结果为准）
		testManagerA->n1 = INT8_MAX;
		testManagerA->n2 = UINT8_MAX;
		testManagerA->n3 = INT16_MAX;
		testManagerA->n4 = UINT16_MAX;
		testManagerA->n5 = INT32_MAX;
		testManagerA->n6 = UINT32_MAX;
		testManagerA->n7 = INT64_MAX;
		testManagerA->n8 = UINT64_MAX;
		testManagerA->n9 = 3.1415926535897932f;
		testManagerA->n10 = 3.1415926535897932;
		testManagerA->n11 = testManager::ec;

		testManagerA->n1 = INT8_MIN;
		testManagerA->n3 = INT16_MIN;
		testManagerA->n5 = INT32_MIN;
		testManagerA->n7 = INT64_MIN;

		//读写速度测试项
		/*for (int i = 0; i < 25000; i++)
		{
			testManagerA->vec->push_back(new testUnit(*testManagerA));
		}
		for (auto i : **(testManagerA->vec))
		{
			i->num = 10000000000000;
			i->num *= 10000000000000;
			i->anotherone = new testUnit(*testManagerA);
			i->anotherone->anotherone = new testUnit(*testManagerA);
			i->wc = L"Hello World. 太慢了，太慢了！十万元素要花3秒！";
		}*/

		//pFunction测试
		testUnitC->funcTest = testFoo2;
		testUnitC->funcTest = testFoo3;

		int ret = testManagerA->download();

		memPtr<testManager> testManagerB = new testManager();
		testManagerB->setUrl(url);
		testManagerB->upload();


		int fooTestRet = testManagerB->tu2->funcTest(10, 5);

		//ingress/egress测试项
		testManagerB->setUrl(url2);
		testManagerA->tu2->egressTest.makeEIPair(*testManagerA, testManagerB->anothert1, L"ingressTest");

		//指针测试项

		//ingress/egress速度测试项
		/*for (int i = 0; i < 100000; i++)
		{
			testManagerA->tu2->egressTest.makeEIPair(*testManagerA, testManagerB->anothert1, L"ingressTest");
		}*/

		//egress vector
		pmemVectorEgress<testUnit> egressvector = new memVectorEgress<testUnit>(*testManagerB);
		egressvector->emplace_back(*testManagerA, testManagerB->anothert1, L"ingressTest");

		testManagerB->download();
		testManagerA->download();

		testManagerA.release();
		testManagerA.release();
		testManagerB.release();

		testManagerB = new testManager();			//调换了testManager
		testManagerB->setUrl(url);
		testManagerB->upload();
		testManagerA = new testManager();
		testManagerA->setUrl(url2);
		testManagerA->upload();
		testUnit* ingressSuccess;
		//testManagerA->anothert1.release();

		//getTarget在全局查找目标memManager。需要事先将memManager upload到内存，才能get
		ret = testManagerB->tu2->egressTest.getTarget(ingressSuccess);



		//SFINAE测试项
		extract_template_memUnit<int>::type* sfinae1 = NULL;
		bool sfinae_isTemplate = is_template<pEgress<testUnit>>::value;
		extract_template_memUnit<memPtr<Egress>>::type* sfinae2 = NULL;
		bool sfinae3 = is_based_on_memPtr<pEgress<testUnit>>::value;
		void* sfinaeVT = memUnit::get_vtable_ptr<Egress>::ptr();



		//反射测试项
		//读
		ReflectResult refRes;
		memUnit* refTestMu = *testManagerA;
		refTestMu->reflectionRead(&refRes);

		ReflectResult refRes2;
		memUnit* refTestMu2 = *testManagerA->tu2;
		refTestMu2->reflectionRead(&refRes2);

		//写
		ULONGLONG refNum = 123456;
		ReflectResultKeyValue refKW = ReflectResultKeyValue(L"num",(UINT&)refNum);
		refTestMu->reflectionWrite(refKW);
		refNum = 654321;
		refKW = ReflectResultKeyValue(L"n8", refNum);
		refTestMu->reflectionWrite(refKW);
		WCHAR refWC[] = L"wchar test";
		refTestMu->reflectionWrite(ReflectResultKeyValue(L"wc", refWC));
		refTestMu->reflectionWrite(ReflectResultKeyValue(L"wc", L"wchar test directly"));
		refKW = ReflectResultKeyValue(L"anothert1", testManagerA->anothert2);	//合法内存单元指针写入
		bool refSuccess = refTestMu->reflectionWrite(refKW);
		refKW = ReflectResultKeyValue(L"anothert1", testManagerA->vec);			//非法内存单元指针写入
		refSuccess = refTestMu->reflectionWrite(refKW);

		memUnit* refTestVariant = *testManagerA->anothert1;
		refKW = ReflectResultKeyValue(L"genetest", refTestVariant);
		refTestMu2->reflectionWrite(refKW);



		//序列化测试项(memUnit)
		std::vector<WCHAR> serializeDump = {};
		testManagerA->anothert1->num = 2.72;
		testManagerA->anothert1->serialize(&serializeDump);

		//反序列化测试项(memUnit)
		testManagerB = new testManager();
		testManagerB->anothert1 = new testUnit(*testManagerB);
		testManagerB->anothert1->deserialize(&serializeDump[0], serializeDump.size());
		serializeDump.clear();
		testManagerB.release();

		//序列化测试项(memManager)
		if (true)
		{
			testManagerA->serialize(&serializeDump);
			testManagerA.release();

			//反序列化测试项(memManager)
			testManagerB = new testManager();
			testManagerB->deserialize(&serializeDump[0], serializeDump.size());
		}

		//内存泄漏测试项2
		}
		return;
	}
#endif // DEBUG
}
#pragma warning(default: MEM_MANAGER_DISABLE_WARINIGS)

//1 不提供弱引用指针。
//	不要过于相信智能指针的自动回收功能。当智能指针可能出现循环引用时，应该直接release掉这些智能指针。
//	多线程时考虑禁用release，使用精确的架构设计来保证不泄漏。
//	所有memUnit在memManager内部都有记录，内存泄漏不严重，不是很怕。
// 
//2 memUnit中成员memPtr的指向，不准跨越这个memUnit的memManager。
//  memManager之间的通信使用egress/ingress
//	pEgress相当于指针，直接指向不同memManager的memUnit。
//	Subfile和Ingress都是内部结构体，无需操作。
//	egress/ingress只支持单线程，多线程自行添加锁
//	从egress处取得的裸指针，不能长期持有，更不能赋给memPtr
// 
//3 在析构memManager前，需要确保没有线程正在使用这个memManager内的东西。
// 
//4 在memUnit派生类的析构函数中使用getManager时，需要检查返回的memManager是否为空。
//  若为空，说明此memManager正在整体析构。
//
//5 若继承memVector，key禁止取为数字名和"size"
//
//6 千万别手贱给pVariant的变参模板、pFunction的初始化表调换顺序，或者在中间删掉一个，不然就读不出存档了！！
//
//7 若有一个新类继承memPtr，则使用vector时，必须重写一份基于此新类的vector
//
//8 存档时，memVector遇到内部的空指针，将自动省略。故不能利用索引对齐两个memVector，保存memVector的索引通常也是没有价值的。
// 
// 
// 
//TODO：进行优化。序列化时，拆包小节不是统一拆，而要交由save_fetch去做，这样能在读取时知道变量类型，就可直接使用进行字节对齐，而不用将所有类型的变量转换成WCHAR。
//		进行安全性改进，使用更好的宽字符处理程序，避免异常
//