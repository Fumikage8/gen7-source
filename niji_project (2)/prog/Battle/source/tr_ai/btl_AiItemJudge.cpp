/**
 * @file btl_AiItemJudge.cpp
 * @brief 戦闘AI( 道具の使用判定 )
 * @author obata_toshihiro
 * @date 2015.04.03
 */
#include "../btl_mainmodule.h"
#include "../btl_pokecon.h"
#include "./btl_AiScript.h"
#include "./btl_AiScriptHandler.h"
#include "./btl_AiScriptCommandHandler.h"
#include "./btl_AiItemJudge.h"


GFL_NAMESPACE_BEGIN( btl ) 


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
AiItemJudge::AiItemJudge( 
  HeapHandle heapHandle, 
  AiScript* aiScript, 
  const MainModule* mainModule, 
  ServerFlow* serverFlow, 
  const POKECON* pokecon, 
  const u32 randSeed,
  u32 scriptBit ) : 
    AiJudge( 
      BTL_AISCRIPT_NO_ITEM_MIN, 
      BTL_AISCRIPT_NO_ITEM_MAX,
      scriptBit ),
    m_mainModule( mainModule ),
    m_pokecon( pokecon ),
    m_poke( NULL ),
    m_randGenerator( NULL ),
    m_script( aiScript ),
    m_scriptHandler( NULL ),
    m_scriptCommandHandler( NULL ),
    m_itemNo( ITEM_DUMMY_DATA ),
    m_score( 0 ),
    m_isFinished( true )
{
  m_randGenerator = GFL_NEW( heapHandle ) AIRandSys();
  m_randGenerator->Initialize( randSeed );

  m_scriptHandler = GFL_NEW( heapHandle ) AiScriptHandler();
  m_scriptCommandHandler = GFL_NEW( heapHandle ) AiScriptCommandHandler( heapHandle, mainModule, serverFlow, pokecon, randSeed );
}

//---------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//---------------------------------------------------------------------------
AiItemJudge::~AiItemJudge()
{
  GFL_SAFE_DELETE( m_randGenerator );
  GFL_SAFE_DELETE( m_scriptHandler );
  GFL_SAFE_DELETE( m_scriptCommandHandler );
}

//---------------------------------------------------------------------------
/**
 * @brief 判定計算を開始する
 * @param poke    判定対象のポケモン
 * @param itemNo  採点対象の道具
 */
//---------------------------------------------------------------------------
void AiItemJudge::StartJudge( const BTL_POKEPARAM* poke, u16 itemNo )
{
  m_poke = poke;
  m_itemNo = itemNo;
  m_seq = 0;
  m_score = AiJudge::SCORE_FLAT;
  m_isFinished = false;
  this->ResetScriptNo();
}

//---------------------------------------------------------------------------
/**
 * @brief 判定計算を更新する
 */
//---------------------------------------------------------------------------
void AiItemJudge::UpdateJudge( void )
{
  enum {
    SEQ_SCRIPT_START,
    SEQ_SCRIPT_WAIT,
    SEQ_TO_NEXT_SCRIPT,
    SEQ_END,
  };

  switch( m_seq )
  {
  case SEQ_SCRIPT_START:
    if( this->IsAllScriptFinished() )
    {
      m_seq = SEQ_END;
      return;
    }
    else
    {
      this->StartScript();
      ++m_seq;
      return;
    }


  case SEQ_SCRIPT_WAIT:
    if( m_scriptHandler->WaitScript() ) {
      this->RegisterScriptResult();
      ++m_seq;
    }
    return;


  case SEQ_TO_NEXT_SCRIPT:
    this->UpdateScriptNo();
    m_seq = SEQ_SCRIPT_START;
    return;


  case SEQ_END:
    m_isFinished = true;
    return;


  default:
    GFL_ASSERT(0);
    m_seq = SEQ_END;
    return;
  }
}

/**
 * @brief スクリプトの実行を開始する
 */
void AiItemJudge::StartScript( void )
{
  BtlPokePos attackPos = m_mainModule->PokeIDtoPokePos( m_pokecon, m_poke->GetID() );
  BtlPokePos defensePos = m_mainModule->GetOpponentPokePos( attackPos, 0 );

  AiScriptHandler::ScriptStartParam startParam;
  startParam.heapForScriptExec = this->GetHeapForScriptExec();
  startParam.script = m_script;
  startParam.scriptNo = this->GetCurrentScriptNo();
  startParam.commandHandler = m_scriptCommandHandler;
  startParam.commandParam.currentItemNo = m_itemNo;
  startParam.commandParam.attackPoke = m_poke;
  startParam.commandParam.defensePoke = m_pokecon->GetFrontPokeDataConst( defensePos );
  m_scriptHandler->StartScript( startParam );
}

/**
 * @brief スクリプトの実行結果を記録する
 */
void AiItemJudge::RegisterScriptResult( void )
{
  AiScript::Result result = m_scriptHandler->GetScriptResult();
  m_score += result.score;
}

//---------------------------------------------------------------------------
/**
 * @brief 判定計算が終了したか？
 * @retval true   終了した
 * @retval false  終了していない
 */
//---------------------------------------------------------------------------
bool AiItemJudge::IsJudgeFinished( void ) const
{
  return m_isFinished;
}

//---------------------------------------------------------------------------
/**
 * @brief 「道具を使用すること」に対する採点結果を取得する
 */
//---------------------------------------------------------------------------
s32 AiItemJudge::GetScore( void ) const
{
  return m_score;
}


GFL_NAMESPACE_END( btl )
