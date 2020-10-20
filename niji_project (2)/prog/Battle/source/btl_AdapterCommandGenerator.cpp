//=============================================================================
/**
 * @file   btl_AdapterCommandGenerator.cpp
 * @date   2015/09/05 16:56:22
 * @author obata_toshihiro
 * @brief  アダプターコマンド生成
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_AdapterCommandGenerator.h"

#include "./btl_SendDataContainer.h"
#include "./btl_ServerCommandCalculator.h"
#include "./btl_AdapterCommandData.h"


GFL_NAMESPACE_BEGIN( btl )


//---------------------------------------------------------------------------
 /**
  * @brief コンストラクタ
  * @param setupParam  セットアップパラメータ
  */
//---------------------------------------------------------------------------
AdapterCommandGenerator::AdapterCommandGenerator( const SetupParam& setupParam ) : 
  m_sendDataContainer( setupParam.sendDataContainer ),
  m_serverCommandCalculator( setupParam.serverCommandCalclator ),
  m_serverFlow( setupParam.serverFlow ),
  m_serverCommandQueue( setupParam.serverCommandQueue ),
  m_recTool( setupParam.recTool ),
  m_clientWorkArray( setupParam.clientWorkArray ),
  m_clientAction( setupParam.clientAction ),
  m_changePokePosArray( setupParam.changePokePosArray ),
  m_changePokePosNum( setupParam.changePokePosNum ),
  m_enemyPutPokeId( setupParam.enemyPutPokeId ),
  m_battleResultContext( setupParam.battleResultContext ),
  m_escapeInfo( setupParam.escapeInfo ),
  m_timeLimit( setupParam.timeLimit ),
  m_isBattleInEventOccured( false ),
  m_isEscapeSucceeded( false ),
  m_flowResult( ServerFlow::RESULT_DEFAULT )
{
}

//---------------------------------------------------------------------------
 /**
  * @brief デストラクタ
  */
//---------------------------------------------------------------------------
AdapterCommandGenerator::~AdapterCommandGenerator()
{
}

//---------------------------------------------------------------------------
/**
 * @brief サーバーからのリクエストから、送信するアダプターコマンドを生成する
 * @param[out] adapterCommand   送信するアダプターコマンド
 * @param[out] data             送信するコマンドデータ本体へのポインタの格納先
 * @param[out] dataSize         送信するコマンドデータ本体のサイズの格納先
 * @param      serverReq        サーバーからのリクエストID
 * @param      commandSerialNo  生成するコマンドの通し番号
 */
//---------------------------------------------------------------------------
void AdapterCommandGenerator::GenerateCommandData( AdapterCommand* adapterCommand, const void** data, u32* dataSize, ServerRequest serverReq, u32 commandSerialNo )
{
  // 既に送信済みなら、再計算をせずに、過去に送信したデータを再送信する
  if( m_sendDataContainer->IsDataExist( commandSerialNo ) )
  {
    const SendData* sendData = m_sendDataContainer->GetData( commandSerialNo );
    *data           = sendData->GetData();
    *dataSize       = sendData->GetDataSize();
    *adapterCommand = sendData->GetAdapterCommand();
    return;
  }

  // 未送信なら、計算で求める
  *adapterCommand = this->GetAdapterCommand( serverReq );
  this->CalcCommandData( data, dataSize, serverReq );
}

/**
 * @brief サーバーのリクエストから、送信するアダプターコマンドの種類を取得する
 * @param serverReq  サーバーのリクエスト
 */
