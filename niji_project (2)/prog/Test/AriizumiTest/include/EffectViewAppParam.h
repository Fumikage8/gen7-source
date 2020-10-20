//======================================================================
/**
 * @file EffectViewAppParam.h
 * @date 2015/12/01 18:47:58
 * @author kijima_satoshi
 * @brief エフェクトビューワー用パラメーター受け渡し用クラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#ifndef __EFFECT_VIEW_APP_PARAM_H__
#define __EFFECT_VIEW_APP_PARAM_H__

#pragma once
#if PM_DEBUG

#include "Battle/include/battle_def.h"

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( fs )
class AsyncFileManager;
GFL_NAMESPACE_END( fs )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class AppRenderingManager;
class Heap;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( ui )
class UIView;
GFL_NAMESPACE_END( ui )
GFL_NAMESPACE_END( app )


GFL_NAMESPACE_BEGIN( applib )
GFL_NAMESPACE_BEGIN( frame )
class Manager;
GFL_NAMESPACE_END( frame )
GFL_NAMESPACE_END( applib )


class pml::pokepara::PokemonParam;


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Ariizumi )

//-----------------------------------------------------------------------------
/**
 *   EffectViewerへのパラメーター受け渡し用クラス
 */
//-----------------------------------------------------------------------------

class EffectViewerUiInfo
{

 public:
  enum 
  {
    POKEMON_MAX_NUM = 4,
  };

  enum EV_ACTION 
  {
    ACTION_NONE = 0,
    ACTION_BGM_PLAY,
    ACTION_BGM_STOP,
    ACTION_POKEMON_SET,
    ACTION_WAZA_PLAY,
    ACTION_DEBUG_PRINT_ON,
    ACTION_DEBUG_PRINT_OFF,
    ACTION_BTL_RULE_SET,
    ACTION_BALL_ID_SET,
    ACTION_TARGET_SET,
  };

  enum
  {
    WAZA_RANGE_SINGLE,
    WAZA_RANGE_SIDE,
    WAZA_RANGE_ENEMY,
    WAZA_RANGE_ALL,

    WAZA_RANGE_MAX,

  };

 
  // -------------------------
  // Constructor & Destructor
  // -------------------------

  EffectViewerUiInfo()
    {
      mViewerAction = ACTION_NONE;

      for (int i=0; i<POKEMON_MAX_NUM; i++)
        {
          mUI_Pp[i] = NULL;
        }

      mTargetSlot = 0;
      mSelectPokemonNum = MONSNO_NULL;
      mSelectPokemonForm = 0;
      mSelectPokemonRare = false;
      mSelectPokemonSex = 0;

      mWazaNo = 0;
      mWazaSubNo = 0;

      mBattleRule = BTL_RULE_SINGLE;

      mRangeType = 0;
      mAttackPos = 0;
      mDeffencePos = 1;

      mBall_ID[0] = 4;  // ID:4 = Monster Ball
      mBall_ID[1] = 4;

      mDispFrame = 0;
  
    };

  ~EffectViewerUiInfo(){};


  // ---------------------
  // Methods
  // ---------------------
  void SetAction( EV_ACTION act ){ mViewerAction = act; };
  void ClearAction(void){ mViewerAction = ACTION_NONE; };
  bool IsExistAction (void) 
  {
    if (mViewerAction == ACTION_NONE)
      {
        return false;
      }

    return true;
  };

  // ---------------------
  // Members
  // ---------------------

  // 実行するアクション
  EV_ACTION     mViewerAction;

  // セットするポケモン情報
  int    mTargetSlot;
  MonsNo mSelectPokemonNum;
  int    mSelectPokemonForm;
  bool   mSelectPokemonRare;
  int    mSelectPokemonSex;

  // 再生する技情報
  u32 mWazaNo;
  u32 mWazaSubNo;
  int mRangeType;
  int mAttackPos;
  int mDeffencePos;

  // 設定するバトルルール
  BtlRule mBattleRule;
  
  // トレーナー用 Ball ID
  int mBall_ID[2];

  // フレーム数表示用
  int mDispFrame;

  // バトルコア側で設定されてるポケモン情報（下画面UI初期化用）
  pml::pokepara::PokemonParam *mUI_Pp[POKEMON_MAX_NUM];

};

//-----------------------------------------------------------------------------
/**
 *      アプリ側に渡すパラメータ用構造体
 */
//-----------------------------------------------------------------------------
struct EFFECT_APP_PARAM 
{

  gfl2::fs::AsyncFileManager      *mpAsyncFileManager;      // ファイルマネージャー
  app::util::AppRenderingManager  *mpAppRenderingManager;   // レンダリングマネージャー
  app::util::Heap                 *mpAppHeap;               // アプリ用ヒープ
  gfl2::heap::HeapBase            *mpTmpHeap;               // アプリ用tmpヒープ
  applib::frame::Manager          *mpAppFrameManager;       // フレームクラス 
  EffectViewerUiInfo              *mpEffViewUiInfo;         // パラメーター受け渡し用クラス

};


GFL_NAMESPACE_END( Ariizumi )
GFL_NAMESPACE_END( Test )

#endif  //PM_DEBUG

#endif //__EFFECT_VIEW_APP_PARAM_H__
