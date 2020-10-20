#if PM_DEBUG
//======================================================================
/**
 * @file DebugMovie.cpp
 * @date 2016/02/26 11:55:57
 * @author nagihashi_toru
 * @brief ムービーのデバッグ機能
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "System/include/HeapDefine.h"
#include <heap/include/gfl2_Heap.h>
#include "DebugMovie.h"
#include "DebugMovieImpl.h"

SINGLETON_INSTANCE_DEFINITION(movie::DebugMovie)
template class gfl2::base::SingletonAccessor<movie::DebugMovie>;

GFL_NAMESPACE_BEGIN(movie)

/**
  *  @brief  ctor
  */
DebugMovie::DebugMovie()
: m_impl(NULL)
{
  gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
  m_impl = GFL_NEW(heap) DebugMovieImpl();
}
/**
  *  @brief  デストラクタ
  */
DebugMovie::~DebugMovie()
{
  GFL_DELETE( m_impl );
}

/**
  *  @brief  初期化
  */
void DebugMovie::Initialize()
{
  m_impl->Initialize();
}

/**
  *  @brief  破棄
  */
void DebugMovie::Finalize()
{
  m_impl->Finalize();
}

/**
  *  @brief  スキップ設定かどうか
  *  @return true で強制スキップ false で通常状態
  */
bool DebugMovie::IsSkip()
{
  return m_impl->IsSkip();
}

/**
  *  @brief  ループ設定かどうか
  *  @return true で強制ループ状態 false で通常状態
  */
bool DebugMovie::IsLoop()
{
  return m_impl->IsLoop();
}

/**
  *  @brief  シングルトン生成
  */
void DebugMovie::InitializeSingleton()
{
  gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );

  DebugMovie* instance = GFL_NEW(heap) DebugMovie();
  instance->Initialize();
}

/**
  *  @brief  シングルトン破棄
  */
void DebugMovie::FinalizeSingleton()
{
  DebugMovie* instance = GFL_SINGLETON_INSTANCE(DebugMovie);
  instance->Finalize();
  GFL_DELETE( instance );
}

GFL_NAMESPACE_END(movie)
#endif //PM_DEBUG
