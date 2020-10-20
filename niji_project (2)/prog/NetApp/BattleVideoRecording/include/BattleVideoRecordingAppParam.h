//======================================================================
/**
 * @file BattleVideoRecordingAppParam.h
 * @date 2015/09/15 19:29:55
 * @author uchida_yuto
 * @brief バトルビデオ録画Procで公開してるパラメータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BATTLE_VIDEO_RECORDING_APP_PARAM_H_INCLUDED__
#define __BATTLE_VIDEO_RECORDING_APP_PARAM_H_INCLUDED__
#pragma once

//  gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <util/include/gfl2_std_string.h>
#include <str/include/gfl2_Str.h>

//  nijiのインクルード
#include "Battle/Regulation/include/Regulation.h"
#include "Battle/include/battle_SetupParam.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(ExtSavedata)

  class BattleRecorderSaveData;

GFL_NAMESPACE_END(ExtSavedata)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoRecording)

  //  Procの結果一覧
  enum ProcResultType
  {
    PROC_RESULT_TYPE_CANCEL = 0,
    PROC_RESULT_TYPE_SUCCESS,
    PROC_RESULT_TYPE_ERROR,
    PROC_RESULT_TYPE_FATAL,
  };

  //  公開アプリ
  struct APP_PARAM
  {
    APP_PARAM()
    {
      out.Clear();
    }

    //  入力パラメータ
    struct IN_PARAM
    {
      IN_PARAM()
      {
        saveDataServerVersion = BTL_NET_SERVER_VERSION;
        pRecorderSaveData     = NULL;
        bRecNG                = false;
      }

      //  レコードセーブ
      ExtSavedata::BattleRecorderSaveData*              pRecorderSaveData;

      //  サーバーバージョン
      u32 saveDataServerVersion;

      //  録画不許可
      bool  bRecNG;

    } in;

    //  出力パラメータ
    struct OUT_PARAM
    {
      void Clear()
      {
        procResultType  = PROC_RESULT_TYPE_ERROR;
      }

      //  proc処理結果
      ProcResultType  procResultType;
    } out;
  };

GFL_NAMESPACE_END(BattleVideoRecording)
GFL_NAMESPACE_END(NetApp)

#endif // __BATTLE_VIDEO_RECORDING_APP_PARAM_H_INCLUDED__