// ============================================================================
/*
 * @file BattleTreeAppParam.h
 * @brief バトルツリーアプリを起動する為のパラメータです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#if !defined( BATTLE_TREE_APPPARAM_H_INCLUDE )
#define BATTLE_TREE_APPPARAM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

//  トレーナータイプ
#include "niji_conv_header/trainer/trtype_def.h"

//  人物名の文字列最大数
#include "pml/include/pml_Const.h"

//  PokeTool::SimpleParamの宣言に必要
#include "PokeTool/include/PokeTool.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)

  enum
  {
    //  選択できる最大数
    NPC_SELECT_LIST_MAX = 50,

    //  NPCの手持ち最大数
    NPC_TEMOCHI_POKE_MAX  = 2,
  };

  enum
  {
    NPC_SELECT_IDX_ERROR  = 0xffffffff,
  };

  //  NPC選択アイテム
  struct NPC_SELECT_ITEM
  {
    NPC_SELECT_ITEM()
    {
      type  = trainer::TRTYPE_TANPAN;
    }

    //  トレーナー名(名前表示に使用)
    gfl2::str::STRCODE aName[ pml::PERSON_NAME_BUFFER_SIZE ];

    //  トレーナーtype(アイコン表示に使用)
    trainer::TrType   type;

    //  手持ちポケモンパラメータ
    PokeTool::SimpleParam aPokeSimpleParam[ NPC_TEMOCHI_POKE_MAX ];
  };

  //  アプリ起動に必要なデータ
  struct APP_PARAM
  {
    APP_PARAM()
    {
      
    }

    //  入力データ(外部から設定するもの（アプリ内では参照のみだけなので値は変わらない）)
    struct inData
    {
      inData()
      {
        npcSelectListNum  = 0;
      }

      //  選択リストデータ
      NPC_SELECT_ITEM aNpcSelectItem[ NPC_SELECT_LIST_MAX ];

      //  NPC選択リスト数
      u32 npcSelectListNum;
    } in;

    //  出力データ（アプリ内で値を変えるデータ）
    struct outData
    {
      outData()
      {
        npcSelectIdx  = (u32)NPC_SELECT_IDX_ERROR;
      }

      u32 npcSelectIdx;
    } out;
  };


GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)

#endif // BATTLE_TREE_APPPARAM_H_INCLUDE
