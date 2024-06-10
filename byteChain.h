#pragma once

#include "windows.h"
//字节链表，这是本人初学C++时期的作品（包括memManager类也是）

typedef UINT BC_OP;// byte chain offset pointer
struct BYTE_CHAIN_SEGMENT {
	BYTE* data;
	BYTE_CHAIN_SEGMENT* next;
};
struct BYTE_CHAIN_MARK {
	unsigned int volume;
	BYTE_CHAIN_SEGMENT* pchain;
	BYTE* pbyte;
	BYTE_CHAIN_MARK* next;
};
class BYTE_CHAIN {
public:
	//构造函数，定义每个segment的长度
	inline BYTE_CHAIN(unsigned int Alignment)
	{
		this->Alignment = Alignment;
		this->AlignmentRemain = Alignment;
		this->size = Alignment;
		this->sumAlignment = 1;
		this->first = (BYTE_CHAIN_SEGMENT*)calloc(1, sizeof(BYTE_CHAIN_SEGMENT));
		this->first->data = (BYTE*)calloc(1, Alignment);
		this->first->next = 0;
		this->now = this->first;
		this->pointer = this->first->data;
		this->marks = (BYTE_CHAIN_MARK*)calloc(1, sizeof(BYTE_CHAIN_MARK));
		this->marknow = this->marks;
	}
	inline ~BYTE_CHAIN()
	{
		BYTE_CHAIN_SEGMENT* a = first;
		while (a)
		{
			BYTE_CHAIN_SEGMENT* b = a;
			a = a->next;
			free(b->data);
			free(b);
		}
		BYTE_CHAIN_MARK* c = marks;
		while (c)
		{
			BYTE_CHAIN_MARK* b = c;
			c = c->next;
			free(b);
		}
	}

	int size;										//总大小
	int sumAlignment;								//节数量

	//向链表中压入指定长度与内容的字节。会自动增长segment。
	inline void Push(BYTE* para, unsigned int len)
	{
		while (len >= AlignmentRemain)
		{
			if (AlignmentRemain)
			{
				memcpy_s(pointer, AlignmentRemain, para, AlignmentRemain);
				len -= AlignmentRemain;
				para += AlignmentRemain;
			}

			now->next = (BYTE_CHAIN_SEGMENT*)calloc(1, sizeof(BYTE_CHAIN_SEGMENT));
			now = now->next;
			now->data = (BYTE*)calloc(1, Alignment);
			now->next = 0;
			pointer = now->data;
			AlignmentRemain = Alignment;
			size += Alignment;
			sumAlignment++;
		}
		if (len)
		{
			memcpy_s(pointer, len, para, len);
			pointer += len;
			AlignmentRemain -= len;
		}
	}

	//把比特链表导出为Bytes。
	inline void Export(BYTE* dest, unsigned int len)
	{
		BYTE_CHAIN_SEGMENT* a = first;
		for (int i = 0; i < sumAlignment; i++)
		{
			memcpy_s(dest, Alignment, a->data, Alignment);
			dest += Alignment;
			a = a->next;
		}
	}

	//标记当前位置，并返回一个mark指针。
	inline BYTE_CHAIN_MARK* Mark()
	{
		BYTE_CHAIN_MARK* a = marknow;
		marknow->next = (BYTE_CHAIN_MARK*)calloc(1, sizeof(BYTE_CHAIN_MARK));
		marknow = marknow->next;
		a->pchain = now;
		a->pbyte = pointer;
		a->volume = AlignmentRemain;
		return a;
	}

	//标记后压入。
	inline BYTE_CHAIN_MARK* MarkPush(BYTE* para, unsigned int len)
	{
		BYTE_CHAIN_MARK* a = this->Mark();
		this->Push(para, len);
		return a;
	}

	//根据mark指针，替换比特链表中的字节。此操作会自动切换segment，但不会增长整个比特链表的segment数。
	//segment数足够返回1，segment数不够返回0。
	inline int Replace(BYTE_CHAIN_MARK* mk, BYTE* para, unsigned int len)
	{
		if (mk->volume >= len)
		{
			memcpy_s(mk->pbyte, len, para, len);
		}
		else
		{
			BYTE_CHAIN_SEGMENT* a = mk->pchain;
			unsigned int remain = mk->volume;
			BYTE* p = mk->pbyte;
			while (a != now)
			{
				if (remain <= len)
				{
					memcpy_s(p, remain, para, remain);
					len -= remain;
				}
				else
				{
					memcpy_s(p, len, para, len);
					return 1;
				}
				a = a->next;
				p = a->data;
				para += remain;
				remain = Alignment;
			}
			unsigned int alilen = Alignment - AlignmentRemain;
			if (len <= alilen)
			{
				memcpy_s(p, len, para, len);
				return 1;
			}
			else
			{
				memcpy_s(p, alilen, para, alilen);
				return 0;
			}
		}
	}

	//根据Offset Pointer输出指针
	inline VOID* op2pt(BC_OP op)
	{
		BYTE_CHAIN_SEGMENT* a = first;
		while (op >= Alignment)
		{
			op -= Alignment;
			a = a->next;
			if (a == NULL)
				return 0;
		}
		return &a->data[op];
	}

	//输出当前位置的Offset Pointer
	inline BC_OP getOP()
	{
		return sumAlignment * Alignment - AlignmentRemain - 1;
	}

	//获得RSHash值（零加密，仅校验完整性）
	inline UINT Hash()
	{
		unsigned int b = 378551;
		unsigned int a = 63689;
		unsigned int hash = 0;
		BYTE_CHAIN_SEGMENT* m = first;
		BYTE* pt = first->data;

		for (int k = 0; k < sumAlignment; k++)
		{
			for (unsigned int i = 0; i < Alignment; pt++, i++)
			{
				hash = hash * a + (*pt);
				a = a * b;
			}
			m = m->next;
			if (m)
				pt = m->data;
			else
				break;
		}
		return hash;
	}
private:
	unsigned int Alignment;			//一个segment有多少字节
	BYTE_CHAIN_MARK* marks;			//第一个mark
	BYTE_CHAIN_MARK* marknow;		//当前mark
	BYTE_CHAIN_SEGMENT* first;		//第一个segment
	BYTE_CHAIN_SEGMENT* now;		//当前segment
	BYTE* pointer;					//当前segment的当前byte
	unsigned int AlignmentRemain;	//当前segment还剩多少字节
};