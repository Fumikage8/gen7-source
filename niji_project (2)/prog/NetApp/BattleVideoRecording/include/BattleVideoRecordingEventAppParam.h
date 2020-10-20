//======================================================================
/**
 * @file BattleVideoRecordingEventEventAppParam.h
 * @date 2015/09/15 19:53:03
 * @author uchida_yuto
 * @brief バトルビデオ録画イベントで公開しているパラメータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BATTLE_VIDEO_RECORDING_EVENT_APP_PARAM_H_INCLUDED__
#define __BATTLE_VIDEO_RECORDING_EVENT_APP_PARAM_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

//  nijiのインクルード
#include "Battle/Regulation/include/Regulation.h"
#include "Battle/include/battle_SetupParam.h"

#include "NetApp/BattleVideoRecording/include/BattleVideoRecordingAppParam.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(BattleVideoRecording)

  //  アプリ結果
  enum EventAppResult
  {
    //  録画せずに終了
    EVENT_APP_RESULT_CANCEL = 0,
    //  録画成功
    EVENT_APP_RESULT_VIDEO_REC
  };

  /**
   * @struct EVENT_APP_PARAM
   * @brief  BattleVideoRecordingEventEventを起動する時の入力と出力データ
   */
  struct EVENT_APP_PARAM
  {
    //  入力構造他
    struct IN_PARAM
    {
      IN_PARAM()
      {
      }

      //  アプリ起動に必要なパラメータ
      NetApp::BattleVideoRecording::APP_PARAM::IN_PARAM  appInParam;

    } in;

    //  出力構造体
    struct OUT_PARAM
    {
      OUT_PARAM()
      {
        result        = EVENT_APP_RESULT_CANCEL;
      }

      //  イベント終了時の結果
      EventAppResult  result;

    } out;
  };

GFL_NAMESPACE_END(BattleVideoRecordingEvent)
GFL_NAMESPACE_END(NetApp)

#endif // __BATTLE_VIDEO_RECORDING_EVENT_APP_PARAM_H_INCLUDED__