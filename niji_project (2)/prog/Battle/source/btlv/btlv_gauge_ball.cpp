//======================================================================
/**
 * @file btvl_gauge_status.cpp
 * @date 15/06/11
 * @author ariizumi_nobuhiko
 * @brief バトル描画 上画面ゲージ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <str/include/gfl2_Str.h>

#include <niji_conv_header/app/battle/upper/minupp_pane.h>
#include <niji_conv_header/app/battle/upper/minupp.h>

#include "System/include/GflUse.h"
#include "AppLib/include/Tool/app_tool_Gauge.h"
#include "PokeTool/include/PokeTool.h"

#include "./btlv_gauge_ball.h"
#include "./btlv_core.h"
#include "../btl_party.h"


GFL_NAMESPACE_BEGIN(btl)

BtlvGaugeBall::BtlvGaugeBall( BattleViewSystem *btlvCore , const INIT_PARAM &param , int idx )
:mBtlvCore(btlvCore)
,mG2dUtil(param.g2dUtil)
,mLytIndex(param.lytIdx)
,mGaugeLyt(param.lyt)
,mBarParts(param.partsLyt)
,mBgLyt(param.bgLyt)
,mIdx(idx)
{
  mIsPlayBallAnime = false;
  mIsPlayBallSe = false;
  mIsFinishBallAnime = false;
  for( int i=0;i<ANM_MAX;i++ )
  {
    mAnmTbl[i] = param.anmTbl[i];
  }
}

void BtlvGaugeBall::Update(void)
{
  UpdateBallAnime();
}

void BtlvGaugeBall::DispBar(void)
{
  if( mIsFinishBallAnime==false )
  {
    for( int i=0;i<6;i++ )
    {
      BtlvGaugeBall::State sta;
      if( mIdx == 0 )
      {
        sta = GetBallState( BtlvGaugeBall::GROUP_FRIEND, i );
      }
      else
      {
        sta = GetBallState( BtlvGaugeBall::GROUP_ENEMY , i );
      }
      if( sta != BtlvGaugeBall::STATE_NULL )
      {
        mG2dUtil->StartAnime( mLytIndex,mAnmTbl[ANM_BALL1+i] );
      }
    }
    mIsFinishBallAnime = true;
  }

  mG2dUtil->SetPaneVisible(mBarParts,true);
  if( mBgLyt )
  {
    mG2dUtil->SetPaneVisible(mBgLyt,true);
  }
}

void BtlvGaugeBall::HideBar(void)
{
  mG2dUtil->SetPaneVisible(mBarParts,false);
  if( mBgLyt )
  {
    mG2dUtil->SetPaneVisible(mBgLyt,false);
  }
}

#define BALL_ANIME_INTERVAL (3)
void BtlvGaugeBall::StartDispAnime(void)
{
  mG2dUtil->SetPaneVisible(mBarParts,true);
  if( mBgLyt )
  {
    mG2dUtil->SetPaneVisible(mBgLyt,true);
  }

  //ビュワーは毎回出た方がが良い
  if( mIsFinishBallAnime==false || mBtlvCore->IsViewerMode() )
  {
    mIsPlayBallAnime = true;
    mAnimeStep = 0;
    mAnimeCnt = BALL_ANIME_INTERVAL+1;
    mBallSeCnt = 0;
    mIsPlayBallSe = true;

    //ボール状況の確認
    int dataNum = 0;
    for( int i=0;i<6;i++ )
    {
      mAnimePlayIndex[i] = -1;
      BtlvGaugeBall::State sta;
      if( mIdx == 0 )
      {
        sta = GetBallState( BtlvGaugeBall::GROUP_FRIEND, i );
      }
      else
      {
        sta = GetBallState( BtlvGaugeBall::GROUP_ENEMY , i );
      }
      if( sta != BtlvGaugeBall::STATE_NULL )
      {
        mAnimePlayIndex[dataNum] = i;
        dataNum++;
      }
    }
    //momijiはデザイン上ランダム廃止
#if 0
    //ソート(乱数の配列作って、一緒にソート)
    if( dataNum > 1 )
    {
      u32 temArr[6];
      for( int i=0;i<dataNum;i++ )
      {
        temArr[i] = System::GflUse::GetPublicRand();
      }
      for( int i=0;i<dataNum;i++ )
      {
        for( int j=0;j<dataNum-i;j++ )
        {
          if( temArr[i] < temArr[j] )
          {
            std::swap(temArr[i],temArr[j]);
            std::swap(mAnimePlayIndex[i],mAnimePlayIndex[j]);
          }
        }
      }
    }
#endif
    if( mAnmTbl[ANM_DISP] != -1 )
    {
      mG2dUtil->StartAnime( mLytIndex,mAnmTbl[ANM_DISP] );
    }
  }
}

void BtlvGaugeBall::UpdateBallAnime(void)
{
  if( mAnmTbl[ANM_DISP] == -1 || mG2dUtil->IsAnimeEnd( mLytIndex,mAnmTbl[ANM_DISP] ) )
  {
    if( mIsPlayBallAnime == true )
    {
      if( mAnimeCnt >= BALL_ANIME_INTERVAL )
      {
        if( mAnimeStep < 6 && mAnimePlayIndex[mAnimeStep] != -1 )
        {
          mG2dUtil->StartAnime( mLytIndex,mAnmTbl[ANM_BALL1 + mAnimePlayIndex[mAnimeStep]] );
          mAnimeStep++;
          mAnimeCnt = 0;
        }
        else
        {
          mIsPlayBallAnime = false;
        }
      }
      mAnimeCnt++;
    }
    //SEをずらして鳴らすため。
    if( mIsPlayBallSe == true )
    {
      static const int START_SE_FRAME = 5;
      if( mBallSeCnt < START_SE_FRAME + BALL_ANIME_INTERVAL * 6)
      {
        if( mBallSeCnt - START_SE_FRAME > 0 &&
            (mBallSeCnt - START_SE_FRAME)%BALL_ANIME_INTERVAL == 0 &&
            mAnimePlayIndex[((mBallSeCnt - START_SE_FRAME)/BALL_ANIME_INTERVAL)-1] != -1 )
        {
          mBtlvCore->PlaySe(SEQ_SE_SYS_NJ_007);
        }
        mBallSeCnt++;
      }
      else
      {
        mIsPlayBallSe = false;
        mIsFinishBallAnime = true;
      }
    }
  }
}

void BtlvGaugeBall::UpdateBall(void)
{

  static const u32 paneArr[6] =
  {
    PANENAME_BATTLE_PLT_UPP_002_PANE_ICONBALL_00,
    PANENAME_BATTLE_PLT_UPP_002_PANE_ICONBALL_01,
    PANENAME_BATTLE_PLT_UPP_002_PANE_ICONBALL_02,
    PANENAME_BATTLE_PLT_UPP_002_PANE_ICONBALL_03,
    PANENAME_BATTLE_PLT_UPP_002_PANE_ICONBALL_04,
    PANENAME_BATTLE_PLT_UPP_002_PANE_ICONBALL_05,
  };

  gfl2::lyt::LytMultiResID& rMultiResId = mG2dUtil->GetLayoutResourceID( mLytIndex );

  for( int i=0;i<6;i++ )
  {
    BtlvGaugeBall::State sta;
    if( mIdx == 0 )
    {
      sta = GetBallState( BtlvGaugeBall::GROUP_FRIEND, i );
    }
    else
    {
      sta = GetBallState( BtlvGaugeBall::GROUP_ENEMY , i );
    }
      

    gfl2::lyt::LytPicture *ballPane = mGaugeLyt->GetPicturePane( mBarParts , paneArr[i], &rMultiResId );
    const gfl2::lyt::LytRes *res =  mGaugeLyt->GetResource();
    

    const nw::lyt::TextureInfo *info;
    switch( sta )
    {
    case BtlvGaugeBall::STATE_NORMAL:  // 通常
      mG2dUtil->SetPaneVisible(ballPane,true);
      info = res->GetTexture(LYTRES_MINUPP_BATTLE_ICNBALL_00_BFLIM,rMultiResId.GetMultiID());
      break;
    case BtlvGaugeBall::STATE_SICK:    // 状態異常
      mG2dUtil->SetPaneVisible(ballPane,true);
      info = res->GetTexture(LYTRES_MINUPP_BATTLE_ICNBALL_01_BFLIM,rMultiResId.GetMultiID());
      break;
    case BtlvGaugeBall::STATE_DEAD:    // 瀕死
      mG2dUtil->SetPaneVisible(ballPane,true);
      info = res->GetTexture(LYTRES_MINUPP_BATTLE_ICNBALL_02_BFLIM,rMultiResId.GetMultiID());
      break;
    case BtlvGaugeBall::STATE_NULL:    // ポケモンがいない
    default:
      mG2dUtil->SetPaneVisible(ballPane,false);
      info = NULL;
      //      info = res->GetTexture(LYTRES_MINUPP_BATTLE_ICON_BALLNULL_00_BFLIM,rMultiResId.GetMultiID());
      //      info = mLytGauge.GetLytWk()->GetResource()->GetResourceAccessor()->GetTexture( "ball_null.bclim" );
      break;
    }

    if(info)
      {
        ballPane->GetMaterial(0)->SetTexMap(0,info);
      }
  }

}

//-------------------------------------------------------------------------
/**
  * @brief ボールアイコンの状態を取得する
  * @param ball_group  ボールグループ
  * @param ball_index  ボールのインデックス
  */
