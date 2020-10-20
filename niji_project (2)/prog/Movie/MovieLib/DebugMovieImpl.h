#if PM_DEBUG
//======================================================================
/**
 * @file DebugMovieImpl.h
 * @date 2016/02/26 12:09:27
 * @author nagihashi_toru
 * @brief DebugMovie実装部分
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __DEBUG_MOVIE_IMPL_HEADER_H_INCLUDED__
#define __DEBUG_MOVIE_IMPL_HEADER_H_INCLUDED__
#pragma once

#include "System/include/HeapDefine.h"
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_DebugWinSystem.h>
#include <debug/include/gfl2_DebugWinItemTypes.h>

GFL_NAMESPACE_BEGIN(movie)

/**
 * @class DebugMovieImpl
 * @brief DebugMovie実装部分
 * @note DebugMovie.cpp でのみ Include されるので h のみで記述。
 */
class DebugMovieImpl
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugMovieImpl);

public:
  /**
    *  @brief ctor
    */
  DebugMovieImpl() : m_isSkip(false), m_isLoop(false), m_group(NULL) {}
  /**
    *  @brief デストラクタ
    */
  virtual ~DebugMovieImpl() {}
  /**
    *  @brief  初期化
    */
  void Initialize()
  {
    gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    AddDebugWin(heap);
  }
  /**
    *  @brief  破棄
    */
  void Finalize()
  {
    RemoveDebugWin();
  }
  /**
    *  @brief  スキップ設定かどうか
    *  @return true で強制スキップ false で通常状態
    */
  bool IsSkip() { return m_isSkip; }
  /**
    *  @brief  ループ設定かどうか
    *  @return true で強制ループ状態 false で通常状態
    */
  bool IsLoop() { return m_isLoop; }

private:
  /**
    *  @brief デバッグメニューにMovie用の項目追加
    *  @param heap 生成用ヒープ
    */
  void AddDebugWin( gfl2::heap::HeapBase *heap )
  {
    m_group     = gfl2::debug::DebugWin_AddGroup(heap, L"Movie", gfl2::debug::DebugWin_GetTopGroup() );
    gfl2::debug::DebugWin_AddItemValueBool(m_group, heap, L"Skip", &m_isSkip);
    gfl2::debug::DebugWin_AddItemValueBool(m_group, heap, L"Loop", &m_isLoop);
  }
  /**
    *  @brief デバッグメニューから Movie用の項目除去
    */
  void RemoveDebugWin()
  {
    gfl2::debug::DebugWin_RemoveGroup(m_group);
  }

  bool m_isSkip;
  bool m_isLoop;
  gfl2::debug::DebugWinGroup* m_group;
};

GFL_NAMESPACE_END(movie)

#endif // __DEBUG_MOVIE_IMPL_HEADER_H_INCLUDED__
#endif // PM_DEBUG
