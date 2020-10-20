//======================================================================
/**
 * @file btlv_gauge_levelup.cpp
 * @date 2015/10/19 17:39:55
 * @author kijima_satoshi
 * @brief レベルアップ表示ウインドウ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>


#include <niji_conv_header/app/battle/upper/minupp_pane.h>       //ゲージ類初期化のため
#include <niji_conv_header/app/battle/upper/minupp_anim_list.h>  //ゲージ類初期化のため
#include <niji_conv_header/message/msg_btl_std.h>
#include <niji_conv_header/message/msg_btl_ui.h>

#include "Battle/source/btlv/BtlvUiMsgWin.h"
#include "./btlv_core.h"
#include "./btlv_gauge_levelup.h"
#include "../btl_common.h"



GFL_NAMESPACE_BEGIN(btl)


//-------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param[in] param : クラス初期化用パラメーター
 */
//-------------------------------------------------------------------------
BtlvGaugeLevelUp::BtlvGaugeLevelUp( BattleViewSystem *btlvCore , ::app::util::G2DUtil *g2dUtil,  app::BtlvUiMsgWin *uiMsgWin)
:mBtlvCore(btlvCore)
,mG2dUtil(g2dUtil)
,mUiMsgWin(uiMsgWin)
{

  mPlayingAnimeID = -1;
  mPokeParam = NULL;  // Pokemon paramater
  mLvupInfo = NULL;   // Level Up Info

}

void BtlvGaugeLevelUp::Initialize (void)
{
  mLytIndex = 0; // 現在レイアウトは１つのみ

  mGaugeLyt = mUiMsgWin->GetLayoutWork(mLytIndex);
  mLytParts = mGaugeLyt->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_PLT_LV );

  // Animation list
  mAnmTbl[ANM_IN] = LA_MINUPP_BATTLE_MIN_UPP_000__PLT_LV_IN;
  mAnmTbl[ANM_ADD] = LA_MINUPP_BATTLE_MIN_UPP_000__PLT_LV_ADD;

}



void BtlvGaugeLevelUp::DispPane(void)
{
  mG2dUtil->SetPaneVisible(mLytParts,true);
}

void BtlvGaugeLevelUp::HidePane(void)
{
  mG2dUtil->SetPaneVisible(mLytParts,false);
}

//-------------------------------------------------------------------------
/**
 * @brief アニメーション表示開始
 * @param idx     : 再生するアニメIndex (ANIM_IDXの値)
 * @param anim_id : アニメ再生チェック用ID (minupp_anim_list.h の中の定義)
 */
//-------------------------------------------------------------------------
void BtlvGaugeLevelUp::StartAnime( ANIM_IDX idx )
{
  mG2dUtil->StartAnime( mLytIndex, mAnmTbl[idx] );
  mPlayingAnimeID = mAnmTbl[idx];       // アニメ再生チェック用

  DispPane ();

}

//-------------------------------------------------------------------------
/**
 * @brief レベルアップ表示開始
 */
