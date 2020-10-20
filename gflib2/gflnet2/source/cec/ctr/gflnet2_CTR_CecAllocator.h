// ============================================================================
/*
 * @file gflnet2_CTR_CecAllocator.h
 * @brief すれ違い通信用のアロケータークラスです。
 * @date 2015.02.16
 */
// ============================================================================
#ifdef  GF_PLATFORM_CTR
#if !defined( GFLNET2_CTR_CECALLOCATOR_H_INCLUDE )
#define GFLNET2_CTR_CECALLOCATOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <nn/fnd.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(ctr)


class CTR_CecAllocator : public nn::fnd::IAllocator
{
public:
  CTR_CecAllocator( gfl2::heap::HeapBase* pHeap );
  virtual~CTR_CecAllocator();

  virtual void* Allocate( size_t size, s32 alignment = 4 );
  virtual void Free( void* p );

private:
  gfl2::heap::HeapBase*     m_pHeap;

};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif // GFLNET2_CTR_CECALLOCATOR_H_INCLUDE
#endif