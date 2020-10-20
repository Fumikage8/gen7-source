/**
 * @file btl_BattleAi.h
 * @brief 戦闘AI
 * @author obata_toshihiro
 * @date 2015.03.31
 */

#ifndef BTL_BATTLE_AI_H_INCLUDED
#define BTL_BATTLE_AI_H_INCLUDED

#include "../btl_common.h"


GFL_NAMESPACE_BEGIN( btl )

class MainModule;
class BTL_CLIENT;
class POKECON;
class AiScript;
class AiJudge;
class AiItemJudge;
class AiPokeChangeJudge;
class AiWazaJudge;
class SenarioAiZenryokuWazaSelector;


/**
 * @class BattleAi
 * @brief 戦闘AI
 *        ターン毎に、自身の行動を決定する
 */
class BattleAi
{
  GFL_FORBID_COPY_AND_ASSIGN( BattleAi );

public:

  /**
   * @brief AIの行動
   */
  enum AiAction
  {
    AIACT_WAZA,         // 技を使う
    AIACT_ITEM,         // 道具を使う
    AIACT_POKECHANGE,   // ポケモンを入れ替える
    AIACT_ESCAPE,       // にげる
    AIACT_WARUAGAKI,    // わるあがき
    AIACT_NUM,
    AIACT_NULL,
  };


  /**
   * @param セットアップに必要なパラメータ
   */
  struct SetupParam
  {
    const MainModule* mainModule;
    ServerFlow* serverFlow;
    const POKECON* pokecon;          // 全クライアントのポケモンを保持するコンテナ
    u32 randSeed;                    // 使用する乱数コンテキスト
    u32 scriptNoBit;                 // 実行するAIスクリプトNo.のビットフラグ
  };

  //---------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param heap        インスタンスの生成に使用するヒープ
   * @param setupParam  セットアップパラメータ
   */
  //---------------------------------------------------------------------------
  BattleAi( HeapHandle heap, const SetupParam& setupParam );

  //---------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //---------------------------------------------------------------------------
  virtual ~BattleAi();




  //---------------------------------------------------------------------------
  /**
   * @brief 実行するスクリプトを変更する
   * @param scriptNoBit  実行するAIスクリプトNo.のビットフラグ
   */
  //---------------------------------------------------------------------------
  void ChangeScript( u32 scriptNoBit );




  /**
   * @param 行動選択の開始に必要なパラメータ
   */
  struct StartParam
  {
    u8 pokeId;                              // 行動決定対象ポケモンのID
    u8 selectableWazaFlags[ PTL_WAZA_MAX ]; // 所持技を使用できるか？
    u16 itemId;                             // 使用候補の道具
    bool canUseZenryokuWaza;                // 全力技を使用可能か？
  };

  //---------------------------------------------------------------------------
  /**
   * @biref 行動選択を開始する
   * @param heapForScriptExec  スクリプトの実行に使用する一時ヒープ
   * @param startParam         開始パラメータ
   */
  //---------------------------------------------------------------------------
  void StartActionSelect( HeapHandle heapForScriptExec, const StartParam& startParam );

  //---------------------------------------------------------------------------
  /**
   * @brief 行動選択メインループ
   */
  //---------------------------------------------------------------------------
  void UpdateActionSelect( void );

  //---------------------------------------------------------------------------
  /**
   * @brief 行動選択が終了したか？
   * @retval true   終了した
   * @retval false  終了していない
   */
  //---------------------------------------------------------------------------
  bool IsActionSelectFinished( void ) const;



  /**
   * @brief 行動選択の結果
   */
  struct Result
  {
    AiAction action;  // AIが選択した行動

    // 技を使用する場合
    u8 wazaIndex;                  // 使用する技のインデックス
    bool wazaZenryokuFlag;         // 全力技として使用するか？
    btl::BtlPokePos wazaTargetPos; // 技ターゲットの位置

    // 道具を使用する場合
    u16 itemId;                    // 使用する道具No.

    // ポケモンを入れ替える場合
    u8 changePokeIndex;            // 新しく場に出すポケモンのパーティ内インデックス
  };

  //---------------------------------------------------------------------------
  /**
   * @brief 行動選択の結果を取得する
   */
  //---------------------------------------------------------------------------
  const Result& GetResult( void ) const;




private:
  bool IsItemUseEnable( void ) const;
  bool IsPokeChangeEnable( void ) const;
  bool IsWazaUseEnable( void ) const;
  bool IsEnemyExist( void ) const;
  void UpdateJudge( AiJudge* judge );
  u16 GetItemNoForWazaJudge( void ) const;
  u32 GetSelectableWazaCount( void ) const;
  void DecideAction( void );
  AiAction GetBestAction( void ) const;
  AiAction GetForcedAction( void ) const;
  AiAction GetBestScoredAction( void ) const;
  u32 storeBestScoredActions( AiAction* dest, u32 numDestArray ) const;
  AiAction SelectActionAtRandom( AiAction* actionArray, u32 actionNum ) const;
  void SetupActionParam( Result* dest, AiAction action ) const;
  void CreateHeapForScriptExec( HeapHandle heap );
  void DeleteHeapForScriptExec( void );
  bool CanScriptExec( void ) const;
  bool CheckSpecialAction( Result* result ) const;
  void setActionDataByWazaJudge( const AiWazaJudge* wazaJudge );

  static AiAction selectForcedActionByPriority( const AiAction* candidates, u32 numCandidates );


private:

  /**
   * @brief アクションの評価
   */
  enum ActionValuation
  {
    ACTION_VALUATION_SCORE,    // 最も高得点の行動を選択する
    ACTION_VALUATION_PROHIBIT, // 絶対に選択しない
    ACTION_VALUATION_FORCE,    // 強制的に選択する
  };

  /**
   * @brief アクション毎のデータ
   */
  struct ActionData
  {
    ActionValuation valuation; // 評価
    s32 score;                 // 点数
  };

  const MainModule* m_mainModule;
  const POKECON* m_pokecon;
  const BTL_CLIENT* m_client;
  const BTL_POKEPARAM* m_procPoke;

  HeapHandle m_heapForScriptExec;
  AIRandSys* m_randGenerator;
  AiScript* m_script;
  AiItemJudge* m_itemJudge;
  AiPokeChangeJudge* m_pokeChangeJudge;
  AiWazaJudge* m_wazaJudge;
  SenarioAiZenryokuWazaSelector* m_senarioZenryokuWazaSelector;

  bool m_isWazaSelectable[ PTL_WAZA_MAX ]; // 各技が選択できるかどうか

  u16 m_judgeTargetItem;
  u8 m_pokeChangeTarget;
  u8 m_selectedWazaIndex;
  BtlPokePos m_selectedTargetPos;
  bool m_canUseZenryokuWaza;
  Result m_result;

  u32 m_turnCount;
  u32 m_seq;
  bool m_isFinished;
  ActionData m_actionData[ AIACT_NUM ];

};


GFL_NAMESPACE_END( btl )


#endif // BTL_BATTLE_AI_H_INCLUDED