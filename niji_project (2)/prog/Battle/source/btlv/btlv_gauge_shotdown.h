//======================================================================
/**
 * @file btlv_gauge_shotdown.h
 * @date 2016/04/13 13:10:57
 * @author kijima_satoshi
 * @brief バトル上画面 バトルロイヤル用撃墜数ゲージ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BTLV_GAUGE_SHOTDOWN_H__
#define __BTLV_GAUGE_SHOTDOWN_H__
#pragma once


#include <Layout/include/gfl2_Layout.h>
#include <AppLib/include/Util/app_util_2d.h>


GFL_NAMESPACE_BEGIN(btl)


// クラス前方宣言
class BattleViewSystem;
GFL_NAMESPACE_BEGIN(app)
class BtlvUiMsgWin;
GFL_NAMESPACE_END(app)



//----------------------------------------------------------------------
/**
 * @class BtlvGaugeShotDown
 * @brief バトル上画面 バトルロイヤル用撃墜数ゲージクラス
 */
//----------------------------------------------------------------------
class BtlvGaugeShotDown
{
  GFL_FORBID_COPY_AND_ASSIGN(BtlvGaugeShotDown);


  // ゲージ位置
 public:
  enum
  {
    GAUGE_POS_LB = 0,
    GAUGE_POS_RB,
    GAUGE_POS_RT,
    GAUGE_POS_LT,

    STAR_MAX = 6,           // 表示できる星の最大数
    DISP_DELAY_FRAME = 10,  // 連続表示ディレイフレーム
  };

  // アニメーション
 private:
  enum
  {
    ANM_STAR1,      // 各スターの表示アニメ
    ANM_STAR2,
    ANM_STAR3,
    ANM_STAR4,
    ANM_STAR5,
    ANM_STAR6,
    
    ANM_MAX,
  };

  // -------------------------
  // Constructor & Destructor
  // -------------------------
 public:
  BtlvGaugeShotDown( BattleViewSystem *btlvCore , ::app::util::G2DUtil *g2dUtil,  app::BtlvUiMsgWin *uiMsgWin);
  virtual ~BtlvGaugeShotDown();


  // -------------------------
  // Methods
  // -------------------------
 public:
  void Initialize( int pos );

  void DispBar(void);
  void HideBar(void);

 
  void Update(void);
  void SetShotDownCount( int n );


 private:
  void SetShotDownMark( int n );
  void StartAnime( s32 anime_num );
 
  // -------------------------
  // Members
  // -------------------------
private:

  BattleViewSystem *mBtlvCore;

  ::app::util::G2DUtil  *mG2dUtil;
  app::BtlvUiMsgWin     *mUiMsgWin;

  int                   mLytIndex;
//  gfl2::lyt::LytWk      *mGaugeLyt;
//  gfl2::lyt::LytParts   *mBarParts;
//  gfl2::lyt::LytPicture *mBgLyt;

  s32                   mPaneIdx;               // Pane Index
  s32                   mAnmTbl[ANM_MAX];       // アニメーションID

  int                   mShotDownNum;           // 撃墜数
  int                   mDispedShotDownNum;     // 現在表示されている撃墜数

  int                   mDispDelayFrameCount;


}; // class BtlvGaugeShotDown

GFL_NAMESPACE_END(btl)




#endif // __BTLV_GAUGE_SHOTDOWN_H__
