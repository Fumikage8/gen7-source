//======================================================================
/**
 * @file btlv_gauge_tokusei.h
 * @date 2015/10/23 12:57:27
 * @author kijima_satoshi
 * @brief 特性表示クラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BTLV_GAUGE_TOKUSEI_H__
#define __BTLV_GAUGE_TOKUSEI_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <Layout/include/gfl2_Layout.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <Print/include/SystemFont.h>     // For SetStringForLytTextBox



GFL_NAMESPACE_BEGIN(btl)


// クラス前方宣言
class BattleViewSystem;
GFL_NAMESPACE_BEGIN(app)
class BtlvUiMsgWin;
GFL_NAMESPACE_END(app)


//----------------------------------------------------------------------
/**
 * @class BtlvGaugeTokusei
 * @brief 特性表示クラス
 */
//----------------------------------------------------------------------
class BtlvGaugeTokusei
{
  GFL_FORBID_COPY_AND_ASSIGN(BtlvGaugeTokusei);
public:

  /**
   * @enum COMBAT_SIDE
   * 戦闘でどちら側の陣営か
   */
  enum COMBAT_SIDE
  {
    SIDE_ALLY,    //!< 味方側
    SIDE_ENEMY,   //!< 敵側
    
    SIDE_MAX,
  };

  /**
   * @enum ANIM_IDX
   * アニメーションリスト
   */
  enum ANIM_IDX
  {
    ANM_IN,     //!< 表示アニメ
    ANM_OUT,    //!< 消去アニメ
    
    ANM_MAX,
  };


  // -------------------------
  // Constructor & Destructor
  // -------------------------
  BtlvGaugeTokusei( BattleViewSystem *btlvCore , ::app::util::G2DUtil*   g2dUtil, app::BtlvUiMsgWin *uiMsgWin);
  virtual ~BtlvGaugeTokusei(){}



  // -------------------------
  // Methods
  // -------------------------
  public:

  void Initialize(void);

  void StartDispAnime( COMBAT_SIDE side, BtlvPos viewPos , const BTL_POKEPARAM *poke_param );
  bool WaitDispAnime (void);

  void StartHideAnime( COMBAT_SIDE side );
  bool WaitHideAnime (void);

  bool IsDisp( COMBAT_SIDE side );

  BtlvPos GetDispPos(COMBAT_SIDE side ){return mTokWinDispPos[side];};

 private :

  void SetInfoText( COMBAT_SIDE side, const BTL_POKEPARAM *poke_param );

  // -------------------------
  // Members
  // -------------------------
  BattleViewSystem            *mBtlvCore;
  ::app::util::G2DUtil        *mG2dUtil;
  app::BtlvUiMsgWin           *mUiMsgWin;

  int                         mLytIndex;
  gfl2::lyt::LytWk            *mGaugeLyt;

  gfl2::lyt::LytParts         *mLytParts[SIDE_MAX];
  s32                         mAnmTbl[SIDE_MAX][ANM_MAX];
  
  // 表示管理用 

  COMBAT_SIDE                 mDispSide;                 // !< 現在表示中のサイド
  bool                        mIsDisp[SIDE_MAX];         // !< 表示中かどうか（表示アニメ開始から消去アニメ開始までの間trueになる）
  BtlvPos                     mTokWinDispPos[SIDE_MAX];


};      // End of class

GFL_NAMESPACE_END(btl)

#endif // __BTLV_GAUGE_TOKUSEI_H__
