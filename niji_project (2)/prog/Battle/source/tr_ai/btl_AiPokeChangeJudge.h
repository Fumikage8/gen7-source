/**
 * @file btl_AiPokeChangeJudge.h
 * @brief 戦闘AI( ポケモン入れ替えの判定 )
 * @author obata_toshihiro
 * @date 2015.04.03
 */

#ifndef BTL_AI_POKECHANGE_JUDGE_H_INCLUDED
#define BTL_AI_POKECHANGE_JUDGE_H_INCLUDED

#include "../btl_common.h"
#include "./btl_AiJudge.h"
#include "./btl_AiScript.h"


GFL_NAMESPACE_BEGIN( btl )

class MainModule;
class ServerFlow;
class BTL_CLIENT;
class POKECON;
class BTL_POKEPARAM;
class AiScript;
class AiScriptHandler;
class AiScriptCommandHandler;



/**
 * @class AiPokeChangeJudge
 * @brief 戦闘AI( 「ポケモンを入れ替える」に対する採点を行う )
 */
class AiPokeChangeJudge : public AiJudge
{
  GFL_FORBID_COPY_AND_ASSIGN( AiPokeChangeJudge );

public:

  //---------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param heapHandle      スクリプトの実行に使用するヒープ
   * @param aiScript        スクリプトの実行に使用する AiScript
   * @param mainModule      関連づける MainModule
   * @param serverFlow      関連づける ServerFlow
   * @param pokecon         関連づけるメインモジュール
   * @param randContext     使用する乱数コンテキスト
   * @param scriptBit       実行するスクリプトNo.のビットフラグ
   */
  //---------------------------------------------------------------------------
  AiPokeChangeJudge( 
    HeapHandle heapHandle, 
    AiScript* aiScript, 
    const MainModule* mainModule, 
    ServerFlow* serverFlow, 
    const POKECON* pokecon,
    const u32 randSeed,
    u32 scriptBit );

  //---------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //---------------------------------------------------------------------------
  virtual ~AiPokeChangeJudge();

  //---------------------------------------------------------------------------
  /**
   * @brief 判定計算を開始する
   * @param targetFrontPoke  判定対象の、場にいるポケモン
   */
  //---------------------------------------------------------------------------
  void StartJudge( const BTL_POKEPARAM* targetFrontPoke );

  //---------------------------------------------------------------------------
  /**
   * @brief 判定計算を更新する
   */
  //---------------------------------------------------------------------------
  virtual void UpdateJudge( void );

  //---------------------------------------------------------------------------
  /**
   * @brief 判定計算が終了したか？
   * @retval true   終了した
   * @retval false  終了していない
   */
  //---------------------------------------------------------------------------
  virtual bool IsJudgeFinished( void ) const;



  /**
   * @brief 判定結果
   */
  struct JudgeResult
  {
    bool isPokeChangeEnable; // 「入れ替え」を行うか？
    s32 score;               // 「入れ替え」の点数
    s32 inPokePartyIndex;    // 場に出るポケモンのパーティー内インデックス
  };

  //---------------------------------------------------------------------------
  /**
   * @brief 判定結果を取得する
   */
  //---------------------------------------------------------------------------
  const JudgeResult& GetJudgeResult( void ) const;




private:
  void InitJudgePokeData( void );
  void StartScript( void );
  const BTL_POKEPARAM* GetCurrentJudgePokeParam( void ) const;
  void RegisterScriptResult( void );
  void SetupJudgeResult( void );
  s32 GetBestScoredJudgePokeIndex( void ) const;


private:

  /**
   * @brief 判定対象ポケモンごとのデータ
   */
  struct JudgePokeData
  {
    u8 inPartyIndex;      // パーティー内インデックス
    bool isChangeEnable;  //「入れ替え」可能か？
    s32 score;            //「入れ替え」点数
  };

  const MainModule* m_mainModule;
  const BTL_CLIENT* m_client;
  const POKECON* m_pokecon;
  const BTL_POKEPARAM* m_targetFrontPoke;

  AIRandSys* m_randGenerator;
  AiScript* m_script;
  AiScriptHandler* m_scriptHandler;
  AiScriptCommandHandler* m_scriptCommandHandler;

  u32 m_seq;
  bool m_isFinished;
  u8 m_judgePokeCount;
  u8 m_judgePokeIndex;
  JudgePokeData m_judgePokeData[ BTL_PARTY_MEMBER_MAX ];
  JudgeResult m_result;
};


GFL_NAMESPACE_END( btl )


#endif // BTL_AI_POKECHANGE_JUDGE_H_INCLUDED
