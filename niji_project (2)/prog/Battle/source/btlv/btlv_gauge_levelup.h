//======================================================================
/**
 * @file btlv_gauge_levelup.h
 * @date 2015/10/19 17:39:40
 * @author kijima_satoshi
 * @brief レベルアップ表示ウインドウ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BTLV_GAUGE_LEVELUP_H__
#define __BTLV_GAUGE_LEVELUP_H__
#pragma once

#include <Print/include/SystemFont.h>     // For SetStringForLytTextBox
#include <macro/include/gfl2_Macros.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <Layout/include/gfl2_Layout.h>

#include "../btl_common.h"                // For BTL_LEVELUP_INFO



GFL_NAMESPACE_BEGIN(btl)


// クラス前方宣言
class BattleViewSystem;
GFL_NAMESPACE_BEGIN(app)
class BtlvUiMsgWin;
GFL_NAMESPACE_END(app)


//----------------------------------------------------------------------
/**
 * @class BtlvGaugeLevelUp
 * @brief レベルアップ表示ウインドウクラス
 */
//----------------------------------------------------------------------
class BtlvGaugeLevelUp
{
  GFL_FORBID_COPY_AND_ASSIGN(BtlvGaugeLevelUp);
public:


  /**
   * @enum ANIM_IDX
   * アニメーションリスト
   */
  enum ANIM_IDX
  {
    ANM_IN,     //!< レベルアップ表示開始アニメ
    ANM_ADD,    //!< パラメーター加算アニメ
    
    ANM_MAX,
  };


  // -------------------------
  // Constructor & Destructor
  // -------------------------
  BtlvGaugeLevelUp( BattleViewSystem *btlvCore , ::app::util::G2DUtil *g2dUtil,  app::BtlvUiMsgWin *uiMsgWin);
  virtual ~BtlvGaugeLevelUp(){}


  // -------------------------
  // Methods
  // -------------------------
  void Initialize (void);

  void SetPokeParam( const BTL_POKEPARAM *param ){ mPokeParam = param; }
  void SetLevelUpInfo( const BTL_LEVELUP_INFO *lvupInfo ){ mLvupInfo = lvupInfo; }

  void StartLevelUp();
  void StartParamAdd();
  bool IsAnimeEnd();

  void DispPane(void);
  void HidePane(void);


 private:
  void StartAnime( ANIM_IDX anim_idx );
  void SetAddParam ();

  void SetNumberText( gfl2::lyt::LytTextBox *textPane , u16 strId , int val , int keta , bool isSmall, print::NumberDispType justification );
  void SetStrIDText( gfl2::lyt::LytTextBox *textPane , u16 strId );


  // -------------------------
  // Members
  // -------------------------
  BattleViewSystem            *mBtlvCore;
  ::app::util::G2DUtil        *mG2dUtil;
  app::BtlvUiMsgWin           *mUiMsgWin;

  int                         mLytIndex;
  gfl2::lyt::LytWk            *mGaugeLyt;
  gfl2::lyt::LytParts         *mLytParts;
  s32                         mAnmTbl[ANM_MAX];

  int                         mPlayingAnimeID;  //!< 現在再生中のアニメID(-1で再生中無し)       
  const BTL_POKEPARAM         *mPokeParam;
  const BTL_LEVELUP_INFO      *mLvupInfo;

};      // End of class



GFL_NAMESPACE_END(btl)

#endif // __BTLV_GAUGE_LEVELUP_H__
