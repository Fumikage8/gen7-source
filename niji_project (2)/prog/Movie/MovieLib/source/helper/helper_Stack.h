#if defined(GF_PLATFORM_CTR)

#ifndef __HELPER_STACK_H__
#define __HELPER_STACK_H__

#pragma once

//#include <gflib.h>
#include "heap/include/gfl2_Heap.h"
namespace mw {
namespace mo {
namespace helper {

class Stack
{
public:
	Stack(gfl::heap::HeapBase* heap, u32 size)
	{
		m_stack = GflHeapAllocMemoryLow(heap, size);
	}
	virtual ~Stack()
	{
		Finalize();
	}

	void Finalize()
	{
		GflHeapFreeMemory(m_stack);
	}

	inline uptr GetStackBottom( void )
    {
		return reinterpret_cast<uptr>(m_stack) + m_size;
    }
private:
	void* m_stack;
	u32 m_size;
};

}}}

#endif //__HELPER_STACK_H__

#endif //defined(GF_PLATFORM_CTR)