//-------------------------------------------------------------------------
BtlvGaugeBall::State BtlvGaugeBall::GetBallState( Group ball_group, u8 ball_index ) const
{
  if( mBtlvCore->IsViewerMode() )
  {
    return STATE_NORMAL;
  }

  switch( ball_group )
  {
  case GROUP_FRIEND:
    return GetBallState_FRIEND( ball_index );

  case GROUP_ENEMY:
    if( mBtlvCore->GetBattleRule() == BTL_RULE_ROYAL ) 
    {
      return GetBallState_ROYAL( ball_index );
    }
    else
    {
      return GetBallState_ENEMY( ball_index );
    }

  default:
    GFL_ASSERT(0);
    break;
  }

  return STATE_NORMAL;
}

//-----------------------------------------------------------------------------------------------
//以下sangoのBallIconから移植
/**
  * @brief 味方側のボールアイコンの状態を取得する
  * @param ball_index  ボールのインデックス
  */
BtlvGaugeBall::State BtlvGaugeBall::GetBallState_FRIEND( u8 ball_index ) const
{
  bool is_friend = false;
  u8 poke_index = 0;
  this->GetFriendIndex( ball_index, &is_friend, &poke_index );

  const btl::BTL_PARTY* btl_party = 
    ( is_friend ) ? 
    ( mBtlvCore->GetBattleParty_Friend() ) : 
    ( mBtlvCore->GetBattleParty_Self() );

  return this->GetBallState( btl_party, poke_index );
}

