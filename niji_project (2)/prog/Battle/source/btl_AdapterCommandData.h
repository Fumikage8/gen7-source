//=============================================================================
/**
 * @file   btl_AdapterCommandData.h
 * @date   2015/09/05 12:55:54
 * @author obata_toshihiro
 * @brief  アダプターコマンドとして送信するデータの計算
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_ADAPTERCOMMANDDATA_H_INCLUDED
#define BTL_ADAPTERCOMMANDDATA_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "./btl_server_const.h"
#include "./btl_ServerFlow.h"
#include "./btl_rec.h"
#include "./btl_server_TimeLimit.h"
#include "./btl_SendData_ClientLimitTime.h"


GFL_NAMESPACE_BEGIN( btl )

  class ServerFlow;
  class ServerCommandCalculator;


  /**
   * @class AdapterCommandData
   * @brief アダプターコマンドとして送信するデータを計算する
   */
  class AdapterCommandData
  {
    GFL_FORBID_COPY_AND_ASSIGN( AdapterCommandData );
  
  public:


    static void CalcData_WAIT_SETUP( const void** data, u32* dataSize );
    static void CalcData_SERVER_CMD_BattleIn( const void** data, u32* dataSize, bool* isAnyEventOccured, ServerCommandCalculator* serverCommandCalclator, ServerFlow* serverFlow, const SCQUE* serverCommandQueue );
    static void CalcData_SERVER_CMD_AfterActionSelect( const void** data, u32* dataSize, ServerFlow::Result* flowResult, ServerCommandCalculator* serverCommandCalclator, ServerFlow* serverFlow, const SCQUE* serverCommandQueue, const SVCL_ACTION& clientAction );
    static void CalcData_SERVER_CMD_AfterCoverPokeIn( const void** data, u32* dataSize, ServerFlow::Result* flowResult, ServerCommandCalculator* serverCommandCalclator, ServerFlow* serverFlow, const SCQUE* serverCommandQueue, const SVCL_ACTION& clientAction );
    static void CalcData_SERVER_CMD_AfterIntervalPokeChange( const void** data, u32* dataSize, ServerFlow::Result* flowResult, ServerCommandCalculator* serverCommandCalclator, ServerFlow* serverFlow, const SCQUE* serverCommandQueue, const SVCL_ACTION& clientAction );
    static void CalcData_SERVER_CMD_Intrude( const void** data, u32* dataSize, ServerCommandCalculator* serverCommandCalclator, ServerFlow* serverFlow, const SCQUE* serverCommandQueue );
    static void CalcData_RECORD_DATA_BtlInTiming( const void** data, u32* dataSize, rec::SendTool* recTool );
    static void CalcData_RECORD_DATA_BtlInChapter( const void** data, u32* dataSize, rec::SendTool* recTool, bool isAnyEventOccured );
    static void CalcData_RECORD_DATA_ActionRecord( const void** data, u32* dataSize, rec::SendTool* recTool, rec::Timing timingCode, bool fChapter, const SVCL_WORK* clientWorkArray );
    static void CalcData_RECORD_DATA_TimeUp( const void** data, u32* dataSize, rec::SendTool* recTool );
    static void CalcData_SELECT_ACTION( const void** data, u32* dataSize );
    static void CalcData_SELECT_CHANGE_OR_ESCAPE( const void** data, u32* dataSize );
    static void CalcData_CHANGE_OR_ESCAPE_ESCAPE_SELECTED( const void** data, u32* dataSize, bool* isEscapeSuccessed, ServerFlow* serverFlow, const SCQUE* serverCommandQueue );
    static void CalcData_SELECT_POKEMON_FOR_COVER( const void** data, u32* dataSize, const u8* changePosArray, u8 changePosNum );
    static void CalcData_CONFIRM_IREKAE( const void** data, u32* dataSize, const u8* enemyPutPokeId );
    static void CalcData_SELECT_POKEMON_FOR_CHANGE( const void** data, u32* dataSize, const u8* changePosArray, u8 changePosNum );
    static void CalcData_NOTIFY_TIMEUP( const void** data, u32* dataSize );
    static void CalcData_EXIT_WIN_WILD( const void** data, u32* dataSize, const BTL_RESULT_CONTEXT* resultContext );
    static void CalcData_EXIT_LOSE_WILD( const void** data, u32* dataSize, const BTL_RESULT_CONTEXT* resultContext );
    static void CalcData_EXIT_CAPTURE( const void** data, u32* dataSize, const BTL_RESULT_CONTEXT* resultContext );
    static void CalcData_EXIT_COMM( const void** data, u32* dataSize, const BTL_RESULT_CONTEXT* resultContext );
    static void CalcData_EXIT_NPC( const void** data, u32* dataSize, const BTL_RESULT_CONTEXT* resultContext );
    static void CalcData_EXIT_BATTLE_SPOT_TRAINER( const void** data, u32* dataSize, const BTL_RESULT_CONTEXT* resultContext );
    static void CalcData_FADEOUT( const void** data, u32* dataSize );
    static void CalcData_QUIT_BATTLE( const void** data, u32* dataSize, const EscapeInfo& escapeInfo );
    static void CalcData_SEND_AGAIN_LAST_DATA( const void** data, u32* dataSize );
    static void CalcData_SEND_CLIENT_LIMIT_TIME( const void** data, u32* dataSize );
    static void CalcData_SYNC_CLIENT_LIMIT_TIME( const void** data, u32* dataSize, SEND_DATA_CLIENT_LIMIT_TIME_SYNC* buffer, const TimeLimit& timeLimit );

  };


GFL_NAMESPACE_END( btl )

#endif // BTL_ADAPTERCOMMANDDATA_H_INCLUDED