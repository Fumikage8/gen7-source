/**
 * @file btl_AiScriptHandler.h
 * @brief 戦闘AIスクリプトの制御
 * @author obata_toshihiro
 * @date 2015.04.09
 */

#ifndef BTL_AI_SCRIPT_HANDLER_H_INCLUDED
#define BTL_AI_SCRIPT_HANDLER_H_INCLUDED

#include "../btl_common.h"
#include "./btl_AiScript.h"
#include "./btl_AiScriptCommandHandler.h"

GFL_NAMESPACE_BEGIN( btl )



/**
 * @class AiScriptHandler
 * @brief 戦闘AIスクリプトの開始〜終了までを制御する
 */
class AiScriptHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( AiScriptHandler );

public:

  //---------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //---------------------------------------------------------------------------
  AiScriptHandler( void );

  //---------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //---------------------------------------------------------------------------
  virtual ~AiScriptHandler();


  /**
   * @brief スクリプトを開始するのに必要なパラメータ
   */
  struct ScriptStartParam
  {
    HeapHandle heapForScriptExec;                       // スクリプトの実行に使用する一時ヒープ
    AiScript* script;                                   // 操作対象のスクリプトオブジェクト
    BtlAiScriptNo scriptNo;                             // 実行するスクリプトNo.
    AiScriptCommandHandler* commandHandler;             // スクリプトに関連づけるコマンドハンドラ
    AiScriptCommandHandler::CommandParam commandParam;  // コマンドハンドラのパラメータ
  };

  //---------------------------------------------------------------------------
  /**
   * @brief スクリプトを開始する
   * @param startParam  開始パラメータ
   */
  //---------------------------------------------------------------------------
  void StartScript( const ScriptStartParam& startParam );

  //---------------------------------------------------------------------------
  /**
   * @brief スクリプトを実行し終了を待つ
   * @retval true   スクリプトが終了した
   * @retval false  スクリプトが終了していない
   */
  //---------------------------------------------------------------------------
  bool WaitScript( void );

  //---------------------------------------------------------------------------
  /**
   * @brief スクリプトの実行結果を取得する
   */
  //---------------------------------------------------------------------------
  const AiScript::Result& GetScriptResult( void ) const;






private:

#if PM_DEBUG
  void DebugPrint( const char* message ) const;
#endif

  HeapHandle m_heapForScriptExec;
  AiScript* m_script;
  BtlAiScriptNo m_scriptNo;
  AiScriptCommandHandler* m_commandHandler;
  AiScriptCommandHandler::CommandParam m_commandParam;

  u32 m_seq;
  AiScript::Result m_result;

};


GFL_NAMESPACE_END( btl )


#endif // BTL_AI_SCRIPT_HANDLER_H_INCLUDED