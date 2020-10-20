//======================================================================
/**
 * @file BattleFes.h
 * @date 2016/11/23
 * @author munakata_kai
 * @brief バトルフェスデータ管理
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BATTLE_FES_H_INCLUDED__
#define __BATTLE_FES_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

#include "Field/FieldStatic/include/BattleInst/BattleInstTool.h"
#include "Field/FieldStatic/include/BattleInst/BattleInstData.h"

#include "Battle/include/battle_def.h" //for AI_BIT
#include <Battle/include/Battle_SetupTrainer.h>
#include <pml/include/pml_PokePara.h>

#include "niji_reference_files/script/FieldPawnTypes.h"  //BattleTreeType

#include <pml/include/item/item.h>

#include "Field/FieldStatic/include/BattleFes/BattleFesDefine.h"

namespace GameSys
{
  class GameManager;
}
namespace pml
{
  class PokeParty;
  namespace pokepara{
    class PokemonParam;
  }
}
namespace Savedata
{
  class MyStatus;
}

namespace BattleInst
{
  struct BINST_TRAINER_ROM_DATA;
}


GFL_NAMESPACE_BEGIN( BattleFes )

/**
 * @class 
 */
class BattleFes
{
  GFL_FORBID_COPY_AND_ASSIGN( BattleFes );

public:

  //----------------------------------------------------------------------------
  /**
   *  @brief  コンストラクタ
  *
   *  @param  pGman    マネージャポインタ
   *  @param pHeap     ヒープ
   *
   *  @return none
   */
  //-----------------------------------------------------------------------------
  BattleFes(GameSys::GameManager *pGman, gfl2::heap::HeapBase* pHeap);

  //----------------------------------------------------------------------------
  /**
   *  @brief デストラクタ
   *
   */
  //-----------------------------------------------------------------------------
  ~BattleFes();

  //----------------------------------------------------------------------------
  /**
  *  @brief 敵トレーナーのポケモンパーティ取得
  *
  */
  //-----------------------------------------------------------------------------
  pml::PokeParty* GetEnemyTrainerPokeParty(const u32 tr_count) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  対戦トレーナーをグリッドに追加
  *
  *  @param  pos_index  モデルのインデックス
  *  @param  char_id
  *  @param  dressup                           着せ替えモデルか？
  *  @param  x                                 Ｘ座標
  *  @param  y                                 Ｙ座標
  *  @param  z                                 Ｚ座標
  *
  *  @return none
  */
  //--------------------------------------------------------------------------
  void AddTrainerObj(const Field::BATTLE_FES_POS_INDEX pos_index, const int char_id, const float x, const float y, const float z);

  //----------------------------------------------------------------------------
  /**
  *  @brief  NPCを削除申請
  *
  *  @return none
  */
  //--------------------------------------------------------------------------
  void DeleteObj();

  //----------------------------------------------------------------------------
  /**
  *  @brief  NPCを削除待ち
  *
  *  @return bool
  */
  //--------------------------------------------------------------------------
  bool WaitModelDelete();

  //----------------------------------------------------------------------------
  /**
  *  @brief  パートナートレーナーをグリッドに追加
  *
  *  @param  pos_index  モデルのインデックス
  *  @param  x                                 Ｘ座標
  *  @param  y                                 Ｙ座標
  *  @param  z                                 Ｚ座標
  *
  *  @return none
  */
  //--------------------------------------------------------------------------
  void AddDressupTrainerObj(const Field::BATTLE_FES_POS_INDEX pos_index, const float x, const float y, const float z);

  //----------------------------------------------------------------------------
  /**
  *  @brief  モデル読み込み待ち
  *
  *  @return trueで完了
  */
  //--------------------------------------------------------------------------
  bool WaitModelLoad();

  //----------------------------------------------------------------------------
  /**
  *  @brief  モデル情報の破棄
  */
  //--------------------------------------------------------------------------
  void ClearModelData(const u32 index);

  //----------------------------------------------------------------------------
  /**
  *  @brief  トレーナーキャラクターID取得
  *
  *  @param  u16 tr_count トレーナーナンバー
  *
  *  @return キャラクターID
  */
  //----------------------------------------------------------------------------- 
  u16 GetCharacterID(const u16 tr_count);

  //----------------------------------------------------------------------
  /*
  *  @brief  対戦相手のトレーナータイプ名を登録
  */
  //----------------------------------------------------------------------
  void WordSetTrainerTypeNameFromTrainerNo(const u16 buf_id, const u16 tr_count);