//-------------------------------------------------------------------------
void BtlvGaugeLevelUp::StartLevelUp()
{
  gfl2::lyt::LytMultiResID& rMultiResId = mG2dUtil->GetLayoutResourceID( mLytIndex );
  gfl2::lyt::LytTextBox *text = NULL;

  // Set parameter name
  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMNAME_00, &rMultiResId );
  SetStrIDText( text , BTLSTR_UI_LVUP_TYPE_HP );
  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMNAME_01, &rMultiResId );
  SetStrIDText( text , BTLSTR_UI_LVUP_TYPE_ATK );
  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMNAME_02, &rMultiResId );
  SetStrIDText( text , BTLSTR_UI_LVUP_TYPE_DEF );
  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMNAME_03, &rMultiResId );
  SetStrIDText( text , BTLSTR_UI_LVUP_TYPE_SATK );
  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMNAME_04, &rMultiResId );
  SetStrIDText( text , BTLSTR_UI_LVUP_TYPE_SDEF );
  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMNAME_05, &rMultiResId );
  SetStrIDText( text , BTLSTR_UI_LVUP_TYPE_SPD );
  
  // Set increase part
  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMADD_00, &rMultiResId );
  SetNumberText( text , BTLSTR_UI_LVUP ,mLvupInfo->hp, 2 , false, print::NUM_DISP_LEFT ) ;
  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMADD_01, &rMultiResId );
  SetNumberText( text , BTLSTR_UI_LVUP ,mLvupInfo->atk, 2 ,false, print::NUM_DISP_LEFT );
  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMADD_02, &rMultiResId );
  SetNumberText( text , BTLSTR_UI_LVUP ,mLvupInfo->def, 2 , false, print::NUM_DISP_LEFT );
  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMADD_03, &rMultiResId );
  SetNumberText( text , BTLSTR_UI_LVUP ,mLvupInfo->sp_atk, 2 , false, print::NUM_DISP_LEFT );
  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMADD_04, &rMultiResId );
  SetNumberText( text , BTLSTR_UI_LVUP ,mLvupInfo->sp_def, 2 , false, print::NUM_DISP_LEFT );
  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMADD_05, &rMultiResId );
  SetNumberText( text , BTLSTR_UI_LVUP ,mLvupInfo->agi, 2 , false, print::NUM_DISP_LEFT );

  // レベルアップ後のステータス
  BTL_LEVELUP_INFO lv_info;
  mPokeParam->GetPPStatus( &lv_info );

  // Set before parameter （LVアップしているので増加分を引く）
  int param;
  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMTOTAL_00, &rMultiResId );
  param = lv_info.hp;
  param -= mLvupInfo->hp;
  SetNumberText( text , BTLSTR_UI_LVUP_VALUE ,param, 3 , false, print::NUM_DISP_SPACE );

  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMTOTAL_01, &rMultiResId );
  param = lv_info.atk;
  param -= mLvupInfo->atk;
  SetNumberText( text , BTLSTR_UI_LVUP_VALUE ,param, 3 , false, print::NUM_DISP_SPACE );

  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMTOTAL_02, &rMultiResId );
  param = lv_info.def;
  param -= mLvupInfo->def;
  SetNumberText( text , BTLSTR_UI_LVUP_VALUE ,param, 3 , false, print::NUM_DISP_SPACE );

  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMTOTAL_03, &rMultiResId );
  param = lv_info.sp_atk;
  param -= mLvupInfo->sp_atk;
  SetNumberText( text , BTLSTR_UI_LVUP_VALUE ,param, 3 , false, print::NUM_DISP_SPACE );

  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMTOTAL_04, &rMultiResId );
  param = lv_info.sp_def;
  param -= mLvupInfo->sp_def;
  SetNumberText( text , BTLSTR_UI_LVUP_VALUE ,param, 3 , false, print::NUM_DISP_SPACE );

  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMTOTAL_05, &rMultiResId );
  param = lv_info.agi;
  param -= mLvupInfo->agi;
  SetNumberText( text , BTLSTR_UI_LVUP_VALUE ,param, 3 , false, print::NUM_DISP_SPACE);


  // Start anime & disp gauge
  StartAnime( BtlvGaugeLevelUp::ANM_IN );
}



//-------------------------------------------------------------------------
/**
 * @brief パラメーター加算表示開始
 */
//-------------------------------------------------------------------------
void BtlvGaugeLevelUp::StartParamAdd()
{
  StartAnime( BtlvGaugeLevelUp::ANM_ADD );
}



//-------------------------------------------------------------------------
/**
 * @brief パラメーター加算後の値をセット
 */
