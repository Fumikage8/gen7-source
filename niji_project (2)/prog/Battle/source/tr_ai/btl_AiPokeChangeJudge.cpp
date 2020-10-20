/**
 * @file btl_AiPokeChangeJudge.cpp
 * @brief 戦闘AI( ポケモン入れ替えの判定 )
 * @author obata_toshihiro
 * @date 2015.04.03
 */

#include "../btl_mainmodule.h"
#include "../btl_client.h"
#include "./btl_AiScript.h"
#include "./btl_AiScriptHandler.h"
#include "./btl_AiScriptCommandHandler.h"
#include "./btl_AiPokeChangeJudge.h"


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
AiPokeChangeJudge::AiPokeChangeJudge( 
  HeapHandle heapHandle, 
  AiScript* aiScript, 
  const MainModule* mainModule, 
  ServerFlow* serverFlow, 
  const POKECON* pokecon, 
  const u32 randSeed,
  u32 scriptBit ) : 
    AiJudge( 
      BTL_AISCRIPT_NO_POKECHANGE_MIN, 
      BTL_AISCRIPT_NO_POKECHANGE_MAX, 
      scriptBit ),
    m_mainModule( mainModule ),
    m_client( NULL ),
    m_pokecon( pokecon ),
    m_targetFrontPoke( NULL ),
    m_randGenerator( NULL ),
    m_script( aiScript ),
    m_scriptHandler( NULL ),
    m_scriptCommandHandler( NULL ),
    m_seq( 0 ),
    m_isFinished( true ),
    m_result()
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
AiPokeChangeJudge::~AiPokeChangeJudge()
{
  GFL_SAFE_DELETE( m_randGenerator );
  GFL_SAFE_DELETE( m_scriptHandler );
  GFL_SAFE_DELETE( m_scriptCommandHandler );
}

//---------------------------------------------------------------------------
/**
 * @brief 判定計算を開始する
 * @param targetFrontPoke  判定対象の、場にいるポケモン
 */
//---------------------------------------------------------------------------
void AiPokeChangeJudge::StartJudge( const BTL_POKEPARAM* targetFrontPoke )
{
  m_client = m_mainModule->GetClientByPokeID( targetFrontPoke->GetID() );
  m_targetFrontPoke = targetFrontPoke;

  m_isFinished = false;
  m_seq = 0;
  m_judgePokeIndex = 0;
  m_result.isPokeChangeEnable = false;
  this->ResetScriptNo();
}

//---------------------------------------------------------------------------
/**
 * @brief 判定計算を更新する
 */
//---------------------------------------------------------------------------
void AiPokeChangeJudge::UpdateJudge( void )
{
  enum {
    SEQ_INIT,
    SEQ_SCRIPT_START,
    SEQ_SCRIPT_WAIT,
    SEQ_SCRIPT_END,
    SEQ_TO_NEXT_POKE,
    SEQ_TO_NEXT_SCRIPT,
    SEQ_END,
  };

  switch( m_seq )
  {
  case SEQ_INIT:
    this->InitJudgePokeData();
    m_seq = ( 0 < m_judgePokeCount ) ? ( SEQ_SCRIPT_START ) : ( SEQ_END );
    return;


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
      ++m_seq;
    }
    return;


  case SEQ_SCRIPT_END:
    this->RegisterScriptResult();
    ++m_seq;
    return;


  case SEQ_TO_NEXT_POKE:
    ++m_judgePokeIndex;
    m_seq = ( m_judgePokeIndex < m_judgePokeCount ) ? ( SEQ_SCRIPT_START ) : ( SEQ_TO_NEXT_SCRIPT );
    return;


  case SEQ_TO_NEXT_SCRIPT:
    this->UpdateScriptNo();
    m_seq = SEQ_SCRIPT_START;
    return;


  case SEQ_END:
    this->SetupJudgeResult();
    m_isFinished = true;
    return;
  }
}

/**
 * @brief 判定対象ポケモンごとのデータを初期化する
 */
void AiPokeChangeJudge::InitJudgePokeData( void )
{
  u8 puttablePokeIndexWithinParty[ BTL_PARTY_MEMBER_MAX ];
  u32 puttablePokeCount = m_client->countPuttablePokemons( puttablePokeIndexWithinParty );

  m_judgePokeCount = 0;
  for( u32 i=0; i<puttablePokeCount; ++i )
  {
    u8 pokeIndexWithinParty = puttablePokeIndexWithinParty[i];
    if( !( m_client->changeAI_CheckReserve( pokeIndexWithinParty ) ) ) {
      m_judgePokeData[ m_judgePokeCount ].inPartyIndex = pokeIndexWithinParty;
      m_judgePokeData[ m_judgePokeCount ].isChangeEnable = false;
      m_judgePokeData[ m_judgePokeCount ].score = AiJudge::SCORE_FLAT;
      ++m_judgePokeCount;
    }
  }
}