  //----------------------------------------------------------------------
  /*
  *  @brief  対戦相手のトレーナー名を登録
  */
  //----------------------------------------------------------------------
  void WordSetTrainerNameFromTrainerNo(const u16 buf_id, const u16 tr_count);

  //----------------------------------------------------------------------------
  /**
  *  @brief  バトル開始
  *
  *  @return none
  */
  //-----------------------------------------------------------------------------
  void StartBattle();

  //----------------------------------------------------------------------------
  /**
  *  @brief バトルに勝ったか？
  *
  *  @return　bool trueで勝利
  */
  //-----------------------------------------------------------------------------
  bool IsWin();

  //----------------------------------------------------------------------------
  /**
  *  @brief  バトル開始
  *
  *  @return none
  */
  //-----------------------------------------------------------------------------
  void EndBattle();

  //----------------------------------------------------------------------------
  /**
  *  @brief  戦うトレーナーをバトルセットアップパラメータにセットする
  *
  *  @param  dst             格納先トレーナーデータポインタ
  *  @param  pHeap           ヒープ
  *
  *  @return none
  */
  //-----------------------------------------------------------------------------
  void SetVsTrainer(BSP_TRAINER_DATA* dst, gfl2::heap::HeapBase* pHeap);

  //--------------------------------------------------------------
  /**
  * @brief 対戦ポケモンセット
  * @param party     ポケパーティ
  */
  //-------------------------------------------------------------- 
  void SetVsPokemon(pml::PokeParty* party);

  //--------------------------------------------------------------
  /**
  * @brief トレーナーデータ読み込み待ち
  * @param tr_count     トレーナーカウント
  */
  //-------------------------------------------------------------- 
  bool WaitCreatedTrainer(const u32 tr_count);

#if PM_DEBUG
  //----------------------------------------------------------------------------
  /**
  *  @brief ５人の敵トレーナーデータ＆ポケモンデータ作成
  *
  */
  //-----------------------------------------------------------------------------
  void DebugCreateEnemy(gfl2::heap::HeapBase* pHeap);
#endif

private :
  //--------------------------------------------------------------
  /**
  * トレーナー抽選
  * @param   start   開始インデックス
  * @param   width   抽選幅
  *
  * @return  抽選結果トレーナーナンバー
  *
  */
  //--------------------------------------------------------------
  u32 selectTrainerCore(const u32 tr_count, const u32 start, const u32 width);

  //----------------------------------------------------------------------------
  /**
  *  @brief ランクに応じてトレーナーＩＤを決定
  *  @return トレーナーID
  */
  //-----------------------------------------------------------------------------
  u32 selectRandamTrainer(const u32 tr_count);

  //--------------------------------------------------------------
  /**
  * ボスＩＤ取得
  */
  //--------------------------------------------------------------
  u32 selectBoss();

  //----------------------------------------------------------------------------
  /**
  *  @brief  対戦相手の決定
  *
  *  @param  tr_count
  *  @param  data_idx
  *
  *  @return none
  */
  //-----------------------------------------------------------------------------
  void makeTrainerData(const u32 tr_count, const u32 data_idx);

  //----------------------------------------------------------------------------
  /**
  *  @brief  対戦ポケモン作成→ポケモンセット
  *
  *  @param gfl2::heap::HeapBase*        ヒープ
  *  @param const u32       何番目のトレーナー
  *  @param const u32      トレーナーＩＤ（dataID）
  *  @param BATTLE_TREE_DATA* バトルツリーデータ
  *
  *  @return bool 50回抽選をした場合 true
  */
  //-----------------------------------------------------------------------------
  bool makeTrainerPokemon(gfl2::heap::HeapBase* pHeap, const u32 tr_count, BATTLE_TREE_DATA* pBattleTreeData);

  //----------------------------------------------------------------------------
  /**
  *  @brief  対戦ポケモン作成（セーブデータからポケモン作成）→ポケモンセット
  *  @param gfl2::heap::HeapBase*        ヒープ
  *  @param const u32     何番目のトレーナー
  *  @param const BATTLE_TREE_DATA&   バトルツリーデータ
  *
  */
  //-----------------------------------------------------------------------------
  void makeTrainerPokemonFromSaveData(gfl2::heap::HeapBase* pHeap, const u32 tr_count, const BATTLE_TREE_DATA* pBattleTreeData);

