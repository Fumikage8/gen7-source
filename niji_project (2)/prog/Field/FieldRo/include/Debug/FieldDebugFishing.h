//======================================================================
/**
 * @file FieldDebugFishing.h
 * @date 2016/01/21
 * @author Masanori Kanamaru
 * @brief 釣りデバッグ
 * @par Copyright
 */
//======================================================================

#if !defined __FIELD_DEBUG_FISHING_H_INCLUDED__
#define __FIELD_DEBUG_FISHING_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#if PM_DEBUG

// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)

GFL_NAMESPACE_BEGIN(str)
class MsgData;
GFL_NAMESPACE_END(str)

GFL_NAMESPACE_BEGIN(debug)
class DebugWinGroup;
GFL_NAMESPACE_END(debug)

  GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(Field)  
GFL_NAMESPACE_BEGIN(Debug)

class DebugFishing
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugFishing);
public:

  DebugFishing(){};

  /**
   *  @brief  初期化
   */
  void Initialize( gfl2::heap::HeapBase *pHeap, gfl2::str::MsgData* pMsgData );

  /**
   *  @brief  メニューの作成
   */
  void CreateDebugMenu( gfl2::debug::DebugWinGroup *root );

  gfl2::heap::HeapBase *GetHeap( void ){ return this->pHeap; }

private:
  gfl2::heap::HeapBase *pHeap;
};


GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif //PM_DEBUG

#endif // __FIELD_DEBUG_FISHING_H_INCLUDED__