/**
 * @brief スクリプトの実行を開始する
 */
void AiPokeChangeJudge::StartScript( void )
{
  BtlPokePos attackPos = m_mainModule->PokeIDtoPokePos( m_pokecon, m_targetFrontPoke->GetID() );
  BtlPokePos defensePos = m_mainModule->GetOpponentPokePos( attackPos, 0 );

  AiScriptHandler::ScriptStartParam startParam;
  startParam.heapForScriptExec = this->GetHeapForScriptExec();
  startParam.script = m_script;
  startParam.scriptNo = this->GetCurrentScriptNo();
  startParam.commandHandler = m_scriptCommandHandler;
  startParam.commandParam.currentBenchPoke = this->GetCurrentJudgePokeParam();
  startParam.commandParam.attackPoke = m_targetFrontPoke;
  startParam.commandParam.defensePoke = m_pokecon->GetFrontPokeDataConst( defensePos );
  m_scriptHandler->StartScript( startParam );
}

/**
 * @brief 現在の判定対象ポケモンのパラメータを取得する
 */
const BTL_POKEPARAM* AiPokeChangeJudge::GetCurrentJudgePokeParam( void ) const
{
  u8 memberIndex = m_judgePokeData[ m_judgePokeIndex ].inPartyIndex;
  return m_client->GetParty()->GetMemberDataConst( memberIndex );
}

/**
 * @brief スクリプトの実行結果を記録する
 */
void AiPokeChangeJudge::RegisterScriptResult( void )
{
  AiScript::Result result = m_scriptHandler->GetScriptResult();
  m_judgePokeData[ m_judgePokeIndex ].isChangeEnable = result.isPokeChangeEnable;
  m_judgePokeData[ m_judgePokeIndex ].score += result.score;
}

/**
 * @brief スクリプトの実行結果から、「入れ替え」の判定結果をセットアップする
 */
void AiPokeChangeJudge::SetupJudgeResult( void )
{
  s32 judgePokeIndex = this->GetBestScoredJudgePokeIndex();
  if( judgePokeIndex < 0 ) {
    m_result.isPokeChangeEnable = false;
    return;
  }

  m_result.isPokeChangeEnable = true;
  m_result.score = m_judgePokeData[ judgePokeIndex ].score;
  m_result.inPokePartyIndex = m_judgePokeData[ judgePokeIndex ].inPartyIndex;
}

/**
 * @brief 最も点数の高い判定対象ポケモンのインデックスを取得する
 * @retval -1  入れ替え可能なポケモンが存在しない場合
 */
s32 AiPokeChangeJudge::GetBestScoredJudgePokeIndex( void ) const
{
  s32 bestScore = 0;
  s32 bestScoredCount = 0;
  s32 bestScoredIndex[ BTL_PARTY_MEMBER_MAX ];
  for( u32 i=0; i<m_judgePokeCount; ++i )
  {
    if( !( m_judgePokeData[i].isChangeEnable ) ) {
      continue;
    }

    if( ( bestScoredCount == 0 ) ||
        ( bestScore < m_judgePokeData[i].score ) ) {
          bestScore = m_judgePokeData[i].score;
          bestScoredIndex[0] = i;
          bestScoredCount = 1;
    }
    else if( bestScore == m_judgePokeData[i].score ) {
      bestScoredIndex[ bestScoredCount ] = i;
      ++bestScoredCount;
    }
  }

  if( bestScoredCount <= 0 ) {
    return -1;
  }

  if( bestScoredCount == 1 ) {
    return bestScoredIndex[0];
  }

  u32 rand = m_randGenerator->Next( bestScoredCount );
  return bestScoredIndex[ rand ];
}

//---------------------------------------------------------------------------
/**
 * @brief 判定計算が終了したか？
 * @retval true   終了した
 * @retval false  終了していない
 */
//---------------------------------------------------------------------------
bool AiPokeChangeJudge::IsJudgeFinished( void ) const
{
  return m_isFinished;
}

//---------------------------------------------------------------------------
/**
 * @brief 判定結果を取得する
 */
//---------------------------------------------------------------------------
const AiPokeChangeJudge::JudgeResult& AiPokeChangeJudge::GetJudgeResult( void ) const
{
  return m_result;
}


GFL_NAMESPACE_END( btl )
