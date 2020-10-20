// ============================================================================
/*
 * @file SuperTrainingAppParam.h
 * @brief すごい特訓アプリを起動する為のパラメータです。
 * @date 2015.12.04
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( SUPER_TRAINIG_APPPARAM_H_INCLUDE )
#define SUPER_TRAINIG_APPPARAM_H_INCLUDE
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_macros.h>
#include <util/include/gfl2_std_string.h>

#include <pml/include/pml_PokePara.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)

  //  アプリの結果
  enum AppResult
  {
    APP_RESULT_CANCEL = 0,
    APP_RESULT_GO_TRAINING,
  };

  //  使用する王冠アイテム
  enum UseCrown
  {
    USE_CROWN_GOLD  = 0,
    USE_CROWN_SILVER,
  };

  struct APP_PARAM
  {
    struct _tag_in_
    {
      //  きたえるアイテムを金の王冠か銀の王冠か指定
      UseCrown  useCrown;

      //  きたえるポケモンのパラメータ
      pml::pokepara::PokemonParam* pPokeParam;

      _tag_in_()
      {
        useCrown    = USE_CROWN_GOLD;
        pPokeParam  = NULL;
      }

    } in;

    struct _tag_out_
    {
      //  アプリ結果
      AppResult result;

      //  どのパラメータに特訓のチェックがついたか
      bool  bPowerUpArray[ pml::pokepara::POWER_NUM ];

      _tag_out_()
      {
        result  = APP_RESULT_CANCEL;
        gfl2::std::MemClear( bPowerUpArray, sizeof( bPowerUpArray ) );
      }

    } out;

    APP_PARAM()
    {
    }
  };

GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)

#endif // SUPER_TRAINIG_APPPARAM_H_INCLUDE
