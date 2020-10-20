//=============================================================================
/**
 * @file    RotomFriendship.h
 * @brief   ロトム親密度関連
 * @author  Hiroyuki Nakamura
 * @date    2016.11.25
 */
//=============================================================================
#if !defined( ROTOM_FRIENDSHIP_H_INCUDED )
#define ROTOM_FRIENDSHIP_H_INCUDED

#pragma once

// gflib2
#include <macro/include/gfl2_Macros.h>
// momiji
#include "Savedata/include/FieldMenuSave.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(Field)
class EventWork;
GFL_NAMESPACE_END(Field)

#if PM_DEBUG
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)
class PlayData;
GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)
#endif // PM_DEBUG


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

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
   * @brief   コンストラクタ
   */
  //-----------------------------------------------------------------------------
  RotomFriendship(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~RotomFriendship();

  //-----------------------------------------------------------------------------
  /**
   * @func    GetRank
   * @brief   親密度ランクを取得
   * @date    2016.11.25
   *
   * @return  親密度ランク
   */
  //-----------------------------------------------------------------------------
  Savedata::FieldMenu::RotomRank GetRank(void);

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
  void Add( s32 val, bool is_touch );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetCharacter
   * @brief   性格取得
   * @date    2016.11.28
   *
   * @return  性格
   */
  //-----------------------------------------------------------------------------
  Savedata::FieldMenu::RotomCharacter GetCharacter(void);

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
  bool IsStartRotopon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ResetRotopon
   * @brief   ロトポンの情報をリセット
   * @date    2016.12.06
   */
  //-----------------------------------------------------------------------------
  void ResetRotopon(void);

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
  bool IsSetRotomRequest(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomRequest
   * @brief   ロトムのお題を設定
   * @date    2016.12.06
   *
   * @param   req   お題
   */
  //-----------------------------------------------------------------------------
  void SetRotomRequest( Savedata::FieldMenu::RotomRequest req );

  //-----------------------------------------------------------------------------
  /**
   * @func    LotRotomRequest
   * @brief   ロトムのお題を抽選
   * @date    2016.12.06
   *
   * @return  抽選結果
   */
  //-----------------------------------------------------------------------------
  Savedata::FieldMenu::RotomRequest LotRotomRequest(void);

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
  bool CheckRotomTalkStart(void);


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeCharacter
   * @brief   性格変更
   * @date    2016.11.28
   */
  //-----------------------------------------------------------------------------
  void ChangeCharacter(void);


private:
  Savedata::FieldMenu * m_saveFieldMenu;  //!< フィールドメニューセーブデータ
  const Field::EventWork * m_eventWork;   //!< イベントワーク

#if PM_DEBUG
  App::FieldMenu::PlayData * m_debugPlayData;
#endif  // PM_DEBUG
};

GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)

#endif  // ROTOM_FRIENDSHIP_H_INCUDED
