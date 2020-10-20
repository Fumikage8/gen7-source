//=============================================================================
/**
 * @file    AppToolRotomFriendship.cpp
 * @brief   ロトム親密度関連
 * @author  Hiroyuki Nakamura
 * @date    2016.11.25
 */
//=============================================================================

// module
#include "AppLib/include/Tool/AppToolRotomFriendship.h"
// momiji
#include "GameSys/include/GameData.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

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
void RotomFriendship::AddCharacterCounter( Savedata::FieldMenu::RotomCharacter character, u32 value )
{
  GameSys::GameData * gd = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  Savedata::FieldMenu * field_menu = gd->GetFieldMenu();
  const Field::EventWork * event_work = gd->GetEventWork();

  if( field_menu->GetRotomRank(GetDataIndex(event_work)) != Savedata::FieldMenu::ROTOM_RANK_4 )
  {
    return;
  }

  field_menu->AddRotomCharacterCounter( character, value );
}

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
u32 RotomFriendship::GetDataIndex( const Field::EventWork * event_work )
{
  static const u16 tbl[] =
  {
    SYS_FLAG_ROTOM_SINMITU_3, // ロトム親密度の上限解放フラグ3_島４に到達した
    SYS_FLAG_ROTOM_SINMITU_2, // ロトム親密度の上限解放フラグ2_島３に到達した
    SYS_FLAG_ROTOM_SINMITU_1, // ロトム親密度の上限解放フラグ1_島２に到達した
  };

  u32 max = GFL_NELEMS(tbl);

  for( u32 i=0; i<max; i++ )
  {
    if( event_work->CheckEventFlag(tbl[i]) != false )
    {
      return ( max - i );
    }
  }
  return 0;
}




#if PM_DEBUG
u32 RotomFriendship::DebugGetFriendshipMax(void)
{
  GameSys::GameData * gd = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  return gd->GetFieldMenu()->DebugGetRotomFriendshipMax( GetDataIndex(gd->GetEventWork()) );
}
#endif  // PM_DEBUG


GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)
