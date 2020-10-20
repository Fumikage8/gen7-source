//======================================================================
/**
* @file FieldDebugMantainRecord.h
* @date 2015/10/26 16:45:33
* @author saito_nozomu
* @brief マンタインサーフレコード　デバッグ
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if !defined __FIELD_DEBUG_MANTAIN_RECORD_H_INCLUDED__
#define __FIELD_DEBUG_MANTAIN_RECORD_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#if PM_DEBUG

// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(debug)
class DebugWinGroup;
GFL_NAMESPACE_END(debug)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

class DebugMantain
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugMantain);
public:
  /**
  *  @brief  コンストラクタ
  */
  DebugMantain(void){}

  /**
  *  @brief  デストラクタ
  */
  ~DebugMantain(void){}

  /**
  *  @brief  初期化
  */
  void Initialize(gfl2::heap::HeapBase *pHeap, gfl2::str::MsgData* pMsgData);

  /**
  *  @brief  メニューの作成
  */
  void CreateDebugMenu(gfl2::debug::DebugWinGroup *root);

  gfl2::heap::HeapBase *GetHeap(void){ return m_pHeap; }
  inline  gfl2::str::MsgData* GetMessageData(void) { return m_pMsgData; }

  void Execute(u32 id);

private:
  gfl2::heap::HeapBase          *m_pHeap;
  gfl2::str::MsgData*           m_pMsgData;
};


GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif //PM_DEBUG

#endif // __FIELD_DEBUG_MANTAIN_RECORD_H_INCLUDED__