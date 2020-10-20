//======================================================================
/**
 * @file BattleVideoPlayerLocalAppParam.h
 * @date 2015/09/24 19:29:55
 * @author uchida_yuto
 * @brief バトルビデオ再生のlProcで公開してるパラメータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if !defined( __BATTLE_VIDEO_PLYAER_PROC_APP_PARAM_H_INCLUDED__ )
#define __BATTLE_VIDEO_PLYAER_PROC_APP_PARAM_H_INCLUDED__
#pragma once

//  gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <util/include/gfl2_std_string.h>
#include <str/include/gfl2_Str.h>

//  NetAppLibのインクルード
#include "NetStatic/NetAppLib/include/BattleVideoPlayer/BattleVideoPlayerVideoDataManager.h"

// pmlib
#include <pml/include/pmlib.h>

#include "ExtSavedata/include/BattleRecorderSaveData.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  //  Procの起動タイプ
  enum ProcRunType
  {
    //  初回から開始
    PROC_RUN_TYPE_BEGIN = 0,
    //  復帰
    PROC_RUN_TYPE_RETURN,
  };

  //  Procの結果一覧
  enum ProcResultType
  {
    PROC_RESULT_TYPE_SUCCESS  = 0,
    PROC_RESULT_TYPE_CANCEL,
    PROC_RESULT_TYPE_FATAL,
    //  模擬戦
    PROC_RESULT_TYPE_CALL_BTL_AI,
    //  再生
    PROC_RESULT_TYPE_CALL_BTL_PLAY,
  };

  //  Proc起動復帰データ
  struct PROC_RUN_TYPE_RETURN_DATA
  {
    PROC_RUN_TYPE_RETURN_DATA()
    {
      pActiveVideoData  = NULL;
      listScroll        = 0.0f;
      listCursorPos     = 0;
      seq               = 0;
      dataCtrlMode      = 0;
      infoDrawMode      = 0;
    }

    u32 seq;

    //  選択中のビデオデータ
    NetAppLib::BattleVideoPlayer::VIDEO_DATA* pActiveVideoData;

    //  リストのスクロール
    f32 listScroll;
    u32 listCursorPos;

    u32 infoDrawMode;
    u32 dataCtrlMode;
  };

  //  公開アプリ
  struct PROC_APP_PARAM
  {
    PROC_APP_PARAM()
    {
      in.Clear();
      out.Clear();

      pVideoDataManager = NULL;
    }

    //  in/outで共通で使うパラメータ
    PROC_RUN_TYPE_RETURN_DATA   procRunTypeReturnData;

    NetAppLib::BattleVideoPlayer::VideoDataManager*  pVideoDataManager;

    //  入力パラメータ
    struct _tag_in_param_
    {
      void Clear()
      {
        procRunType       = PROC_RUN_TYPE_BEGIN;
        bPlayCompletion   = false;
      }

      ProcRunType                 procRunType;

      //  ビデオ再生完走
      bool                        bPlayCompletion;
    } in;

    //  出力パラメータ
    struct _tag_out_param_
    {
      void Clear()
      {
        procResultType    = PROC_RESULT_TYPE_SUCCESS;
        pActiveVideoData  = NULL;
        dualTargetIdx     = 0;
      }

      //  選択したビデオデータ
      NetAppLib::BattleVideoPlayer::VIDEO_DATA*     pActiveVideoData;

      //  処理終了時の起動タイプ
      ProcResultType  procResultType;

      //  模擬戦相手の選択idx
      u32 dualTargetIdx;

    } out;

  };

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetApp)

#endif // __BATTLE_VIDEO_PLYAER_PROC_APP_PARAM_H_INCLUDED__