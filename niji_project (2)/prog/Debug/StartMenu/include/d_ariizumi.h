//======================================================================
/**
 * @file  d_ariizumi.h
 * @brief 15/06/06
 * @author  ariizumi
 * @data  有泉テストメニュー
 */
//======================================================================

#ifndef __D_ARIIZUMI_H__
#define __D_ARIIZUMI_H__
#pragma once
#if PM_DEBUG

#include <macro/include/gfl2_Macros.h>

#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/Launcher/include/LauncherListData.h>
#include <Battle/include/battle_SetupParam.h>
#include <Battle/include/btlv/btlv_debug_param.h>

#include <Debug/include/gfl2_DebugWinItemTypes.h>

#include "App/Kawaigari/include/KawaigariProc.h"


GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)

class AriTestLauncher : public debug::launcher::ListData

{
private:
  static const u32 LIST_MAX = 24;        //!< 項目数
  static const u32 ItemList[LIST_MAX];  //!< 項目文字列データテーブル

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //-----------------------------------------------------------------------------
  AriTestLauncher(void);
  virtual ~AriTestLauncher();

  //-----------------------------------------------------------------------------
  /**
   * @func    SelectListener
   * @brief   決定時のリスナー
   * @date    2015.02.18
   *
   * @param   pos       カーソル位置 ( キャンセル時はCURSOR_POS_CANCEL )
   * @param   launcher  呼び出し元のランチャークラス
   *
   * @return  ランチャーへのリクエストID
   */
  //-----------------------------------------------------------------------------
  virtual debug::launcher::ListData::RequestID SelectListener( u32 pos, debug::launcher::LauncherProc * launcher );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetArcDataID
   * @brief   アークデータID取得
   * @date    2015.02.18
   *
   * @return  アークデータID
   */
  //-----------------------------------------------------------------------------
  virtual gfl2::fs::ArcFile::ARCDATID GetArcDataID(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetListTable
   * @brief   項目文字列データテーブル取得
   * @date    2015.02.18
   *
   * @return  文字列データテーブル
   */
  //-----------------------------------------------------------------------------
  virtual const u32 * GetListTable(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetListTable
   * @brief   項目文字列データテーブルの要素数取得
   * @date    2015.02.18
   *
   * @return  文字列データテーブルの要素数
   */
  //-----------------------------------------------------------------------------
  virtual u32 GetListTableSize(void);

private:

  void ClearBattleParam(void);
  void ClearEffviewParam(void);
  pml::pokepara::PokemonParam* ZenryokuCheckSetup(void);
  void HeapTest1(void);

  int mEffViewMonsNo;
  int mRideWareType;
  bool mSkipEncountEffect;
  bool mAiCtrl;

  BATTLE_SETUP_PARAM  mSetupParam;
  BTL_FIELD_SITUATION mSit;

  pml::PokeParty* mPlayerParty;
  pml::PokeParty* mEnemyParty;
  pml::pokepara::PokemonParam* mPlayerPokePara[6];
  pml::pokepara::PokemonParam* mEnemyPokePara[6];

public:
  int mTrainerId[3];
  btl::BTLV_DEBUG_PARAM mDebugParam;

  int mSelfPoke;
  int mSelfPokeForm;
  int mEnemyPoke;
  int mEnemyPokeForm;
  int mEnemyPokeNum;
  bool mIsLastBoss;

  int mBgGround;
  int mBgNear;
  int mBgFar;
  bool mBgSkyBox;
  bool mBgApplyEffView;
  int mBgPartySet;
  bool mMuteki;
  bool mTrainerNoBall;

  int mZenryokuChech;
private:

  // For Kawaigari
  struct KawaigariDebugParam
  {
    int  Mons_no;
    int  Form_no;
    bool Rare;
    int  Sex;
    
    int  Bad_status;

    u8  Nakayoshi;
    u8  Manpuku;
    u8  Nadenade;

    int  BgGround;
    int  BgNear;
    int  BgFar;
    bool BgSkyBox;

    int  DirtType;
    int  DirtNum;

  };

  void InitializeKawaigariParam(gfl2::heap::HeapBase *heap);
  void ClearKawaigariParam(void);
  void SetKawaigariDebugParam(void);

  App::Kawaigari::KawaigariProcParam mKawaigariProcData;
  KawaigariDebugParam mKawaigariDebugParam;

  gfl2::debug::DebugWinGroup *mpKawaigariMenuTop;

};//AriTestMainProc

GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)

#endif  //PM_DEBUG
#endif  //__D_ARIIZUMI_H__
