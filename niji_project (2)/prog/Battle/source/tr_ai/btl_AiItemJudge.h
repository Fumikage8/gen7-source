/**
 * @file btl_AiItemJudge.h
 * @brief 戦闘AI( 道具の使用判定 )
 * @author obata_toshihiro
 * @date 2015.04.03
 */

#ifndef BTL_AI_ITEM_JUDGE_H_INCLUDED
#define BTL_AI_ITEM_JUDGE_H_INCLUDED

#include "../btl_common.h"
#include "./btl_AiJudge.h"
#include "./btl_AiScript.h"


GFL_NAMESPACE_BEGIN( btl )

class MainModule;
class ServerFlow;
class POKECON;
class AiScript;
class AiScriptHandler;
class AiScriptCommandHandler;



/**
 * @class AiItemJudge
 * @brief 戦闘AI( 「道具を使用すること」に対する採点を行う )
 */
class AiItemJudge : public AiJudge
{
  GFL_FORBID_COPY_AND_ASSIGN( AiItemJudge );

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
  AiItemJudge( 
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
  virtual ~AiItemJudge();

  //---------------------------------------------------------------------------
  /**
   * @brief 判定計算を開始する
   * @param poke    判定対象のポケモン
   * @param itemNo  採点対象の道具
   */
  //---------------------------------------------------------------------------
  void StartJudge( const BTL_POKEPARAM* poke, u16 itemNo );

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

  //---------------------------------------------------------------------------
  /**
   * @brief 「道具を使用すること」に対する採点結果を取得する
   */
  //---------------------------------------------------------------------------
  s32 GetScore( void ) const;



private:
  void StartScript( void );
  void RegisterScriptResult( void );


private:

  const MainModule* m_mainModule;
  const POKECON* m_pokecon;
  const BTL_POKEPARAM* m_poke;

  AIRandSys* m_randGenerator;
  AiScript* m_script;
  AiScriptHandler* m_scriptHandler;
  AiScriptCommandHandler* m_scriptCommandHandler;
  
  u16 m_itemNo;
  s32 m_score;
  u32 m_seq;
  bool m_isFinished;
};


GFL_NAMESPACE_END( btl )


#endif // BTL_AI_ITEM_JUDGE_H_INCLUDED
