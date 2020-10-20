//======================================================================
/**
 * @file btlv_gauge_tokusei.cpp
 * @date 2015/10/23 12:57:52
 * @author kijima_satoshi
 * @brief 特性表示クラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================


#include <niji_conv_header/app/battle/upper/minupp_pane.h>       //ゲージ類初期化のため
#include <niji_conv_header/app/battle/upper/minupp_anim_list.h>  //ゲージ類初期化のため
#include <niji_conv_header/message/msg_btl_std.h>

#include "Battle/source/btlv/BtlvUiMsgWin.h"
#include "./btlv_gauge_tokusei.h"



GFL_NAMESPACE_BEGIN(btl)


//-------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param [in] param : クラス初期化用パラメーター
 */
//-------------------------------------------------------------------------
BtlvGaugeTokusei::BtlvGaugeTokusei( BattleViewSystem *btlvCore , ::app::util::G2DUtil *g2dUtil,  app::BtlvUiMsgWin *uiMsgWin)
:mBtlvCore(btlvCore)
,mG2dUtil(g2dUtil)
,mUiMsgWin(uiMsgWin)
{

  mLytIndex = -1;
  mGaugeLyt = NULL;

  for( int i=0;i<SIDE_MAX;i++ )
  {
    mLytParts[i] = NULL;
    mIsDisp[i] = false;
    mTokWinDispPos[i] = BTL_VPOS_ERROR;
  }

}

//-------------------------------------------------------------------------
/**
 * @brief レイアウト情報の初期化
 */
//-------------------------------------------------------------------------
void BtlvGaugeTokusei::Initialize(void)
{
  mLytIndex = 0; // Fixed value?

  // Left side (Ally)
  mGaugeLyt = mUiMsgWin->GetLayoutWork(mLytIndex);

  mLytParts[SIDE_ALLY] = mUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_TOKUSEI_L );
  mAnmTbl[SIDE_ALLY][ANM_IN] = LA_MINUPP_BATTLE_MIN_UPP_000__TOKUSEI_L_IN_BLUE;
  mAnmTbl[SIDE_ALLY][ANM_OUT] = LA_MINUPP_BATTLE_MIN_UPP_000__TOKUSEI_L_OUT_BLUE;

  // Right Side (Enemy)
  mLytParts[SIDE_ENEMY] = mUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_TOKUSEI_R );
  mAnmTbl[SIDE_ENEMY][ANM_IN] = LA_MINUPP_BATTLE_MIN_UPP_000__TOKUSEI_R_IN_RED;
  mAnmTbl[SIDE_ENEMY][ANM_OUT] = LA_MINUPP_BATTLE_MIN_UPP_000__TOKUSEI_R_OUT_RED;

}

//-------------------------------------------------------------------------
/**
 * @brief レイアウトに情報テキストをセット
 * @param [in] side 陣営サイドの指定
 * @param [in] poke_param ポケモン情報取得用
 */
//-------------------------------------------------------------------------
void BtlvGaugeTokusei::SetInfoText( COMBAT_SIDE side, const BTL_POKEPARAM *poke_param )
{
  gfl2::lyt::LytMultiResID& rMultiResId = mG2dUtil->GetLayoutResourceID( mLytIndex );
  gfl2::lyt::LytTextBox *textBox = NULL;
  int textPaneID = -1;

  if( side == SIDE_ALLY )
  {
    textPaneID = PANENAME_BATTLE_PLT_UPP_007_PANE_TEXTBOX_00;
  }
  else
  {
    textPaneID = PANENAME_BATTLE_PLT_UPP_007_PANE_TEXTBOX_01;
  }
  textBox = mGaugeLyt->GetTextBoxPane( mLytParts[side], textPaneID, &rMultiResId );

  u8  pokeID = poke_param->GetID();
  u16 tokusei = poke_param->GetValue( BTL_POKEPARAM::BPP_TOKUSEI );

  str::MakeStringStd( mBtlvCore->GetWorkStr(0), BTL_STRID_STD_TokWin, 2, pokeID, tokusei );
  print::SetStringForLytTextBox( textBox , mBtlvCore->GetWorkStr(0) );

}

//-------------------------------------------------------------------------
/**
 * @brief 表示アニメーション開始
 */
//-------------------------------------------------------------------------
void BtlvGaugeTokusei::StartDispAnime( COMBAT_SIDE side,  BtlvPos viewPos, const BTL_POKEPARAM *poke_param )
{
  SetInfoText( side, poke_param );

  mG2dUtil->StartAnime( mLytIndex, mAnmTbl[side][ANM_IN] );

  mG2dUtil->SetPaneVisible( mLytParts[side], true );    // Visible ON
 
  mDispSide = side;
  mIsDisp[side] = true;
  mTokWinDispPos[side] = viewPos;


}

//-------------------------------------------------------------------------
/**
 * @brief 消去アニメーション開始
 */
//-------------------------------------------------------------------------
void BtlvGaugeTokusei::StartHideAnime( COMBAT_SIDE side )
{
  mG2dUtil->StartAnime( mLytIndex, mAnmTbl[side][ANM_OUT] );

  mG2dUtil->SetPaneVisible( mLytParts[side], true );    // Visible ON
 
  mDispSide = side;
  mIsDisp[side] = false;
  mTokWinDispPos[side] = BTL_VPOS_ERROR;

}

//-------------------------------------------------------------------------
/**
 * @brief 表示されているかどうか
 * 表示アニメ開始から消去アニメ開始までの間trueになる（消去アニメ開始直後にfalseになる）
 */
//-------------------------------------------------------------------------
bool BtlvGaugeTokusei::IsDisp(COMBAT_SIDE side)
{
  return mIsDisp[side];
}

//-------------------------------------------------------------------------
/**
 * @brief 表示アニメが終了しているか
 */
//-------------------------------------------------------------------------
bool BtlvGaugeTokusei::WaitDispAnime (void)
{

  if( mG2dUtil->IsAnimeEnd( mLytIndex, mAnmTbl[mDispSide][ANM_IN]) )
    {
      return true;
    }
  
  return false;
}

//-------------------------------------------------------------------------
/**
 * @brief 消去アニメが終了しているか
 */
//-------------------------------------------------------------------------
bool BtlvGaugeTokusei::WaitHideAnime (void)
{

  if( mG2dUtil->IsAnimeEnd( mLytIndex, mAnmTbl[mDispSide][ANM_OUT]) )
    {
      mG2dUtil->SetPaneVisible(mLytParts[mDispSide],false);     // Visible OFF
      return true;
    }
  
  return false;
}


GFL_NAMESPACE_END(btl)
