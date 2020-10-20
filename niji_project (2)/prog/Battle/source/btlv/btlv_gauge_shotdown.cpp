//======================================================================
/**
 * @file btlv_gauge_shotdown.cpp
 * @date 2016/04/13 13:11:10
 * @author kijima_satoshi
 * @brief バトル上画面 バトルロイヤル用撃墜数ゲージ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <niji_conv_header/app/battle/upper/minupp_pane.h>       // For Layout Pane
#include <niji_conv_header/app/battle/upper/minupp_anim_list.h>  // For Layout Anime

#include "Battle/source/btlv/BtlvUiMsgWin.h"
#include "../btl_party.h"
#include "./btlv_gauge_shotdown.h"

GFL_NAMESPACE_BEGIN(btl)

//-------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-------------------------------------------------------------------------
BtlvGaugeShotDown::BtlvGaugeShotDown( BattleViewSystem *btlvCore , ::app::util::G2DUtil *g2dUtil,  app::BtlvUiMsgWin *uiMsgWin)
  :mBtlvCore(btlvCore)
  ,mG2dUtil(g2dUtil)
  ,mUiMsgWin(uiMsgWin)
{

}

//-------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-------------------------------------------------------------------------
BtlvGaugeShotDown:: ~BtlvGaugeShotDown()
{
}

//-------------------------------------------------------------------------
/**
 * @brief 使用するレイアウトの初期化
 * @param pane_id 表示対象のPane ID
 */
//-------------------------------------------------------------------------
void BtlvGaugeShotDown::Initialize( int pos )
{

  switch (pos)
    {

    case GAUGE_POS_LB:
    default:
      mPaneIdx = PANENAME_BATTLE_MIN_UPP_000_PANE_STARPLATE_LB;
      mAnmTbl[ANM_STAR1] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_LB_BALL_IN;
      mAnmTbl[ANM_STAR2] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_LB_BALL_IN_00;
      mAnmTbl[ANM_STAR3] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_LB_BALL_IN_01;
      mAnmTbl[ANM_STAR4] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_LB_BALL_IN_02;
      mAnmTbl[ANM_STAR5] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_LB_BALL_IN_03;
      mAnmTbl[ANM_STAR6] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_LB_BALL_IN_04;
      break;

    case GAUGE_POS_RB:
      mPaneIdx = PANENAME_BATTLE_MIN_UPP_000_PANE_STARPLATE_RB;
      mAnmTbl[ANM_STAR1] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_RB_BALL_IN;
      mAnmTbl[ANM_STAR2] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_RB_BALL_IN_00;
      mAnmTbl[ANM_STAR3] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_RB_BALL_IN_01;
      mAnmTbl[ANM_STAR4] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_RB_BALL_IN_02;
      mAnmTbl[ANM_STAR5] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_RB_BALL_IN_03;
      mAnmTbl[ANM_STAR6] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_RB_BALL_IN_04;
      break;

    case GAUGE_POS_LT:
      mPaneIdx = PANENAME_BATTLE_MIN_UPP_000_PANE_STARPLATE_LT;
      mAnmTbl[ANM_STAR1] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_LT_BALL_IN;
      mAnmTbl[ANM_STAR2] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_LT_BALL_IN_00;
      mAnmTbl[ANM_STAR3] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_LT_BALL_IN_01;
      mAnmTbl[ANM_STAR4] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_LT_BALL_IN_02;
      mAnmTbl[ANM_STAR5] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_LT_BALL_IN_03;
      mAnmTbl[ANM_STAR6] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_LT_BALL_IN_04;
      break;

    case GAUGE_POS_RT:
      mPaneIdx = PANENAME_BATTLE_MIN_UPP_000_PANE_STARPLATE_RT;
      mAnmTbl[ANM_STAR1] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_RT_BALL_IN;
      mAnmTbl[ANM_STAR2] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_RT_BALL_IN_00;
      mAnmTbl[ANM_STAR3] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_RT_BALL_IN_01;
      mAnmTbl[ANM_STAR4] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_RT_BALL_IN_02;
      mAnmTbl[ANM_STAR5] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_RT_BALL_IN_03;
      mAnmTbl[ANM_STAR6] = LA_MINUPP_BATTLE_MIN_UPP_000__STARPLATE_RT_BALL_IN_04;
      break;

    }

  mLytIndex = 0;
  mShotDownNum = 0;
  mDispedShotDownNum = 0;
  mDispDelayFrameCount = 0;

}


void BtlvGaugeShotDown::DispBar(void)
{
  gfl2::lyt::LytParts *partsuPane;
  partsuPane = mUiMsgWin->GetLayoutWork(0)->GetPartsPane( mPaneIdx );
  mUiMsgWin->GetG2DUtil()->SetPaneVisible( partsuPane, true );
}

void BtlvGaugeShotDown::HideBar(void)
{
  gfl2::lyt::LytParts *partsuPane;
  partsuPane = mUiMsgWin->GetLayoutWork(0)->GetPartsPane( mPaneIdx );
  mUiMsgWin->GetG2DUtil()->SetPaneVisible( partsuPane, false );
}

//-------------------------------------------------------------------------
// アニメーション再生
//-------------------------------------------------------------------------
void BtlvGaugeShotDown::StartAnime( s32 anime_num )
{
  mG2dUtil->StartAnime( mLytIndex, mAnmTbl[anime_num] );
}

//-------------------------------------------------------------------------
// アップデート
//-------------------------------------------------------------------------
void BtlvGaugeShotDown::Update(void)
{

  // 撃墜数表示のディレイ制御
  if( mShotDownNum > mDispedShotDownNum )
    {

      if( mDispDelayFrameCount > 0 )
        {
          mDispDelayFrameCount--;
        }

      if( mDispDelayFrameCount <= 0 )
        {
          mDispedShotDownNum++;
          SetShotDownMark( mDispedShotDownNum-1 );

          if( mShotDownNum > mDispedShotDownNum )
            {
              mDispDelayFrameCount = DISP_DELAY_FRAME;
            }

        }
    }

}


//-------------------------------------------------------------------------
// 撃墜数のセット
//-------------------------------------------------------------------------
void BtlvGaugeShotDown::SetShotDownCount( int n )
{
  mShotDownNum = n;

  if( mShotDownNum > STAR_MAX )
    {
      mShotDownNum = STAR_MAX;
    }

}


//-------------------------------------------------------------------------
// 撃墜マークのセット
// n = 0 - 5
//-------------------------------------------------------------------------
void BtlvGaugeShotDown::SetShotDownMark( int n )
{
  if( n >= STAR_MAX ) 
    {
      return;
    }

  StartAnime( ANM_STAR1 + n );

}

GFL_NAMESPACE_END(btl)
