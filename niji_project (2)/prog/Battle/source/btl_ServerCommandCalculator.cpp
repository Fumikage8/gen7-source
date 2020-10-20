//=============================================================================
/**
 * @file   btl_ServerCommandCalculator.cpp
 * @date   2015/09/03 15:15:25
 * @author obata_toshihiro
 * @brief  サーバーコマンドの計算
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_ServerCommandCalculator.h"

GFL_NAMESPACE_BEGIN( btl )


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
 //-----------------------------------------------------------------------------
ServerCommandCalculator::ServerCommandCalculator( void ) : 
  m_isCommandCalculating_AfterActionSelect( false ),
  m_isCommandCalculating_AfterCoverPokeIn( false ),
  m_isCommandCalculating_AfterIntervalPokeChange( false )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
 //-----------------------------------------------------------------------------
ServerCommandCalculator::~ServerCommandCalculator()
{
}



//-----------------------------------------------------------------------------
/**
 * @brief 初期ポケモン投入コマンドを計算する
 * @param[out] output  コマンド計算の結果
 * @param      input   コマンド計算のための入力データ
 */
//-----------------------------------------------------------------------------
void ServerCommandCalculator::CalcBattleInCommand( Output_BattleInCommand* output, const Input_BattleInCommand& input )
{
  output->isAnyEventOccured = input.serverFlow->StartBtlIn();
  output->pCommandData      = input.commandQueue->buffer;
  output->commandDataSize   = input.commandQueue->writePtr;
}




//-----------------------------------------------------------------------------
/**
 * @brief 行動選択後の一連のコマンド計算を開始する
 * @param input  コマンド計算のための入力データ
 */
//-----------------------------------------------------------------------------
void ServerCommandCalculator::StartCalc_AfterActionSelect( const Input_AfterActionSelect& input )
{
  // 一連のコマンド計算の途中なら、開始処理を無視する
  if( m_isCommandCalculating_AfterActionSelect )
  {
    return;
  }

  input.serverFlow->StartTurn_Boot();
  m_isCommandCalculating_AfterActionSelect = true;
}

//-----------------------------------------------------------------------------
/**
 * @brief 行動選択後の一連のコマンド計算を更新する
 * @param[out] output  コマンド計算の結果
 * @param      input   コマンド計算のための入力データ
 */
//-----------------------------------------------------------------------------
void ServerCommandCalculator::UpdateCalc_AfterActionSelect( Output_AfterActionSelect* output, const Input_AfterActionSelect& input )
{
  // 事前に StartCalc_AfterActionSelect() が呼ばれている必要がある
  if( !m_isCommandCalculating_AfterActionSelect )
  {
    GFL_ASSERT(0);
    output->flowResult      = ServerFlow::RESULT_DEFAULT;
    output->pCommandData    = NULL;
    output->commandDataSize = 0;
    return;
  }

  const ServerFlow::Result result = input.serverFlow->StartTurn( input.clientAction );
  output->flowResult      = result;
  output->pCommandData    = input.commandQueue->buffer;
  output->commandDataSize = input.commandQueue->writePtr;


  // レベルアップしたなら、そこでコマンド列が途切れ、一旦クライアントに処理が渡る。
  // クライアントによるレベルアップ処理の後、残りのコマンド計算が行われる。
  // このとき、今回の計算の続きからコマンド計算が行われる必要があるため、
  // コマンド計算中フラグ( m_isCommandCalculating_AfterActionSelect )を落とさず、次の StartCalc_AfterActionSelect() 呼び出しを無視する。
  if( result != ServerFlow::RESULT_LEVELUP )
  {
    m_isCommandCalculating_AfterActionSelect = false;
  }
}



//-----------------------------------------------------------------------------
/**
 * @brief カバーポケモン投入後の一連のコマンド計算を開始する
 * @param input  コマンド計算のための入力データ
 */
//-----------------------------------------------------------------------------
void ServerCommandCalculator::StartCalc_AfterCoverPokeIn( const Input_AfterCoverPokeIn& input )
{
  // 一連のコマンド計算の途中なら、開始処理を無視する
  if( m_isCommandCalculating_AfterCoverPokeIn )
  {
    return;
  }

  input.serverFlow->StartAfterPokeIn_Boot();
  m_isCommandCalculating_AfterCoverPokeIn = true;
}

//-----------------------------------------------------------------------------
/**
 * @brief カバーポケモン投入後の一連のコマンド計算を更新する
 * @param[out] output  コマンド計算の結果
 * @param      input   コマンド計算のための入力データ
 */
