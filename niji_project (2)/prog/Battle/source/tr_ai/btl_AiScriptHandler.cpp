/**
 * @file btl_AiScriptHandler.cpp
 * @brief 戦闘AIスクリプトの制御
 * @author obata_toshihiro
 * @date 2015.04.09
 */

#include "../btl_common.h"
#include "../btl_pokeparam.h"
#include "./btl_AiScriptHandler.h"
#include "./btl_AiScriptCommandHandler.h"

GFL_NAMESPACE_BEGIN( btl )



//---------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//---------------------------------------------------------------------------
AiScriptHandler::AiScriptHandler( void ) : 
  m_script( NULL ),
  m_commandHandler( NULL ),
  m_result(),
  m_seq( 0 )
{
}

//---------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//---------------------------------------------------------------------------
AiScriptHandler::~AiScriptHandler()
{
}


//---------------------------------------------------------------------------
/**
 * @brief スクリプトを開始する
 * @param startParam  開始パラメータ
 */
//---------------------------------------------------------------------------
void AiScriptHandler::StartScript( const ScriptStartParam& startParam )
{
  m_heapForScriptExec = startParam.heapForScriptExec;
  m_script = startParam.script;
  m_scriptNo = startParam.scriptNo;
  m_commandHandler = startParam.commandHandler;
  m_commandParam = startParam.commandParam;
  m_seq = 0;
}

//---------------------------------------------------------------------------
/**
 * @brief スクリプトを実行し終了を待つ
 * @retval true   スクリプトが終了した
 * @retval false  スクリプトが終了していない
 */
//---------------------------------------------------------------------------
bool AiScriptHandler::WaitScript( void )
{
  enum {
    SEQ_LOAD_START,
    SEQ_LOAD_WAIT,
    SEQ_EXEC_START,
    SEQ_EXEC_WAIT,
    SEQ_END,
  };

  switch( m_seq )
  {
  case SEQ_LOAD_START:
    #if PM_DEBUG
    this->DebugPrint( "[AiScriptHandler] スクリプトの読み込み開始" );
    #endif
    if( m_script->StartLoadScript( m_heapForScriptExec, m_scriptNo ) ) {
      ++m_seq;
      return false;
    }
    return true;


  case SEQ_LOAD_WAIT:
    if( m_script->WaitLoadScript() ) 
    {
      #if PM_DEBUG
      this->DebugPrint( "[AiScriptHandler] スクリプトの読み込み完了" );
      #endif
      ++m_seq;
    }
    return false;


  case SEQ_EXEC_START:
    #if PM_DEBUG
    this->DebugPrint( "[AiScriptHandler] スクリプトの実行開始" );
    #endif
    m_script->SetExecParameter( m_commandHandler );
    m_commandHandler->SetCommandParam( m_commandParam );
    ++m_seq;
    return false;


  case SEQ_EXEC_WAIT:
    if( m_script->Execute() ) 
    {
      m_script->GetResult( &m_result );
      #if PM_DEBUG
      this->DebugPrint( "[AiScriptHandler] スクリプトの実行終了" );
      #endif
      ++m_seq;
    }
    return false;


  case SEQ_END:
    m_script->UnLoadScript();
    return true;

  default:
    GFL_ASSERT(0);
    return true;
  }
}

#if PM_DEBUG

 /**
  * @brief デバッグ出力
  * @param message  出力するメッセージ
  */
void AiScriptHandler::DebugPrint( const char* message ) const
{
  u8 attackPokeID = ( m_commandParam.attackPoke == NULL ) ? ( BTL_POKEID_NULL ) : ( m_commandParam.attackPoke->GetID() );
  u8 defensePokeID = ( m_commandParam.defensePoke == NULL ) ? ( BTL_POKEID_NULL ) : ( m_commandParam.defensePoke->GetID() );
  u8 benchPokeID = ( m_commandParam.currentBenchPoke == NULL ) ? ( BTL_POKEID_NULL ) : ( m_commandParam.currentBenchPoke->GetID() );
  BTL_PRINT( "%s", message );
  BTL_PRINT( "  m_scriptNo=%d\n", m_scriptNo );
  BTL_PRINT( "  attackPokeID=%d\n", attackPokeID );
  BTL_PRINT( "  defensePokeID=%d\n", defensePokeID );
  BTL_PRINT( "  benchPokeID=%d\n", benchPokeID );
  BTL_PRINT( "  m_commandParam.currentWazaIndex=%d\n", m_commandParam.currentWazaIndex );
  BTL_PRINT( "  m_commandParam.currentWazaNo=%d\n", m_commandParam.currentWazaNo );
  BTL_PRINT( "  m_commandParam.currentItemNo=%d\n", m_commandParam.currentItemNo );
}

#endif

//---------------------------------------------------------------------------
/**
 * @brief スクリプトの実行結果を取得する
 */
//---------------------------------------------------------------------------
const AiScript::Result& AiScriptHandler::GetScriptResult( void ) const
{
  return m_result;
}


  


GFL_NAMESPACE_END( btl )