#if PM_DEBUG

//======================================================================
/**
 * @file FieldDebugRotomPower.h
 * @date 2017/01/30
 * @author munakata_kai
 * @brief デバッグ：ロトムパワー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELDDEBUG_ROTOM_POWER_H_INCLUDED__
#define __FIELDDEBUG_ROTOM_POWER_H_INCLUDED__
#pragma once

#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

// 前方宣言
namespace gfl2 {
  namespace debug {
    class DebugWinGroup;
  }
}

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
 * @class DebugRotomPower
 * @brief ロトムパワーデバッグ関連
 */
class DebugRotomPower
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugRotomPower);

public:
  /**
   *  @brief  コンストラクタ
   */
  DebugRotomPower(void);

  /**
   *  @brief  デストラクタ
   */
  ~DebugRotomPower(void);
  
  /**
   *  @brief  初期化
   */
  void Initialize(
    gfl2::heap::HeapBase *pHeap,
    gfl2::str::MsgData* pMsgData
    );

  /**
   *  @brief  破棄
   */
  void Terminate( void );

  /**
   *  @brief  メニューの作成
   */
  void CreateDebugMenu( gfl2::debug::DebugWinGroup *root );

public:

  gfl2::heap::HeapBase*             m_pHeap;
  gfl2::str::MsgData*               m_pMsgData;
  s32                               m_rotomPowerID;
  gfl2::str::StrBuf*                m_pRotomPowerIDSTR;
  bool                              m_isChangeRotomPowerID;
  s32                               m_nowRotomPowerID;
  gfl2::str::StrBuf*                m_pNowRotomPowerIDSTR;
  s32                               m_rotomPowerTime;

public:

  static gfl2::ui::Button* GetButton();
  static s32 UpdateKey( gfl2::ui::Button* pButton, s32 input, s32 value, s32 valueHigh, s32 min, s32 max);
  static void UpdateFunc_RotomPower(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* DrawFunc_RotomPower(void* userWork, gfl2::debug::DebugWinItem* item);

  static wchar_t* DrawFunc_NowRotomPower(void* userWork, gfl2::debug::DebugWinItem* item);

  static void UpdateFunc_RotomPowerTime(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* DrawFunc_RotomPowerTime(void* userWork, gfl2::debug::DebugWinItem* item);

  static void CreateDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item );
  static void DestroyDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item );
};

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // __FIELDDEBUG_ROTOM_POWER_H_INCLUDED__

#endif // PM_DEBUG
