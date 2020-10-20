//======================================================================
/**
 * @file	btlv_debug_param.h
 * @brief	バトル描画用デバッグパラメーター
 * @author	ariizumi
 * @data	15/06/03
 */
//======================================================================
#ifndef __BTLV_DEBUG_PARAM_H__
#define __BTLV_DEBUG_PARAM_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <util/include/gfl2_std_string.h>
//BgComponentData
#include "Battle/Background/include/BgSetupUtil.h"

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Ariizumi)
  class SDCardLog;
GFL_NAMESPACE_END(Ariizumi)
GFL_NAMESPACE_END(Test)


GFL_NAMESPACE_BEGIN(btl)

#if PM_DEBUG
  //前方宣言
struct BTLV_DEBUG_PARAM;
void InitializeBattleDebugParam(BTLV_DEBUG_PARAM *param);

struct BTLV_DEBUG_PARAM
{
  BTLV_DEBUG_PARAM()
  {
    InitializeBattleDebugParam(this);
  }

  int isHioEffResource;
  int isHioBgResource;
  //エフェクトビュワー用初期ポケモン
  pml::pokepara::PokemonParam *pp[4];

  //トレーナー(-1でデフォルト chara_model_id.h)
  //Modeが0の時はchara_model_id.h
  //Modeが1の時はTrType
  int mTrainerTypeMode;
  int mTrainerType[4];

  //モーション選択のデバッグ(-1でデフォルト)
  int mTrainerThrowType;

  //シーケンスオプションの通信の擬似切り替え(-1 でふぉ、0非通信、1通信)
  int mSeqOptFakeComm;

	//デバッグバトルの時常に１
	int isDebugBattle;

  //強制色違い演出
  bool mForceRareEffect;

  //ワザチェックモード
  //背景点滅追加
  bool mWazaCheckMode;

  //メッセージ自動
  bool mIsAutoMsg;

  //自動テスト系
  bool mIsAutoTestMode; //基本的なログの表示などに使う
  bool mIsNoDraw; //描画無しモード
  Test::Ariizumi::SDCardLog *mSdLogFile;

	// btlvを処理負荷改善モードで動作させる。
	bool mIsOptimizeMode;

  //背景の外部指定(デバッグ環境用
  bool mEnebleBgSetting;
  BgComponentData  mBgComponent;
};

//デバッグパラメータの初期化(BattleSetupParamでゼロクリアされる。
//関数をもたせてclassにするものでもないので関数化
inline void InitializeBattleDebugParam(BTLV_DEBUG_PARAM *param)
{
  gfl2::std::MemClear( param, sizeof(BTLV_DEBUG_PARAM) );
  for( int i=0;i<4;i++ )
  {
    param->mTrainerType[i] = -1;
  }
  param->mTrainerTypeMode = 0;
  param->mTrainerThrowType = -1;
  param->mSeqOptFakeComm = -1;
  param->mForceRareEffect = false;
	param->mIsOptimizeMode = false;
}
#endif
GFL_NAMESPACE_END(btl)

#endif //__BTLV_DEBUG_PARAM_H__