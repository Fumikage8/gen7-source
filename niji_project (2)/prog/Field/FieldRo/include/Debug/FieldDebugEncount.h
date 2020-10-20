//======================================================================
/**
 * @file FieldDebugEncount.h
 * @date 2015/09/16 13:16:27
 * @author saita_kazuki
 * @brief デバッグ：Encount
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELDDEBUG_ENCOUNT_H_INCLUDED__
#define __FIELDDEBUG_ENCOUNT_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>

// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(debug)
class DebugWinGroup;
GFL_NAMESPACE_END(debug)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

#if PM_DEBUG

/**
 * @class DebugEncount
 * @brief クラスの説明
 */
class DebugEncount
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugEncount);

public:
  /**
   *  @brief  コンストラクタ
   */
  DebugEncount( void );

  /**
   *  @brief  デストラクタ
   */
  ~DebugEncount( void );
  
  /**
   *  @brief  初期化
   */
  void Initialize( gfl2::heap::HeapBase *pHeap, gfl2::str::MsgData* pMsgData );

  /**
   *  @brief  破棄
   */
  void Terminate( void );

  /**
   *  @brief  メニューの作成
   */
  void CreateDebugMenu( gfl2::debug::DebugWinGroup *root );

  //! ヒープ取得
  gfl2::heap::HeapBase *GetHeap( void ){ return m_pHeap; }

  inline gfl2::str::MsgData* GetMessageData( void ) { return m_pMsgData; }

public:

  gfl2::heap::HeapBase*   m_pHeap;
  gfl2::str::MsgData*     m_pMsgData;

};

#endif // PM_DEBUG

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // __FIELDDEBUG_ENCOUNT_H_INCLUDED__
