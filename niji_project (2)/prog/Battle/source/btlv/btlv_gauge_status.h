//======================================================================
/**
 * @file btvl_gauge_status.h
 * @date 15/06/11
 * @author ariizumi_nobuhiko
 * @brief バトル描画 上画面ゲージ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BTLV_GAUGE_STATSU_H__
#define __BTLV_GAUGE_STATSU_H__
#pragma once

#include <Print/include/SystemFont.h>

#include <macro/include/gfl2_Macros.h>
#include <Layout/include/gfl2_Layout.h>

#include <AppLib/include/Util/app_util_2d.h>


GFL_NAMESPACE_BEGIN(btl)

GFL_NAMESPACE_BEGIN(app)
class BattleUIDataManager;
GFL_NAMESPACE_END(app)

class BTL_POKEPARAM;
class BattleViewSystem;

/**
  * @class BtlvGaugeBase
  * @brief ゲージ基底クラス
  */
class BtlvGaugeBase
{
  GFL_FORBID_COPY_AND_ASSIGN(BtlvGaugeBase);
public:
  static const int GAUGE_ANIME_DEF_FRAME = 40;
  static const int EXP_GAUGE_SIZE = 58;

  enum
  {
    ANM_GAUGE_IN,

    ANM_GENDER_MALE,
    ANM_GENDER_FEMALE,

    ANM_HP_GREEN,
    ANM_HP_YELLOW,
    ANM_HP_RED,
    ANM_HP_VALUE,

    ANM_SICK_LOOP,

    ANM_BATSUGUN,

    ANM_GLOVE_BLUE,     
    ANM_GLOVE_YELLOW,
    ANM_GLOVE_GREEN,
    ANM_GLOVE_RED,

    ANM_ICON_MEGA_EVOLVE,
    ANM_ICON_GENSIKAIKI_ALPHA,
    ANM_ICON_GENSIKAIKI_OMEGA,

    ANM_MAX,
  };

  enum
  {
    PARTS_GENDER_ICON,
    PARTS_SICK_ICON,
    PARTS_MEGA_ICON,
    PARTS_GAUGE1,
    PARTS_GAUGE2, //下画面は２段になっている
    PARTS_EXP_GAUGE,
    PARTS_NUM_HP_MIN,
    PARTS_NUM_HP_MAX,
    PARTS_NUM_HP_SLASH,

    PARTS_CAPTURE_ICON,

    PARTS_MAX,
  };
  struct INIT_PARAM
  {
    int lytIdx; 

    //  バトルで共通で使用するパラメータ
    app::BattleUIDataManager* dataManager;

    ::app::util::G2DUtil* g2dUtil;
    gfl2::lyt::LytWk* lyt;
    gfl2::lyt::LytParts* partsLyt;
    gfl2::lyt::LytPane *baseLyt;
    s32 anmTbl[ANM_MAX];
    s32 subPartsLyt[PARTS_MAX];
    s32 royalGloveLyt;
  };


  BtlvGaugeBase( BattleViewSystem *btlvCore , const INIT_PARAM &param , int idx );
  virtual ~BtlvGaugeBase();

  virtual void Update(void);

  virtual void SetPokeParam(BTL_POKEPARAM *param);

  void UpdateReq(void){mUpdateReq = true; }
  void UpdateForce(void){UpdateParam(); }

  void DispGauge(void);
  void HideGauge(void);
  
  bool StartGaugeInAnime(void);
  bool WaitGaugeInAnime(void);

  void StartDamageAnime(int cnt);
  bool IsDamageAnime(void){return mIsDamageAnime;}
  void PlayBatugunAnime(void);

  void ExpAnime(u32 value);
  void ExpAnime_Lvup(void);
  void UpdateExpGauge(void);
  bool IsAnime(void);
  bool IsPinch(void);

  void InitGlove( CornerColor col );
  void DispGlove(void);
  void HideGlove(void);
  

#if KANTAIHANTAI_TEST
  void TestChainaMessage(int monsno);
  ::app::util::Heap* mpAppHeap;
#endif //KANTAIHANTAI_TEST

  
protected:
  void SetHpGauge( int maxHp , int hp , f32 hpF32 );
  virtual void UpdateParam(void);

private:
  void SetNumberText( gfl2::lyt::LytTextBox *textPane , u16 strId , int val , int keta , bool isSmall );

  BattleViewSystem *mBtlvCore;
  app::BattleUIDataManager *mDataManager;

  BTL_POKEPARAM *mParam;

  ::app::util::G2DUtil  *mG2dUtil;
  int                   mLytIndex;
  gfl2::lyt::LytWk      *mGaugeLyt;
  gfl2::lyt::LytParts   *mGaugeParts;
  gfl2::lyt::LytPane    *mBaseLyt;

  u32 mIdx;
  int mDispHp;  //今表示されているHp
  int mDispTrgHp; //表示したいHp
  int mDispAnmHp; //アニメ開始時のHP

  s32 mAnmTbl[ANM_MAX];
  s32 mSubPartsLyt[PARTS_MAX];
  s32 mRoyalGloveLyt;

  bool mUpdateReq;
  int  mDamageAnimeCnt;
  int  mDamageAnimeCntMax;
  int  mDamageAnimeDelayCnt;
  bool mIsDamageAnime;
  bool mIsGaugeIn;

  bool mIsUpdateExp;
  u32  mDispExpRate;  //今表示されているExp
  u32  mDispTrgExpRate; //表示したいExp
  int  mExpAnimCnt;

  static const int MIN_EXP_ANIME_CNT = 10;  //最低10フレームは時間を取って待つ

};

GFL_NAMESPACE_END(btl)

#endif // __BTLV_GAUGE_STATSU_H__
