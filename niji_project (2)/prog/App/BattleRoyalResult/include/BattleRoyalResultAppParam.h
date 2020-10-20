// ============================================================================
/*
 * @file BattleRoyalResultAppParam.h
 * @brief バトルロイヤルの結果アプリを起動する為のパラメータです。
 * @date 2015.10.23
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( BATTLE_ROYAL_RESULT_APPPARAM_H_INCLUDE )
#define BATTLE_ROYAL_RESULT_APPPARAM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

//  nijiのインクルード
#include "AppLib/include/Tool/AppToolTrainerIconRendering.h"
#include "PokeTool/include/PokeTool.h"
#include "Battle/include/battle_def.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)

  //  トレーナー表示フレーム位置
  enum TrainerFramePos
  {
    TRAINER_FRAME_POS_LEFT_UP = 0,
    TRAINER_FRAME_POS_RIGHT_UP,
    TRAINER_FRAME_POS_LEFT_DOWN,
    TRAINER_FRAME_POS_RIGHT_DOWN,
    TRAINER_FRAME_POS_MAX,
  };

  //  ポケモン配置個数
  enum
  {
    POKE_DATA_MAX = 3,
  };

  //  ポケモンデータ
  struct POKE_DATA
  {
    POKE_DATA()
    {
      hp    = 0;
      maxHp = 100;
    };

    //  パラメータ
    PokeTool::SimpleParam simpleParam;
    u16 hp;
    u16 maxHp;
  };

  //  トレーナー単体のデータ
  struct TRINER_BTL_RESULT_DATA
  {
    TRINER_BTL_RESULT_DATA()
    {
      ranking   = 1;
      killCount = 0;
      pNameBuf  = NULL;
      correrColor = CORRER_COLOR_BLUE;

      //  アイコンのデフォルト設定
      trainerType = trainer::TRTYPE_HERO;
      app::tool::AppToolTrainerIconRendering::OutputDefaultMyStatusIconData( 0, &iconObjData );
    };

    //  順儀(1 ～ 4)
    u8  ranking;

    //  トレーナータイプ（顔アイコン表示の指針になる）
    trainer::TrType trainerType;

    //  トレーナー顔アイコンデータ(主人公の男女のアイコンを表示する場合は設定)
    Savedata::MyStatus::ICON_DATA iconObjData;

    //  コーナーの色
    CornerColor         correrColor;

    //  ポケモンを倒した数
    u32 killCount;

    //  名前
    gfl2::str::StrBuf*  pNameBuf;
    //  ポケモンデータ
    POKE_DATA aPokeData[ POKE_DATA_MAX ];
  };

  struct APP_PARAM
  {
    APP_PARAM()
    {
      selfRanking = 1;
    };

    //  各プレイヤーのリザルト情報
    TRINER_BTL_RESULT_DATA  aTrainerData[ TRAINER_FRAME_POS_MAX ];

    //  プレイヤー自身の順位
    u8  selfRanking;
  };

GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)

#endif // BATTLE_ROYAL_RESULT_APPPARAM_H_INCLUDE