AdapterCommand AdapterCommandGenerator::GetAdapterCommand( ServerRequest serverReq ) const
{
  // サーバーリクエスト　→　アダプターコマンド　変換テーブル
  static const struct 
  {
    ServerRequest req;
    AdapterCommand cmd;
  } 
  REQUEST_COMMAND_TABLE[ SERVER_REQ_NUM ] = 
  {
    { SERVER_REQ_START_SETUP,                               ACMD_WAIT_SETUP },
    { SERVER_REQ_START_ACTION_SELECT,                       ACMD_SELECT_ACTION },
    { SERVER_REQ_START_CHANGE_OR_ESCAPE,                    ACMD_SELECT_CHANGE_OR_ESCAPE },
    { SERVER_REQ_START_COVER_POKE_SELECT,                   ACMD_SELECT_POKEMON_FOR_COVER },
    { SERVER_REQ_START_SELECT_POKE_BY_INTERVAL_POKECHANGE,  ACMD_SELECT_POKEMON_FOR_CHANGE },
    { SERVER_REQ_START_CONFIRM_COUNTER_POKECHANGE,          ACMD_CONFIRM_IREKAE },
    { SERVER_REQ_SERVER_COMMAND_FIRST_BATTLE_IN,            ACMD_SERVER_CMD },
    { SERVER_REQ_SERVER_COMMAND_AFTER_ACTION_SELECT,        ACMD_SERVER_CMD },
    { SERVER_REQ_SERVER_COMMAND_AFTER_COVER_POKE_SELECT,    ACMD_SERVER_CMD },
    { SERVER_REQ_SERVER_COMMAND_AFTER_INTERVAL_POKECHANGE,  ACMD_SERVER_CMD },
    { SERVER_REQ_SERVER_COMMAND_ESCAPE_BY_CHANGE_OR_ESCAPE, ACMD_SERVER_CMD },
    { SERVER_REQ_SERVER_COMMAND_INTRUDE,                    ACMD_SERVER_CMD },
    { SERVER_REQ_RECORD_BATTLE_START_TIMING,                ACMD_RECORD_DATA },
    { SERVER_REQ_RECORD_BATTLE_START_CHAPTER,               ACMD_RECORD_DATA },
    { SERVER_REQ_RECORD_SELECTED_ACTION,                    ACMD_RECORD_DATA },
    { SERVER_REQ_RECORD_COVER_POKE_SELECT,                  ACMD_RECORD_DATA },
    { SERVER_REQ_RECORD_INTERVAL_POKECHANGE,                ACMD_RECORD_DATA },
    { SERVER_REQ_RECORD_TIMEUP_CHAPTER,                     ACMD_RECORD_DATA },
    { SERVER_REQ_NOTIFY_TIMEUP,                             ACMD_NOTIFY_TIMEUP },
    { SERVER_REQ_EXIT_WILD_WIN,                             ACMD_EXIT_WIN_WILD },
    { SERVER_REQ_EXIT_WILD_LOSE,                            ACMD_EXIT_LOSE_WILD },
    { SERVER_REQ_EXIT_WILD_CAPTURE,                         ACMD_EXIT_CAPTURE },
    { SERVER_REQ_EXIT_COMM,                                 ACMD_EXIT_COMM },
    { SERVER_REQ_EXIT_NPC,                                  ACMD_EXIT_NPC },
    { SERVER_REQ_EXIT_BATTLE_SPOT_TRAINER,                  ACMD_EXIT_SUBWAY_TRAINER },
    { SERVER_REQ_EXIT_FADEOUT,                              ACMD_FADEOUT },
    { SERVER_REQ_QUIT_BATTLE,                               ACMD_QUIT_BTL },
    { SERVER_REQ_SEND_LATEST_REPLY,                         ACMD_SEND_LATEST_REPLY },
    { SERVER_REQ_SEND_CLIENT_LIMIT_TIME,                    ACMD_CLIENT_LIMIT_TIME_SEND },
    { SERVER_REQ_SYNC_CLIENT_LIMIT_TIME,                    ACMD_CLIENT_LIMIT_TIME_SYNC },
  };

  for( u32 i=0; i<GFL_NELEMS( REQUEST_COMMAND_TABLE ); ++i )
  {
    if( serverReq == REQUEST_COMMAND_TABLE[i].req )
    {
      return REQUEST_COMMAND_TABLE[i].cmd;
    }
  }

  GFL_ASSERT_STOP(0);
  return ACMD_QUIT_BTL;
}

