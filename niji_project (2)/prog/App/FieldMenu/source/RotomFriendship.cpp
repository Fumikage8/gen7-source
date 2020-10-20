//=============================================================================
/**
 * @file    RotomFriendship.cpp
 * @brief   ロトム親密度関連
 * @author  Hiroyuki Nakamura
 * @date    2016.11.25
 */
//=============================================================================

// module
#include "RotomFriendship.h"
// momiji
#include "GameSys/include/GameData.h"
#include "AppLib/include/Tool/AppToolRotomFriendship.h"
#if PM_DEBUG
#include "App/AppEvent/include/FieldMenu/FieldMenuGamedata.h"
#endif  // PM_DEBUG


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
RotomFriendship::RotomFriendship(void)
{
  GameSys::GameData * gd = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  m_saveFieldMenu = gd->GetFieldMenu();
  m_eventWork     = gd->GetEventWork();
#if PM_DEBUG
  m_debugPlayData = gd->GetFieldMenuPlayData();
#endif  // PM_DEBUG

//  GFL_PRINT( "★★★★★★★★　%d\n", sizeof(RotomFriendship) );
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
RotomFriendship::~RotomFriendship()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    GetRank
 * @brief   親密度ランクを取得
 * @date    2016.11.25
 *
 * @return  親密度ランク
 */
//-----------------------------------------------------------------------------
Savedata::FieldMenu::RotomRank RotomFriendship::GetRank(void)
{
  return m_saveFieldMenu->GetRotomRank( App::Tool::RotomFriendship::GetDataIndex(m_eventWork) );
}

//-----------------------------------------------------------------------------
/**
 * @func    Add
 * @brief   親密度をアップ
 * @date    2016.11.25
 *
 * @param   val       アップ値
 * @param   is_touch  true = 枠タッチで上昇した
 */
//-----------------------------------------------------------------------------
void RotomFriendship::Add( s32 val, bool is_touch )
{
  if( m_saveFieldMenu->AddRotomFriendship( App::Tool::RotomFriendship::GetDataIndex(m_eventWork), val, is_touch ) != false )
  {
    ChangeCharacter();
  }
}


//-----------------------------------------------------------------------------
/**
 * @func    GetCharacter
 * @brief   性格取得
 * @date    2016.11.28
 *
 * @return  性格
 */
//-----------------------------------------------------------------------------
Savedata::FieldMenu::RotomCharacter RotomFriendship::GetCharacter(void)
{
  return m_saveFieldMenu->GetRotomCharacter();
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeCharacter
 * @brief   性格変更
 * @date    2016.11.28
 */
//-----------------------------------------------------------------------------
void RotomFriendship::ChangeCharacter(void)
{
  /*
    @note:
      好戦的：    ポケモンを倒した数 x 0.1
      人懐っこい：ポケパルレ起動数 x 0.5
      楽天的：    ポケセン回復数 x 10

      なので、比率は 1 : 5 : 100

      取得したカウンタは最大値0x00ffffffなので、100倍してもオーバーしない
  */

  static const Savedata::FieldMenu::RotomCharacter chara_tbl[] =
  {
    Savedata::FieldMenu::ROTOM_CHAR_BELLIGERENT,
    Savedata::FieldMenu::ROTOM_CHAR_FRIENDLY,
    Savedata::FieldMenu::ROTOM_CHAR_OPTIMISTIC,
  };
  static const u32 mul_tbl[] = { 1, 5, 100 };

  u32 val_tbl[Savedata::FieldMenu::ROTOM_CHAR_COUNTER];

  // 各カウンタを取得
  for( u32 i=0; i<Savedata::FieldMenu::ROTOM_CHAR_COUNTER; i++ )
  {
    val_tbl[i] = m_saveFieldMenu->GetRotomCharacterCounter( chara_tbl[i] ) * mul_tbl[i];
  }

  u32 max_bit = 0;  // 最大値の場合、各ビットが1
  u32 bit_cnt = 0;  // 最大値の数

  // 最大値を取得
  for( u32 i=0; i<Savedata::FieldMenu::ROTOM_CHAR_COUNTER; i++ )
  {
    bool is_max = true;
    for( u32 j=0; j<Savedata::FieldMenu::ROTOM_CHAR_COUNTER; j++ )
    {
      if( i != j )
      {
        if( val_tbl[i] < val_tbl[j] )
        {
          is_max = false;
          break;
        }
      }
    }
    if( is_max != false )
    {
      max_bit |= ( 1 << i );
      bit_cnt++;
    }
  }

  Savedata::FieldMenu::RotomCharacter old_chara = GetCharacter();
  Savedata::FieldMenu::RotomCharacter new_chara = old_chara;

  if( bit_cnt == 0 || max_bit == 0 )
  {
    GFL_ASSERT( 0 );
  }
  else
  {
    // 全て0
    if( bit_cnt == Savedata::FieldMenu::ROTOM_CHAR_COUNTER && val_tbl[0] == 0 )
    {
      if( old_chara == Savedata::FieldMenu::ROTOM_CHAR_SHY )
      {
        new_chara = chara_tbl[ System::GflUse::GetPublicRand(Savedata::FieldMenu::ROTOM_CHAR_COUNTER) ];
      }
    }
    // それ以外
    else
    {
      for ever
      {
        u32 rand = System::GflUse::GetPublicRand( Savedata::FieldMenu::ROTOM_CHAR_COUNTER );
        if( ( max_bit & (1<<rand) ) != 0 )
        {
          if( old_chara != chara_tbl[rand] || bit_cnt == 1 )
          {
            new_chara = chara_tbl[rand];
            break;
          }
        }
      }
    }
  }

  if( new_chara != old_chara )
  {
    m_saveFieldMenu->SetRotomCharacter( new_chara );
  }
  m_saveFieldMenu->InitRotomCharacterCounter();
}

//-----------------------------------------------------------------------------
/**
 * @func    IsStartRotopon
 * @brief   ロトポンを開始できるか
 * @date    2016.12.06
 *
 * @retval  true  = できる
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool RotomFriendship::IsStartRotopon(void)
{
  // ロトムのお題をクリアしたか
  if( m_saveFieldMenu->IsRotomRequestClearFlag() != false )
  {
    return true;
  }

  // 親密度
  if( m_saveFieldMenu->IsFriendshipRotoponStart() != false )
  {
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    ResetRotopon
 * @brief   ロトポンの情報をリセット
 * @date    2016.12.06
 */
//-----------------------------------------------------------------------------
void RotomFriendship::ResetRotopon(void)
{
  // ロトムのお題をクリア
  if( m_saveFieldMenu->IsRotomRequestClearFlag() != false )
  {
    m_saveFieldMenu->SetRotomRequest( Savedata::FieldMenu::ROTOM_REQ_NONE );
    m_saveFieldMenu->SetRotomRequestClearFlag( 0 );
  }
  // 親密度
  else
  {
    m_saveFieldMenu->ClearFriendshipRotopon();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsSetRotomRequest
 * @brief   ロトムのお題を設定できるか
 * @date    2016.12.06
 *
 * @retval  true  = できる
 * @retval  falae = それ以外
 */
//-----------------------------------------------------------------------------
bool RotomFriendship::IsSetRotomRequest(void)
{
  return !m_saveFieldMenu->IsRotomRequestClearFlag();
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRotomRequest
 * @brief   ロトムのお題を設定
 * @date    2016.12.06
 *
 * @param   req   お題
 */
//-----------------------------------------------------------------------------
void RotomFriendship::SetRotomRequest( Savedata::FieldMenu::RotomRequest req )
{
  if( m_saveFieldMenu->IsRotomRequestClearFlag() != false )
  {
    GFL_ASSERT( 0 );
    return;
  }
  m_saveFieldMenu->SetRotomRequest( req );
}

//-----------------------------------------------------------------------------
/**
 * @func    LotRotomRequest
 * @brief   ロトムのお題を抽選
 * @date    2016.12.06
 *
 * @return  抽選結果
 */
//-----------------------------------------------------------------------------
Savedata::FieldMenu::RotomRequest RotomFriendship::LotRotomRequest(void)
{
/* @note 上書きを許可 2017/03/23
  if( m_saveFieldMenu->GetRotomRequest() != Savedata::FieldMenu::ROTOM_REQ_NONE )
  {
    return Savedata::FieldMenu::ROTOM_REQ_NONE;
  }
*/

  if( m_eventWork->CheckEventFlag(SYS_FLAG_GAME_CLEAR) != false )
  {
#if PM_DEBUG
    if( m_debugPlayData->DebugGetRotomRequest() != Savedata::FieldMenu::ROTOM_REQ_NONE )
    {
      return m_debugPlayData->DebugGetRotomRequest();
    }
#endif  // PM_DEBUG
    u32 random = System::GflUse::GetPublicRand( 100 );

    if( random >= Savedata::FieldMenu::ROTOM_REQ_FINDER_STUDIO && random <= Savedata::FieldMenu::ROTOM_REQ_FES_CIRCLE )
    {
      return static_cast<Savedata::FieldMenu::RotomRequest>( random );
    }
  }

  return Savedata::FieldMenu::ROTOM_REQ_NONE;
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckRotomTalkStart
 * @brief   ロトムの会話が発生するか
 * @date    2016.12.07
 *
 * @retval  true  = する
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool RotomFriendship::CheckRotomTalkStart(void)
{
  if( m_saveFieldMenu->IsRotomTalkStart() != false )
  {
    m_saveFieldMenu->InitRotomTalkStartCount();
    return true;
  }
  return false;
}


GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)
