//=============================================================================
/**
 * @file   btl_AdapterCommandData.cpp
 * @date   2015/09/05 12:55:54
 * @author obata_toshihiro
 * @brief  アダプターコマンド計算
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_AdapterCommandData.h"

#include "./btl_ServerFlow.h"
#include "./btl_ServerCommandCalculator.h"
#include "./btl_SendData_ActionSelect.h"
#include "./btl_SendData_ClientLimitTime.h"

GFL_NAMESPACE_BEGIN( btl )


//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「戦闘開始直後のセットアップ」の送信データを計算する
 * @param[out] data      送信データ本体へのポインタの格納先
 * @param[out] dataSize  送信データ本体のサイズ
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_WAIT_SETUP( const void** data, u32* dataSize )
{
  *data     = NULL;
  *dataSize = 0;
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「初期ポケモン投入時のサーバーコマンド」の送信データを生成する
 * @param[out]    data                    送信データ本体へのポインタの格納先
 * @param[out]    dataSize                送信データ本体のサイズ
 * @param[out]    isAnyEventOccured       サーバーコマンドコマンド計算時に何らかのイベントが発生したか？
 * @param[in,out] serverCommandCalclator  使用するサーバーコマンド計算オブジェクト
 * @param[in,out] serverFlow              使用するサーバーフロー
 * @param[in,out] serverCommandQueue      serverFlow が生成したサーバーコマンドの格納先
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_SERVER_CMD_BattleIn( 
  const void**             data, 
  u32*                     dataSize, 
  bool*                    isAnyEventOccured, 
  ServerCommandCalculator* serverCommandCalclator,
  ServerFlow*              serverFlow,
  const SCQUE*             serverCommandQueue )
{
  ServerCommandCalculator::Input_BattleInCommand input;
  input.serverFlow   = serverFlow;
  input.commandQueue = serverCommandQueue;

  ServerCommandCalculator::Output_BattleInCommand calcResult;
  serverCommandCalclator->CalcBattleInCommand( &calcResult, input );

  *data              = calcResult.pCommandData;
  *dataSize          = calcResult.commandDataSize;
  *isAnyEventOccured = calcResult.isAnyEventOccured;
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「バトル開始時のタイミングコード送信」の送信データを生成する
 * @param[out] data       送信データ本体へのポインタの格納先
 * @param[out] dataSize   送信データ本体のサイズ
 * @param      recTool    データ生成に使用する rec::SendTool
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_RECORD_DATA_BtlInTiming( 
  const void**   data, 
  u32*           dataSize, 
  rec::SendTool* recTool )
{
  recTool->Init( true );
  *data = recTool->PutBtlInTiming( dataSize );
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「バトル開始の録画チャプター記録」の送信データを生成する
 * @param[out] data               送信データ本体へのポインタの格納先
 * @param[out] dataSize           送信データ本体のサイズ
 * @param      recTool            データ生成に使用する rec::SendTool
 * @param      isAnyEventOccured  初期ポケモン投入時に何らかのイベントが発生したか？
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_RECORD_DATA_BtlInChapter( 
  const void**   data, 
  u32*           dataSize, 
  rec::SendTool* recTool, 
  bool           isAnyEventOccured )
{
  recTool->Init( true );
  *data = recTool->PutBtlInChapter( dataSize, isAnyEventOccured );
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「行動選択開始」の送信データを計算する
 * @param[out] data      送信データ本体へのポインタの格納先
 * @param[out] dataSize  送信データ本体のサイズ
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_SELECT_ACTION( const void** data, u32* dataSize )
{
  *data     = NULL;
  *dataSize = 0;
}

//-----------------------------------------------------------------------------
/**
 * @brief アクション記録データを生成
 * @param[out] data             送信データ本体へのポインタの格納先
 * @param[out] dataSize         送信データ本体のサイズ
 * @param      recTool          データ生成に使用する rec::SendTool
 * @param      timingCode       アクション記録のタイミングコード
 * @param      fChapter         trueならターン区切りコードを記録データに埋め込む
 * @param      clientWorkArray  全クライアントのワーク
 *
 * @retval  void*   正しく生成できたら送信データポインタ / できない場合NULL
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_RECORD_DATA_ActionRecord( 
  const void**     data, 
  u32*             dataSize, 
  rec::SendTool*   recTool, 
  rec::Timing      timingCode,
  bool             fChapter,
  const SVCL_WORK* clientWorkArray )
{
  u32 ID;

  recTool->Init( fChapter );

  for(ID=0; ID<BTL_CLIENT_MAX; ++ID)
  {
    if( clientWorkArray[ID].myID != CLIENT_DISABLE_ID )
    {
      const SendData& returnData = clientWorkArray[ID].adapter->GetReturnData();
      const void* data = returnData.GetData();

      const SEND_DATA_ACTION_SELECT* actionSelectData = reinterpret_cast<const SEND_DATA_ACTION_SELECT*>( data );
      const BTL_ACTION_PARAM* action = actionSelectData->actionParam;
      const u32 numAction = actionSelectData->actionNum;
      recTool->PutSelActionData( ID, action, numAction );
    }
  }

  *data = recTool->FixSelActionData( timingCode, dataSize );
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「行動選択後のターン再生コマンド」の送信データを生成する
 * @param[out]    data                    送信データ本体へのポインタの格納先
 * @param[out]    dataSize                送信データ本体のサイズ
 * @param[out]    flowResult              サーバーコマンドコマンの計算結果
 * @param[in,out] serverCommandCalclator  使用するサーバーコマンド計算オブジェクト
 * @param[in,out] serverFlow              使用するサーバーフロー
 * @param[in,out] serverCommandQueue      serverFlow が生成したサーバーコマンドの格納先
 * @param         clientAction            全クライアントの行動パラメータ
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_SERVER_CMD_AfterActionSelect( 
  const void**             data, 
  u32*                     dataSize, 
  ServerFlow::Result*      flowResult, 
  ServerCommandCalculator* serverCommandCalclator, 
  ServerFlow*              serverFlow, 
  const SCQUE*             serverCommandQueue,
  const SVCL_ACTION&       clientAction )
{
  ServerCommandCalculator::Input_AfterActionSelect input;
  input.serverFlow   = serverFlow;
  input.commandQueue = serverCommandQueue;
  input.clientAction = &clientAction;

  ServerCommandCalculator::Output_AfterActionSelect calcResult;
  serverCommandCalclator->StartCalc_AfterActionSelect( input );
  serverCommandCalclator->UpdateCalc_AfterActionSelect( &calcResult, input );

  *data       = calcResult.pCommandData;
  *dataSize   = calcResult.commandDataSize;
  *flowResult = calcResult.flowResult;
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「逃げる、次を出す　選択開始」の送信データを計算する
 * @param[out] data      送信データ本体へのポインタの格納先
 * @param[out] dataSize  送信データ本体のサイズ
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_SELECT_CHANGE_OR_ESCAPE( const void** data, u32* dataSize )
{
  *data     = NULL;
  *dataSize = 0;
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「逃げる、次を出す　選択後の逃げる」の送信データを計算する
 * @param[out]    data               送信データ本体へのポインタの格納先
 * @param[out]    dataSize           送信データ本体のサイズ
 * @param[out]    isEscapeSuccessed  「逃げる」に成功したか？
 * @param[in,out] serverFlow         使用するサーバーフロー
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_CHANGE_OR_ESCAPE_ESCAPE_SELECTED( 
  const void** data, 
  u32*         dataSize,
  bool*        isEscapeSuccessed, 
  ServerFlow*  serverFlow,
  const SCQUE* serverCommandQueue )
{
  *isEscapeSuccessed = serverFlow->MakePlayerEscapeCommand();
  *data              = serverCommandQueue->buffer;
  *dataSize          = serverCommandQueue->writePtr;
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「カバーポケモン選択」の送信データを計算する
 * @param[out] data            送信データ本体へのポインタの格納先
 * @param[out] dataSize        送信データ本体のサイズ
 * @param      changePosArray  入れ替える位置の配列
 * @param      changePosNum    入れ替える位置の数
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_SELECT_POKEMON_FOR_COVER( 
  const void** data, 
  u32*         dataSize, 
  const u8*    changePosArray, 
  u8           changePosNum )
{
  *data     = changePosArray;
  *dataSize = changePosNum * sizeof( changePosArray[0] );
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「プレイヤへのポケモン入れ替え確認」の送信データを計算する
 * @param[out] data            送信データ本体へのポインタの格納先
 * @param[out] dataSize        送信データ本体のサイズ
 * @param      enemyPutPokeId  相手が次に繰り出すポケモンのID
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_CONFIRM_IREKAE( const void** data, u32* dataSize, const u8* enemyPutPokeId )
{
  *data     = enemyPutPokeId;
  *dataSize = sizeof( *enemyPutPokeId );
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「カバーポケモン投入　ポケモン投入後のコマンド」の送信データを生成する
 * @param[out]    data                    送信データ本体へのポインタの格納先
 * @param[out]    dataSize                送信データ本体のサイズ
 * @param[out]    flowResult              サーバーコマンドコマンの計算結果
 * @param[in,out] serverCommandCalclator  使用するサーバーコマンド計算オブジェクト
 * @param[in,out] serverFlow              使用するサーバーフロー
 * @param[in,out] serverCommandQueue      serverFlow が生成したサーバーコマンドの格納先
 * @param         clientAction            全クライアントの行動パラメータ
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_SERVER_CMD_AfterCoverPokeIn( 
  const void**             data, 
  u32*                     dataSize, 
  ServerFlow::Result*      flowResult,
  ServerCommandCalculator* serverCommandCalclator, 
  ServerFlow*              serverFlow,
  const SCQUE*             serverCommandQueue, 
  const SVCL_ACTION&       clientAction )
{
  ServerCommandCalculator::Input_AfterCoverPokeIn input;
  input.serverFlow   = serverFlow;
  input.commandQueue = serverCommandQueue;
  input.clientAction = &clientAction;

  ServerCommandCalculator::Output_AfterCoverPokeIn calcResult;
  serverCommandCalclator->StartCalc_AfterCoverPokeIn( input );
  serverCommandCalclator->UpdateCalc_AfterCoverPokeIn( &calcResult, input );

  *data       = calcResult.pCommandData;
  *dataSize   = calcResult.commandDataSize;
  *flowResult = calcResult.flowResult;
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「ターン途中のポケモン入れ替え　投入ポケモンの選択」の送信データを計算する
 * @param[out] data            送信データ本体へのポインタの格納先
 * @param[out] dataSize        送信データ本体のサイズ
 * @param      changePosArray  入れ替える位置の配列
 * @param      changePosNum    入れ替える位置の数
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_SELECT_POKEMON_FOR_CHANGE( 
  const void** data,
  u32*         dataSize, 
  const u8*    changePosArray, 
  u8           changePosNum )
{
  *data     = changePosArray;
  *dataSize = changePosNum * sizeof( changePosArray[0] );
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「ターン途中のポケモン入れ替え後のターン再生コマンド」の送信データを生成する
 * @param[out]    data                    送信データ本体へのポインタの格納先
 * @param[out]    dataSize                送信データ本体のサイズ
 * @param[out]    flowResult              サーバーコマンドコマンの計算結果
 * @param[in,out] serverCommandCalclator  使用するサーバーコマンド計算オブジェクト
 * @param[in,out] serverFlow              使用するサーバーフロー
 * @param[in,out] serverCommandQueue      serverFlow が生成したサーバーコマンドの格納先
 * @param         clientAction            全クライアントの行動パラメータ
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_SERVER_CMD_AfterIntervalPokeChange(
  const void**             data, 
  u32*                     dataSize, 
  ServerFlow::Result*      flowResult,
  ServerCommandCalculator* serverCommandCalclator, 
  ServerFlow*              serverFlow,
  const SCQUE*             serverCommandQueue, 
  const SVCL_ACTION&       clientAction )
{
  ServerCommandCalculator::Input_AfterIntervalPokeChange input;
  input.serverFlow   = serverFlow;
  input.commandQueue = serverCommandQueue;
  input.clientAction = &clientAction;

  ServerCommandCalculator::Output_AfterIntervalPokeChange calcResult;
  serverCommandCalclator->StartCalc_AfterIntervalPokeChange( input );
  serverCommandCalclator->UpdateCalc_AfterIntervalPokeChange( &calcResult, input );

  *data       = calcResult.pCommandData;
  *dataSize   = calcResult.commandDataSize;
  *flowResult = calcResult.flowResult;
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「乱入ポケモンの入場コマンド」の送信データを生成する
 * @param[out]    data                    送信データ本体へのポインタの格納先
 * @param[out]    dataSize                送信データ本体のサイズ
 * @param[in,out] serverCommandCalclator  使用するサーバーコマンド計算オブジェクト
 * @param[in,out] serverFlow              使用するサーバーフロー
 * @param[in,out] serverCommandQueue      serverFlow が生成したサーバーコマンドの格納先
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_SERVER_CMD_Intrude( 
  const void**             data, 
  u32*                     dataSize, 
  ServerCommandCalculator* serverCommandCalclator,
  ServerFlow*              serverFlow,
  const SCQUE*             serverCommandQueue )
{
  ServerCommandCalculator::Input_IntrudePokeIn input;
  input.serverFlow   = serverFlow;
  input.commandQueue = serverCommandQueue;

  ServerCommandCalculator::Output_IntrudePokeIn calcResult;
  serverCommandCalclator->CalcIntrudePokeInCommand( &calcResult, input );

  *data     = calcResult.pCommandData;
  *dataSize = calcResult.commandDataSize;
}

//-----------------------------------------------------------------------------
/*
 * @brief アダプターコマンド「制限時間終了　録画チャプター記録」の送信データを計算する
 * @param[out] data      送信データ本体へのポインタの格納先
 * @param[out] dataSize  送信データ本体のサイズ
 * @param      recTool   データ生成に使用する rec::SendTool
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_RECORD_DATA_TimeUp( const void** data, u32* dataSize, rec::SendTool* recTool )
{
  *data = recTool->PutTimeOverData( dataSize );
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「行動選択開始」の送信データを計算する
 * @param[out] data      送信データ本体へのポインタの格納先
 * @param[out] dataSize  送信データ本体のサイズ
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_NOTIFY_TIMEUP( const void** data, u32* dataSize )
{
  *data     = NULL;
  *dataSize = 0;
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「野生戦勝ち」の送信データを計算する
 * @param[out] data           送信データ本体へのポインタの格納先
 * @param[out] dataSize       送信データ本体のサイズ
 * @param      resultContext  勝敗判定コンテキスト
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_EXIT_WIN_WILD( const void** data, u32* dataSize, const BTL_RESULT_CONTEXT* resultContext )
{
  *data     = resultContext;
  *dataSize = sizeof( *resultContext );
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「野生戦負け」の送信データを計算する
 * @param[out] data           送信データ本体へのポインタの格納先
 * @param[out] dataSize       送信データ本体のサイズ
 * @param      resultContext  勝敗判定コンテキスト
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_EXIT_LOSE_WILD( const void** data, u32* dataSize, const BTL_RESULT_CONTEXT* resultContext )
{
  *data     = resultContext;
  *dataSize = sizeof( *resultContext );
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「野生戦捕獲」の送信データを計算する
 * @param[out] data           送信データ本体へのポインタの格納先
 * @param[out] dataSize       送信データ本体のサイズ
 * @param      resultContext  勝敗判定コンテキスト
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_EXIT_CAPTURE( const void** data, u32* dataSize, const BTL_RESULT_CONTEXT* resultContext )
{
  *data     = resultContext;
  *dataSize = sizeof( *resultContext );
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「戦闘終了　通信対戦」の送信データを計算する
 * @param[out] data           送信データ本体へのポインタの格納先
 * @param[out] dataSize       送信データ本体のサイズ
 * @param      resultContext  勝敗判定コンテキスト
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_EXIT_COMM( const void** data, u32* dataSize, const BTL_RESULT_CONTEXT* resultContext )
{
  *data     = resultContext;
  *dataSize = sizeof( *resultContext );
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「戦闘終了　NPC」の送信データを計算する
 * @param[out] data           送信データ本体へのポインタの格納先
 * @param[out] dataSize       送信データ本体のサイズ
 * @param      resultContext  勝敗判定コンテキスト
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_EXIT_NPC( const void** data, u32* dataSize, const BTL_RESULT_CONTEXT* resultContext )
{
  *data     = resultContext;
  *dataSize = sizeof( *resultContext );
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「戦闘終了　バトル施設トレーナー」の送信データを計算する
 * @param[out] data           送信データ本体へのポインタの格納先
 * @param[out] dataSize       送信データ本体のサイズ
 * @param      resultContext  勝敗判定コンテキスト
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_EXIT_BATTLE_SPOT_TRAINER( const void** data, u32* dataSize, const BTL_RESULT_CONTEXT* resultContext )
{
  *data     = resultContext;
  *dataSize = sizeof( *resultContext );
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「戦闘終了　フェードアウト」の送信データを計算する
 * @param[out] data      送信データ本体へのポインタの格納先
 * @param[out] dataSize  送信データ本体のサイズ
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_FADEOUT( const void** data, u32* dataSize )
{
  *data     = NULL;
  *dataSize = 0;
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「戦終了コマンド」の送信データを計算する
 * @param[out] data      送信データ本体へのポインタの格納先
 * @param[out] dataSize  送信データ本体のサイズ
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_QUIT_BATTLE( const void** data, u32* dataSize, const EscapeInfo& escapeInfo )
{
  *data = escapeInfo.Serialize( dataSize );
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「最後に送信したデータの再送信リクエスト」の送信データを計算する
 * @param[out] data      送信データ本体へのポインタの格納先
 * @param[out] dataSize  送信データ本体のサイズ
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_SEND_AGAIN_LAST_DATA( const void** data, u32* dataSize )
{
  *data     = NULL;
  *dataSize = 0;
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「残り手持ち時間の送信リクエスト」の送信データを計算する
 * @param[out] data      送信データ本体へのポインタの格納先
 * @param[out] dataSize  送信データ本体のサイズ
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_SEND_CLIENT_LIMIT_TIME( const void** data, u32* dataSize )
{
  *data     = NULL;
  *dataSize = 0;
}

//-----------------------------------------------------------------------------
/**
 * @brief アダプターコマンド「残り手持ち時間の同期」の送信データを計算する
 * @param[out] data       送信データ本体へのポインタの格納先
 * @param[out] dataSize   送信データ本体のサイズ
 * @param[out] buffer     送信するデータのバッファ
 * @param      timeLimit  送信する制限時間情報
 */
//-----------------------------------------------------------------------------
void AdapterCommandData::CalcData_SYNC_CLIENT_LIMIT_TIME( const void** data, u32* dataSize, SEND_DATA_CLIENT_LIMIT_TIME_SYNC* buffer, const TimeLimit& timeLimit )
{
  for( u32 clientID=0; clientID<BTL_CLIENT_NUM; ++clientID )
  {
    buffer->limitTime[ clientID ] = timeLimit.clientLimitTime[ clientID ];
  }

  *data = buffer;
  *dataSize = sizeof( SEND_DATA_CLIENT_LIMIT_TIME_SYNC );
}

GFL_NAMESPACE_END( btl )