//-----------------------------------------------------------------------------
void ServerCommandCalculator::UpdateCalc_AfterCoverPokeIn( Output_AfterCoverPokeIn* output, const Input_AfterCoverPokeIn& input )
{
  // 事前に StartCalc_AfterCoverPokeIn() が呼ばれている必要がある
  if( !m_isCommandCalculating_AfterCoverPokeIn )
  {
    GFL_ASSERT(0);
    output->flowResult      = ServerFlow::RESULT_DEFAULT;
    output->pCommandData    = NULL;
    output->commandDataSize = 0;
    return;
  }

  const ServerFlow::Result result = input.serverFlow->StartAfterPokeIn( input.clientAction );
  output->flowResult      = result;
  output->pCommandData    = input.commandQueue->buffer;
  output->commandDataSize = input.commandQueue->writePtr;


  // レベルアップしたなら、そこでコマンド列が途切れ、一旦クライアントに処理が渡る。
  // クライアントによるレベルアップ処理の後、残りのコマンド計算が行われる。
  // このとき、今回の計算の続きからコマンド計算が行われる必要があるため、
  // コマンド計算中フラグ( m_isCommandCalculating_AfterCoverPokeIn )を落とさず、次の StartCalc_AfterCoverPokeIn() 呼び出しを無視する。
  if( result != ServerFlow::RESULT_LEVELUP )
  {
    m_isCommandCalculating_AfterCoverPokeIn = false;
  }
}



//-----------------------------------------------------------------------------
/**
 * @brief ターン途中でのポケモン入れ替え後の一連のコマンド計算を開始する
 * @param input  コマンド計算のための入力データ
 */
//-----------------------------------------------------------------------------
void ServerCommandCalculator::StartCalc_AfterIntervalPokeChange( const Input_AfterIntervalPokeChange& input )
{
  // 一連のコマンド計算の途中なら、開始処理を無視する
  if( m_isCommandCalculating_AfterIntervalPokeChange )
  {
    return;
  }

  input.serverFlow->ContinueAfterPokeChange_Boot();
  m_isCommandCalculating_AfterIntervalPokeChange = true;
}

//-----------------------------------------------------------------------------
/**
 * @brief ターン途中でのポケモン入れ替え後の一連のコマンド計算を更新する
 * @param[out] output  コマンド計算の結果
 * @param      input   コマンド計算のための入力データ
 */
//-----------------------------------------------------------------------------
void ServerCommandCalculator::UpdateCalc_AfterIntervalPokeChange( Output_AfterIntervalPokeChange* output, const Input_AfterIntervalPokeChange& input )
{
  // 事前に StartCalc_AfterIntervalPokeChange() が呼ばれている必要がある
  if( !m_isCommandCalculating_AfterIntervalPokeChange )
  {
    GFL_ASSERT(0);
    output->flowResult      = ServerFlow::RESULT_DEFAULT;
    output->pCommandData    = NULL;
    output->commandDataSize = 0;
    return;
  }

  const ServerFlow::Result result = input.serverFlow->ContinueAfterPokeChange( input.clientAction );
  output->flowResult      = result;
  output->pCommandData    = input.commandQueue->buffer;
  output->commandDataSize = input.commandQueue->writePtr;

  // レベルアップしたなら、そこでコマンド列が途切れ、一旦クライアントに処理が渡る。
  // クライアントによるレベルアップ処理の後、残りのコマンド計算が行われる。
  // このとき、今回の計算の続きからコマンド計算が行われる必要があるため、
  // コマンド計算中フラグ( m_isCommandCalculating_AfterIntervalPokeChange )を落とさず、次の StartCalc_AfterIntervalPokeChange() 呼び出しを無視する。
  if( result != ServerFlow::RESULT_LEVELUP )
  {
    m_isCommandCalculating_AfterIntervalPokeChange = false;
  }
}



//-----------------------------------------------------------------------------
/**
 * @brief 乱入ポケモン入場コマンドを計算する
 * @param[out] output  コマンド計算の結果
 * @param      input   コマンド計算のための入力データ
 */
//-----------------------------------------------------------------------------
void ServerCommandCalculator::CalcIntrudePokeInCommand( Output_IntrudePokeIn* output, const Input_IntrudePokeIn& input )
{
  input.serverFlow->IntrudePokeIn();
  output->pCommandData    = input.commandQueue->buffer;
  output->commandDataSize = input.commandQueue->writePtr;
}



GFL_NAMESPACE_END( btl )