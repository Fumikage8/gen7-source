// ============================================================================
/*
 * @file gflnet2_CTR_CecAllocator.cpp
 * @brief すれ違い通信用のアロケータークラスです。
 * @date 2015.02.16
 */
// ============================================================================
#ifdef  GF_PLATFORM_CTR
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecAllocator.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(ctr)


CTR_CecAllocator::CTR_CecAllocator( gfl2::heap::HeapBase* pHeap ) :
  m_pHeap( pHeap )
{
}


CTR_CecAllocator::~CTR_CecAllocator()
{
}


void* CTR_CecAllocator::Allocate( size_t size, s32 alignment )
{
  return GflHeapAllocMemoryAlign( m_pHeap, size, alignment );
}


void CTR_CecAllocator::Free( void* p )
{
  GflHeapFreeMemory(p);
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif