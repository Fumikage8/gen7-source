#if defined(GF_PLATFORM_CTR)
/*-----------------------------------------------------------------------------------------------------------*
 File:        helper_Allocator.cpp
 Description: Helper file that manages allocation with additional debugging
              stats

 Copyright (C)2011 Mobiclip, Inc.
 *-----------------------------------------------------------------------------------------------------------*/

#include "helper_Allocator.h"

/*-----------------------------------------------------------------------------------------------------------*/

using namespace mw::mo::helper;

/*-----------------------------------------------------------------------------------------------------------*

 *-----------------------------------------------------------------------------------------------------------*/

void* DebugExpHeap::Allocate(size_t byteSize, s32 alignment, bit8 groupId, AllocationMode mode, bool reuse)
{
    void* ptr = nn::fnd::ExpHeap::Allocate(byteSize, alignment, groupId, mode, reuse);
	NN_ASSERT(ptr != NULL);
    if (ptr == NULL)
        return NULL;
    
    m_AllocSizeCurrent += nn::fnd::ExpHeap::GetSizeOf(ptr);
    if (m_AllocSizeCurrent > m_AllocSizeMax)
        m_AllocSizeMax = m_AllocSizeCurrent;

    m_AllocCountTotal++;
    m_AllocCountCurrent++;
    if (m_AllocCountCurrent > m_AllocCountMax)
        m_AllocCountMax = m_AllocCountCurrent;

    return ptr;
}

/*-----------------------------------------------------------------------------------------------------------*

 *-----------------------------------------------------------------------------------------------------------*/

void DebugExpHeap::Free(void* ptr)
{
    if (ptr == NULL || nn::fnd::ExpHeap::HasAddress(ptr) == false)
        return;

    m_AllocSizeCurrent -= nn::fnd::ExpHeap::GetSizeOf(ptr);
    m_AllocCountCurrent--;
    
    nn::fnd::ExpHeap::Free(ptr);
}

/*-----------------------------------------------------------------------------------------------------------*

 *-----------------------------------------------------------------------------------------------------------*/

void* DebugExpHeap::DebugAllocator::Allocate(size_t size, s32 alignment)
{
    return m_pHeap->Allocate(size, alignment);
}

/*-----------------------------------------------------------------------------------------------------------*

 *-----------------------------------------------------------------------------------------------------------*/

void DebugExpHeap::DebugAllocator::Free(void* p)
{
    m_pHeap->Free(p);
}

/*-----------------------------------------------------------------------------------------------------------*

 *-----------------------------------------------------------------------------------------------------------*/
#endif// defined(GF_PLATFORM_CTR)
