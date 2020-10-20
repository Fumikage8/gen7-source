//=============================================================================
/**
 * @file    AppToolRotomFriendship.h
 * @brief   ロトム親密度関連
 * @author  Hiroyuki Nakamura
 * @date    2016.11.25
 */
//=============================================================================
#if !defined( APP_TOOL_ROTOM_FRIENDSHIP_H_INCUDED )
#define APP_TOOL_ROTOM_FRIENDSHIP_H_INCUDED

#pragma once

// gflib2
#include <macro/include/gfl2_Macros.h>
// momiji
#include "Savedata/include/FieldMenuSave.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(Field)
class EventWork;
GFL_NAMESPACE_END(Field)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

//=============================================================================
/**
 * @class RotomFriendship
 * @brief ロトム親密度操作
 * @date  2016.11.25
 */
//=============================================================================
class RotomFriendship
{
  GFL_FORBID_COPY_AND_ASSIGN( RotomFriendship );

public:
  //-----------------------------------------------------------------------------
  /**
   * @func    AddCharacterCounter
   * @brief   性格変更用のカウンタをアップ
   * @date    2016.11.30
   *
   * @param   character   性格
   * @param   value       アップ値
   */
  //-----------------------------------------------------------------------------
  static void AddCharacterCounter( Savedata::FieldMenu::RotomCharacter character, u32 value );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetDataIndex
   * @brief   親密度データのインデックスを取得
   * @date    2016.11.25
   *
   * @param   event_work  イベントワーク
   *
   * @return  データインデックス
   */
  //-----------------------------------------------------------------------------
  static u32 GetDataIndex( const Field::EventWork * event_work );


#if PM_DEBUG
public:
  static u32 DebugGetFriendshipMax(void);
#endif  // PM_DEBUG

};

GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

#endif  // APP_TOOL_ROTOM_FRIENDSHIP_H_INCUDED