  //----------------------------------------------------------------------------
  /**
  *  @brief トレーナーロムデータから捕獲ボールIDを取得
  *  @param pHeap 一時確保用ヒープ
  *  @param td トレーナーデータ
  *  @return 捕獲ボールID
  */
  //-----------------------------------------------------------------------------
  item::BALL_ID getBallIDByTrainerRomData(gfl2::heap::HeapBase* pHeap, const BattleInst::BINST_TRAINER_ROM_DATA* td);

  //----------------------------------------------------------------------
  /*
  *  @brief  トレーナー名メッセージデータを作成
  */
  //----------------------------------------------------------------------
  void initTrainerMsgData(gfl2::heap::HeapBase* pHeap);

  //----------------------------------------------------------------------
  /*
  *  @brief  トレーナー名メッセージデータを削除
  */
  //----------------------------------------------------------------------
  void finalizeTrainerMsgData();

  //----------------------------------------------------------------------------
  /**
  *  @brief トレーナーデータ作成
  */
  //-----------------------------------------------------------------------------
  void createTrainer();

  //----------------------------------------------------------------------------
  /**
  *  @brief イベントトレーナーデータ作成
  */
  //-----------------------------------------------------------------------------
  void createEventTrainer();

  //----------------------------------------------------------------------------
  /**
  *  @brief トレーナーポケモン作成
  *  @param pHeap ヒープ
  *  @param tr_count トレーナーカウント
  */
  //-----------------------------------------------------------------------------
  void createTrainerPokemon(gfl2::heap::HeapBase* pHeap, const u32 tr_count);

  //----------------------------------------------------------------------------
  /**
  *  @brief 全トレーナーデータ作成リクエスト終了処理
  */
  //-----------------------------------------------------------------------------
  void allWaitCreatedTrainer();

  //----------------------------------------------------------------------------
  /**
  *  @brief  ＡＩビットセット
  *
  *  @param  const int base_ai_bit ビット
  *  @param  int rank
  *  @param  bool is_boss
  *  @param  dst                   格納先
  *
  */
  //-----------------------------------------------------------------------------
  void setAiBit(const int base_ai_bit, const u32 rank, const bool is_boss, BSP_TRAINER_DATA* dst);

#if PM_DEBUG
  //----------------------------------------------------------------------------
  /**
  *  @brief  対戦相手の決定
  *
  *  @param  tr_count
  *  @param  data_idx
  *
  *  @return none
  */
  //-----------------------------------------------------------------------------
  void debugMakeTrainerData(gfl2::heap::HeapBase* pHeap, const u32 tr_count, const u32 data_idx);
#endif

  static const int AI_BIT = (BTL_AISCRIPT_BIT_WAZA_BASIC | BTL_AISCRIPT_BIT_WAZA_STRONG | BTL_AISCRIPT_BIT_WAZA_EXPERT | BTL_AISCRIPT_BIT_POKECHANGE_BASIC);
  static const u32 BATTLE_FES_BOSS_INDEX = BattleFesDefine::FIGHT_TRAINER_MAX - 1;    // ボスＩＮＤＥＸ

  struct BattlelFesTrainerParam
  {
    u32                                   trID;             ///<
    BattleInst::BINST_TRAINER_ROM_DATA*   pRomData;         ///<
    pml::PokeParty*                       pPokeParty;
    gfl2::heap::HeapBase*                 pLocalHeapForTrainerResource;   //! トレーナデータリソース用ローカルヒープ
  };

  //ゲームシステムが持つデータ
  struct BattleFesData
  {
    BattlelFesTrainerParam                  TrainerParam[BattleFesDefine::FIGHT_TRAINER_MAX];           ///<５人の敵トレーナーパラメータ

    BattleFesData()
    {
      for (u32 index(0); index < BattleFesDefine::FIGHT_TRAINER_MAX; ++index)
      {
        TrainerParam[index].pPokeParty = NULL;
        TrainerParam[index].pRomData = NULL;
        TrainerParam[index].trID = 0;
        TrainerParam[index].pLocalHeapForTrainerResource = NULL;
      }
    }
  };

  GameSys::GameManager*                         m_pGm;                                               ///<
  BattleFesData                                 m_Data;                                              ///<
  gfl2::str::MsgData*                           m_pTrainerNameMsgData;                               ///<トレーナー名メッセージデータ
  MoveModelData                                 m_ModelData[BattleFesDefine::BFES_MODEL_DISP_MAX];   ///<
  bool                                          m_isEvent;                                           ///<レインボーロケット団イベント中か否か
};

GFL_NAMESPACE_END( BattleFes )

#endif // __BATTLE_FES_H_INCLUDED__