/**
 * @brief アダプターコマンドとして送信するデータを計算する
 * @param[out] data        コマンドデータ本体へのポインタの格納先
 * @param[out] dataSize    コマンドデータ本体のサイズの格納先
 * @param      serverReq   サーバーからのリクエストID
 */
void AdapterCommandGenerator::CalcCommandData( const void** data, u32* dataSize, ServerRequest serverReq )
{
  switch( serverReq )
  {
  case SERVER_REQ_START_SETUP:                                AdapterCommandData::CalcData_WAIT_SETUP( data, dataSize );  break;
  case SERVER_REQ_START_ACTION_SELECT:                        AdapterCommandData::CalcData_SELECT_ACTION( data, dataSize );  break;
  case SERVER_REQ_START_CHANGE_OR_ESCAPE:                     AdapterCommandData::CalcData_SELECT_CHANGE_OR_ESCAPE( data, dataSize );  break;
  case SERVER_REQ_START_COVER_POKE_SELECT:                    AdapterCommandData::CalcData_SELECT_POKEMON_FOR_COVER( data, dataSize, m_changePokePosArray, *m_changePokePosNum );  break;
  case SERVER_REQ_START_CONFIRM_COUNTER_POKECHANGE:           AdapterCommandData::CalcData_CONFIRM_IREKAE( data, dataSize, m_enemyPutPokeId );  break;
  case SERVER_REQ_SERVER_COMMAND_AFTER_COVER_POKE_SELECT:     AdapterCommandData::CalcData_SERVER_CMD_AfterCoverPokeIn( data, dataSize, &m_flowResult, m_serverCommandCalculator, m_serverFlow, m_serverCommandQueue, *m_clientAction );  break;
  case SERVER_REQ_START_SELECT_POKE_BY_INTERVAL_POKECHANGE:   AdapterCommandData::CalcData_SELECT_POKEMON_FOR_CHANGE( data, dataSize, m_changePokePosArray, *m_changePokePosNum );  break;

  case SERVER_REQ_SERVER_COMMAND_FIRST_BATTLE_IN:             AdapterCommandData::CalcData_SERVER_CMD_BattleIn( data, dataSize, &m_isBattleInEventOccured, m_serverCommandCalculator, m_serverFlow, m_serverCommandQueue );  break;
  case SERVER_REQ_SERVER_COMMAND_AFTER_ACTION_SELECT:         AdapterCommandData::CalcData_SERVER_CMD_AfterActionSelect( data, dataSize, &m_flowResult, m_serverCommandCalculator, m_serverFlow, m_serverCommandQueue, *m_clientAction );  break;
  case SERVER_REQ_SERVER_COMMAND_ESCAPE_BY_CHANGE_OR_ESCAPE:  AdapterCommandData::CalcData_CHANGE_OR_ESCAPE_ESCAPE_SELECTED( data, dataSize, &m_isEscapeSucceeded, m_serverFlow, m_serverCommandQueue );  break;
  case SERVER_REQ_SERVER_COMMAND_AFTER_INTERVAL_POKECHANGE:   AdapterCommandData::CalcData_SERVER_CMD_AfterIntervalPokeChange( data, dataSize, &m_flowResult, m_serverCommandCalculator, m_serverFlow, m_serverCommandQueue, *m_clientAction );  break;
  case SERVER_REQ_SERVER_COMMAND_INTRUDE:                     AdapterCommandData::CalcData_SERVER_CMD_Intrude( data, dataSize, m_serverCommandCalculator, m_serverFlow, m_serverCommandQueue );  break;

  case SERVER_REQ_RECORD_BATTLE_START_TIMING:                 AdapterCommandData::CalcData_RECORD_DATA_BtlInTiming( data, dataSize, m_recTool );  break;
  case SERVER_REQ_RECORD_BATTLE_START_CHAPTER:                AdapterCommandData::CalcData_RECORD_DATA_BtlInChapter( data, dataSize, m_recTool, m_isBattleInEventOccured );  break;
  case SERVER_REQ_RECORD_SELECTED_ACTION:                     AdapterCommandData::CalcData_RECORD_DATA_ActionRecord( data, dataSize, m_recTool, rec::RECTIMING_StartTurn, true, m_clientWorkArray );  break;
  case SERVER_REQ_RECORD_COVER_POKE_SELECT:                   AdapterCommandData::CalcData_RECORD_DATA_ActionRecord( data, dataSize, m_recTool, rec::RECTIMING_PokeInCover, false, m_clientWorkArray );  break;
  case SERVER_REQ_RECORD_INTERVAL_POKECHANGE:                 AdapterCommandData::CalcData_RECORD_DATA_ActionRecord( data, dataSize, m_recTool, rec::RECTIMING_PokeInChange, false, m_clientWorkArray );  break;
  case SERVER_REQ_RECORD_TIMEUP_CHAPTER:                      AdapterCommandData::CalcData_RECORD_DATA_TimeUp( data, dataSize, m_recTool );  break;

  case SERVER_REQ_NOTIFY_TIMEUP:                              AdapterCommandData::CalcData_NOTIFY_TIMEUP( data, dataSize );  break;
  case SERVER_REQ_EXIT_WILD_WIN:                              AdapterCommandData::CalcData_EXIT_WIN_WILD( data, dataSize, m_battleResultContext );  break;
  case SERVER_REQ_EXIT_WILD_LOSE:                             AdapterCommandData::CalcData_EXIT_LOSE_WILD( data, dataSize, m_battleResultContext );  break;
  case SERVER_REQ_EXIT_WILD_CAPTURE:                          AdapterCommandData::CalcData_EXIT_CAPTURE( data, dataSize, m_battleResultContext ); break;
  case SERVER_REQ_EXIT_COMM:                                  AdapterCommandData::CalcData_EXIT_COMM( data, dataSize, m_battleResultContext );  break;
  case SERVER_REQ_EXIT_NPC:                                   AdapterCommandData::CalcData_EXIT_NPC( data, dataSize, m_battleResultContext );  break;
  case SERVER_REQ_EXIT_BATTLE_SPOT_TRAINER:                   AdapterCommandData::CalcData_EXIT_BATTLE_SPOT_TRAINER( data, dataSize, m_battleResultContext );  break;
  case SERVER_REQ_EXIT_FADEOUT:                               AdapterCommandData::CalcData_FADEOUT( data, dataSize );  break;
  case SERVER_REQ_QUIT_BATTLE:                                AdapterCommandData::CalcData_QUIT_BATTLE( data, dataSize, *m_escapeInfo );  break;

  case SERVER_REQ_SEND_LATEST_REPLY:                          AdapterCommandData::CalcData_SEND_AGAIN_LAST_DATA( data, dataSize );  break;
  case SERVER_REQ_SEND_CLIENT_LIMIT_TIME:                     AdapterCommandData::CalcData_SEND_CLIENT_LIMIT_TIME( data, dataSize );  break;
  case SERVER_REQ_SYNC_CLIENT_LIMIT_TIME:                     AdapterCommandData::CalcData_SYNC_CLIENT_LIMIT_TIME( data, dataSize, &m_sendData_ClientLimitTime, *m_timeLimit );  break;
  }
}

//---------------------------------------------------------------------------
/**
 * @brief フロー計算の結果を取得する
 */
//---------------------------------------------------------------------------
ServerFlow::Result AdapterCommandGenerator::GetFlowResult( void ) const
{
  return m_flowResult;
}

//---------------------------------------------------------------------------
/**
 * @brief 逃げる、次を出す　選択において、「逃げる」コマンドに成功したか？
 * @retval true   成功した
 * @retval false  失敗した
 */
//---------------------------------------------------------------------------
bool AdapterCommandGenerator::IsEscapeSucceededOnChangeOrEscape( void ) const
{
  return m_isEscapeSucceeded;
}



GFL_NAMESPACE_END( btl )