/**
  * @brief ボールに対応する味方ポケモンのインデックスを取得する
  * @param[in]  ball_index   ボールのインデックス
  * @param[out] is_friend    味方か？
  * @param[out] poke_index   パーティ内インデックス
  */
void BtlvGaugeBall::GetFriendIndex( u8 ball_index, bool* is_friend, u8* poke_index ) const
{
  //u8 self_clientID = mBtlvCore->GetClientID();
  u8 friend_cliendID = mBtlvCore->GetFriendCleintID();

  // 味方なし
  if( friend_cliendID == BTL_CLIENTID_NULL ) {  
    *is_friend = false;
    *poke_index = ball_index;
    return;
  }
      
  // 味方あり
  // シナリオ中のAIマルチバトル( 味方がNPC && バトルハウスでない )なら、
  // 自分のポケモンを6体まで使用できるため、ボールアイコンはすべて自分のポケモンを表す
  if( ( mBtlvCore->IsClientNpc( friend_cliendID ) ) && 
      ( mBtlvCore->GetBattleCompetitor() != BTL_COMPETITOR_INST ) ) {
    *is_friend = false;
    *poke_index = ball_index;
    return;
  }
      
  // 味方あり
  // 自分が左側の場合
  if( mBtlvCore->IsPlayerInLeftSide() ) {    
    *is_friend = ( 3 <= ball_index );
    *poke_index = ball_index % 3;
    return;
  }
      
  // 味方あり
  // 自分が右側の場合
  {
    *is_friend = ( ball_index < 3 );
    *poke_index = ball_index % 3;
    return;
  }
} 

/**
  * @brief 該当するボールの状態を決定する
  * @param party       パーティ
  * @param poke_index  パーティ内インデックス
  */
BtlvGaugeBall::State BtlvGaugeBall::GetBallState( const BTL_PARTY* party, u8 poke_index ) const
{
  u32 member_count = party->GetMemberCount(); 
  if( member_count <= poke_index ) {
    return STATE_NULL;
  }

  const btl::BTL_POKEPARAM* btl_param = party->GetMemberDataConst( poke_index );
  if( btl_param->GetSrcData()->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) ) {
    return STATE_NULL;
  }

  if( btl_param->IsDead() ) {
    return STATE_DEAD;
  }

  if( btl_param->GetPokeSick() != pml::pokepara::SICK_NULL ) {
    return STATE_SICK;
  }

  return STATE_NORMAL;
}

/**
  * @brief 相手側のボールアイコンの状態を取得する
  * @param ball_index  ボールのインデックス
  */
BtlvGaugeBall::State BtlvGaugeBall::GetBallState_ENEMY( u8 ball_index ) const
{
  u8 enemy_index = 0;
  u8 poke_index = 0;
  this->GetEnemyIndex( ball_index, &enemy_index, &poke_index ); 
  const btl::BTL_PARTY* btl_party = mBtlvCore->GetBattleParty_Enemy( enemy_index );
  return this->GetBallState( btl_party, poke_index );
}

/**
  * @brief ボールに対応する相手ポケモンのインデックスを取得する
  * @param[in]  ball_index   ボールのインデックス
  * @param[out] enemy_index  何番目の敵か？
  * @param[out] poke_index   パーティ内インデックス
  */
void BtlvGaugeBall::GetEnemyIndex( u8 ball_index, u8* enemy_index, u8* poke_index ) const
{
  if( mBtlvCore->IsMultiMode() ) 
  {
    //GFBTS2328 敵側のゲージは左右が逆。
    *enemy_index = 1-(ball_index / 3);
    *poke_index = ball_index % 3;
  }
  else
  {
    *enemy_index = ball_index;
    *poke_index = ball_index;
  }
} 
//以上sangoのBallIconから移植
//-----------------------------------------------------------------------------------------------

BtlvGaugeBall::State BtlvGaugeBall::GetBallState_ROYAL( u8 ball_index ) const
{
  const btl::BTL_PARTY* btl_party = mBtlvCore->GetBattleParty_Enemy( mIdx-1 );
  return this->GetBallState( btl_party, ball_index );
}

GFL_NAMESPACE_END(btl)
