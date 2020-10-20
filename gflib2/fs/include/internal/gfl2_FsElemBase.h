#if !defined(GFL2_FSELEMBASE_H_INCLUDED)
#define GFL2_FSELEMBASE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsElemBase.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   ファイルシステムで使うリスト要素の基底クラス
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     ElemBase
 @brief     ファイルシステムで使うリスト要素の基底クラス
 */
//==============================================================================
class ElemBase
{
  GFL_FORBID_COPY_AND_ASSIGN(ElemBase);

public:
  ElemBase(void)
    : m_prev(0),
      m_next(0),
      m_prio(0)
  {}
  virtual ~ElemBase() {}  // 必ずリストから外してから破棄して下さい。
  virtual s32 GetElemType(void) const { return 0; }
  void SetPrev(ElemBase* elem) { m_prev = elem; }
  void SetNext(ElemBase* elem) { m_next = elem; }
  void SetPrio(s32 prio) { m_prio = prio; }
  ElemBase* GetPrev(void) const { return m_prev; }
  ElemBase* GetNext(void) const { return m_next; }
  s32 GetPrio(void) const { return m_prio; }
private:
  ElemBase* m_prev;
  ElemBase* m_next;
  s32       m_prio;  // 小さいほど優先度が高い、即ち、小さいほどリストの前に並ぶ。
};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL2_FSELEMBASE_H_INCLUDED

