#if defined(GF_PLATFORM_CTR)
#if !defined( __HELPER_ALLOCATOR_H__ )
#define __HELPER_ALLOCATOR_H__
/*---------------------------------------------------------------------------*
 File:        helper_Allocator.h
 Description: Helper file that manages allocation with additional debugging
              stats

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#ifndef helper_Allocator_h
#define helper_Allocator_h

/*---------------------------------------------------------------------------*/

#include <nn/types.h>
#include <nn/fnd.h>

/*---------------------------------------------------------------------------*
  C++ API interface
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*
  ExpHeap helper for improved stats on memory allocation usage
 *---------------------------------------------------------------------------*/

class DebugExpHeap : public nn::fnd::ExpHeap
{
public:

    int       m_AllocCountTotal;
    int       m_AllocCountMax;
    int       m_AllocCountCurrent;
    size_t    m_AllocSizeMax;
    size_t    m_AllocSizeCurrent;

public:
    DebugExpHeap() : m_AllocCountTotal(0), m_AllocCountMax(0), m_AllocCountCurrent(0), 
                     m_AllocSizeMax(0), m_AllocSizeCurrent(0){}


    void* Allocate (size_t byteSize, s32 alignment = DEFAULT_ALIGNMENT, bit8 groupId = 0, 
                    AllocationMode mode = ALLOCATION_MODE_FIRST_FIT, bool reuse = false);
    void  Free     (void* ptr);

public:

    class DebugAllocator;
};

/*---------------------------------------------------------------------------*
  IAllocator helper for improved stats on memory allocation usage
 *---------------------------------------------------------------------------*/

class DebugExpHeap::DebugAllocator : public nn::fnd::IAllocator
{
private:

    DebugExpHeap* m_pHeap;

public:

    DebugAllocator(DebugExpHeap& heap){m_pHeap = &heap;}

    virtual void* Allocate (size_t size, s32 alignment);
    virtual void  Free     (void* p);
};

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/

#endif // ifdef __cplusplus

/*---------------------------------------------------------------------------*/

#endif // ifndef helper_Allocator_h

/*---------------------------------------------------------------------------*/
#endif // __HELPER_ALLOCATOR_H__
#endif// defined(GF_PLATFORM_CTR)
