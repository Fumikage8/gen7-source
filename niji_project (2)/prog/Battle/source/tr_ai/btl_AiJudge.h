/**
 * @file btl_AiJudge.h
 * @brief 戦闘AI( 何らかの判定を行うクラスの抽象化 )
 * @author obata_toshihiro
 * @date 2015.04.03
 */

#ifndef BTL_AI_JUDGE_H_INCLUDED
#define BTL_AI_JUDGE_H_INCLUDED

#include "../btl_common.h"
#include "./btl_AiScript.h"


GFL_NAMESPACE_BEGIN( btl )


/**
 * @class AiJudge
 * @brief 戦闘AI( 何らかの判定を行うクラスの抽象化 )
 */
class AiJudge
{
  GFL_FORBID_COPY_AND_ASSIGN( AiJudge );

public:

  /**
   * @brief 初期スコア
   */
  static const s32 SCORE_FLAT = 100;

  /**
   * @brief 非推奨スコア
   */
  static const s32 SCORE_DISCOURAGE = 0;



public:

  //---------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param minScriptNo      実行対象のスクリプト番号の最小値
   * @param maxScriptNo      実行対象のスクリプト番号の最大値
   * @param targetScriptBit  実行するAIのビットフラグ
   */
  //---------------------------------------------------------------------------
  AiJudge( BtlAiScriptNo minScriptNo, BtlAiScriptNo maxScriptNo, u32 targetScriptBit );

  //---------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //---------------------------------------------------------------------------
  virtual ~AiJudge();

  //---------------------------------------------------------------------------
  /**
   * @brief スクリプトを実行するための一時ヒープをアタッチする
   * @param heap  スクリプトを実行するための一時ヒープ
   */
  //---------------------------------------------------------------------------
  void AttackHeapForScriptExec( HeapHandle heap );

  //---------------------------------------------------------------------------
  /**
   * @brief 実行するＡＩのビットフラグを変更する
   * @param targetScriptBit  実行するAIのビットフラグ
   */
  //---------------------------------------------------------------------------
  void ChangeAiBit( u32 targetScriptBit );

  //---------------------------------------------------------------------------
  /**
   * @brief 判定計算を更新する
   */
  //---------------------------------------------------------------------------
  virtual void UpdateJudge( void ) = 0;

  //---------------------------------------------------------------------------
  /**
   * @brief 判定計算が終了したか？
   * @retval true   終了した
   * @retval false  終了していない
   */
  //---------------------------------------------------------------------------
  virtual bool IsJudgeFinished( void ) const = 0;



protected:

  //---------------------------------------------------------------------------
  /**
   * @brief スクリプトを実行するための一時ヒープを取得する
   */
  //---------------------------------------------------------------------------
  HeapHandle GetHeapForScriptExec( void ) const;

  //---------------------------------------------------------------------------
  /**
   * @brief 実行すべきスクリプト番号を取得する
   */
  //---------------------------------------------------------------------------
  BtlAiScriptNo GetCurrentScriptNo( void ) const;

  //---------------------------------------------------------------------------
  /**
   * @brief 実行するスクリプト番号をリセットする
   */
  //---------------------------------------------------------------------------
  void ResetScriptNo( void );

  //---------------------------------------------------------------------------
  /**
   * @brief 実行するスクリプト番号を更新する
   */
  //---------------------------------------------------------------------------
  void UpdateScriptNo( void );

  //---------------------------------------------------------------------------
  /**
   * @brief 全てのスクリプトを実行し終えたか？
   * @retval true    全てのスクリプトを実行し終えた
   * @retval false   実行していないスクリプトが存在する
   */
  //---------------------------------------------------------------------------
  bool IsAllScriptFinished( void ) const;



private:
  BtlAiScriptNo GetNextScriptNo( 
    BtlAiScriptNo currentScriptNo, 
    BtlAiScriptNo maxScriptNo ) const;



private:
  HeapHandle m_heapForScriptExec;
  BtlAiScriptNo m_minScriptNo;
  BtlAiScriptNo m_maxScriptNo;
  BtlAiScriptNo m_currentScriptNo;
  u32 m_targetScriptBit;
};


GFL_NAMESPACE_END( btl )


#endif // BTL_AI_JUDGE_H_INCLUDED