//-------------------------------------------------------------------------
void BtlvGaugeLevelUp::SetAddParam ()
{
  gfl2::lyt::LytMultiResID& rMultiResId = mG2dUtil->GetLayoutResourceID( mLytIndex );
  gfl2::lyt::LytTextBox *text = NULL;

  BTL_LEVELUP_INFO lvupInfo;
  mPokeParam->GetPPStatus( &lvupInfo );


  // レベルアップ後のステータス
  BTL_LEVELUP_INFO lv_info;
  mPokeParam->GetPPStatus( &lv_info );

  // Set after parameter
  int param;
  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMTOTAL_00, &rMultiResId );
  param = lv_info.hp;
  SetNumberText( text , BTLSTR_UI_LVUP_VALUE ,param, 3 , false, print::NUM_DISP_SPACE );

  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMTOTAL_01, &rMultiResId );
  param = lv_info.atk;
  SetNumberText( text , BTLSTR_UI_LVUP_VALUE ,param, 3 , false, print::NUM_DISP_SPACE );

  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMTOTAL_02, &rMultiResId );
  param = lv_info.def;
  SetNumberText( text , BTLSTR_UI_LVUP_VALUE ,param, 3 , false, print::NUM_DISP_SPACE );

  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMTOTAL_03, &rMultiResId );
  param = lv_info.sp_atk;
  SetNumberText( text , BTLSTR_UI_LVUP_VALUE ,param, 3 , false, print::NUM_DISP_SPACE );

  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMTOTAL_04, &rMultiResId );
  param = lv_info.sp_def;
  SetNumberText( text , BTLSTR_UI_LVUP_VALUE ,param, 3 , false, print::NUM_DISP_SPACE );

  text = mGaugeLyt->GetTextBoxPane( mLytParts, PANENAME_BATTLE_PLT_UPP_006_PANE_PARAMTOTAL_05, &rMultiResId );
  param = lv_info.agi;
  SetNumberText( text , BTLSTR_UI_LVUP_VALUE ,param, 3 , false, print::NUM_DISP_SPACE);

}

//-------------------------------------------------------------------------
/**
 * @brief アニメーション終了判定
 */
//-------------------------------------------------------------------------
bool BtlvGaugeLevelUp::IsAnimeEnd ()
{
  bool ret = mG2dUtil->IsAnimeEnd( mLytIndex, mPlayingAnimeID );

  // パラメーター追加アニメの最終フレームに値を更新
  if ((ret == true ) && (mPlayingAnimeID == LA_MINUPP_BATTLE_MIN_UPP_000__PLT_LV_ADD))
    {
      SetAddParam();
    }

  // アニメが終わっていて、アニメIDが入っている場合
  if ((ret == true ) && (mPlayingAnimeID != -1))
    {
      mPlayingAnimeID = -1;
    }

  return ret;
}

//-------------------------------------------------------------------------
/**
 * @brief タグを数字展開して設定
 * @param[in]   textPane       : 設定するレイアウトペイン
 * @param[in]   strId          : 設定する文字列のID（文字列マネージャのID）
 * @param[in]   val            : タグで展開する値
 * @param[in]   keta           : 表示桁数
 * @param[in]   isSmall        : 大文字/小文字
 * @param[in]   justification  : 右寄せ/左寄せ（ゼロ詰め/空白詰め）
 */
//-------------------------------------------------------------------------
void BtlvGaugeLevelUp::SetNumberText( gfl2::lyt::LytTextBox *textPane , u16 strId , int val , int keta , bool isSmall, print::NumberDispType justification )
{
  gfl2::str::StrBuf *msgStr = mBtlvCore->GetWorkStr(0);
  gfl2::str::StrBuf *expStr = mBtlvCore->GetWorkStr(1);
  print::WordSet *wset = mBtlvCore->GetWordSet();
      
  mBtlvCore->GetStringUi( msgStr , strId );
  if( isSmall )
  {
    wset->RegisterNumber( 0 , val , keta , justification , print::NUM_CODE_ZENKAKU );
  }
  else
  {
    wset->RegisterNumber( 0 , val , keta , justification , print::NUM_CODE_HANKAKU );
  }
  wset->Expand( expStr , msgStr );

  print::SetStringForLytTextBox( textPane , expStr );
}

//-------------------------------------------------------------------------
/**
 * @brief 指定した文字列IDの文字列を設定
 * @param[in]   textPane       : 設定するレイアウトペイン
 * @param[in]   strId          : 設定する文字列のID（文字列マネージャのID）
 */
//-------------------------------------------------------------------------
void BtlvGaugeLevelUp::SetStrIDText( gfl2::lyt::LytTextBox *textPane , u16 strId )
{
  gfl2::str::StrBuf *msgStr = mBtlvCore->GetWorkStr(0);

  mBtlvCore->GetStringUi( msgStr , strId );
  print::SetStringForLytTextBox( textPane , msgStr );

}

GFL_NAMESPACE_END(btl)
