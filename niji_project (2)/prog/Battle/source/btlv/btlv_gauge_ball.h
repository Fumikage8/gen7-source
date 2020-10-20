//======================================================================
/**
 * @file btvl_gauge_ball.h
 * @date 15/06/19
 * @author ariizumi_nobuhiko
 * @brief バトル描画 上画面ボール
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BTLV_GAUGE_BALL_H__
#define __BTLV_GAUGE_BALL_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <Layout/include/gfl2_Layout.h>

#include <AppLib/include/Util/app_util_2d.h>

GFL_NAMESPACE_BEGIN(btl)

class BTL_PARTY;
class BattleViewSystem;

/**
  * @class BtlvGaugeBall
  * @brief ゲージ基底クラス
  */
class BtlvGaugeBall
{
  GFL_FORBID_COPY_AND_ASSIGN(BtlvGaugeBall);
public:
  enum
  {
    ANM_DISP,
    ANM_BALL1,
    ANM_BALL2,
    ANM_BALL3,
    ANM_BALL4,
    ANM_BALL5,
    ANM_BALL6,
    
    ANM_MAX,

  };
  struct INIT_PARAM
  {
    int lytIdx; 
    ::app::util::G2DUtil* g2dUtil;
    gfl2::lyt::LytWk* lyt;
    gfl2::lyt::LytParts* partsLyt;
    gfl2::lyt::LytPicture *bgLyt;
    s32 anmTbl[ANM_MAX];
  };


  BtlvGaugeBall( BattleViewSystem *btlvCore , const INIT_PARAM &param , int idx );
  virtual ~BtlvGaugeBall(){}

  virtual void Update(void);

  void DispBar(void);
  void HideBar(void);
  
  void UpdateBall(void);

  void StartDispAnime(void);

  enum State {
    STATE_NORMAL,  // 通常
    STATE_SICK,    // 状態異常
    STATE_DEAD,    // 瀕死
    STATE_NULL,    // ポケモンがいない
    STATE_NUM,
  }; 
  enum Group {
    GROUP_FRIEND,  // 味方側
    GROUP_ENEMY,   // 相手側
  };
private:
  void UpdateBallAnime(void);
  //---------------------------------------------------------------------
  // ボールアイコンの状態
  //---------------------------------------------------------------------
  /**
    * @brief ボールアイコンの状態を取得する
    * @param ball_group  ボールグループ
    * @param ball_index  ボールのインデックス
    */
  State GetBallState( Group ball_group, u8 ball_index ) const;

  State GetBallState_FRIEND( u8 ball_index ) const;
  State GetBallState_ENEMY( u8 ball_index ) const;
  State GetBallState_ROYAL( u8 ball_index ) const;
  void GetFriendIndex( u8 ball_index, bool* is_friend, u8* poke_index ) const;
  void GetEnemyIndex( u8 ball_index, u8* enemy_index, u8* poke_index ) const; 
  State GetBallState( const BTL_PARTY* party, u8 poke_index ) const;

  BattleViewSystem *mBtlvCore;

  ::app::util::G2DUtil  *mG2dUtil;
  int                   mLytIndex;
  gfl2::lyt::LytWk      *mGaugeLyt;
  gfl2::lyt::LytParts   *mBarParts;
  gfl2::lyt::LytPicture *mBgLyt;

  u32 mIdx;

  s32 mAnmTbl[ANM_MAX];

  //ボール登場アニメ関連
  bool mIsFinishBallAnime;
  bool mIsPlayBallAnime;
  int mAnimeStep;
  int mAnimeCnt;
  int mAnimePlayIndex[6];

  bool mIsPlayBallSe;
  int mBallSeCnt;
};

GFL_NAMESPACE_END(btl)

#endif // __BTLV_GAUGE_BALL_H__