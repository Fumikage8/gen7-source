#if !defined(NIJI_PROJECT_BATTLE_VIDEO_PLYAER_LOCAL_PROC_H_INCLUDED)
#define NIJI_PROJECT_BATTLE_VIDEO_PLYAER_LOCAL_PROC_H_INCLUDED
#pragma once

//==============================================================================
/**
 @file    BattleVideoPlayerProc.h
 @date    2015.09.15
 @note
          バトルビデオ録画プロセス
 */
//==============================================================================

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameProc.h>
#include <GameSys/include/GameManager.h>

//  アプリでの公開用
#include "NetApp/BattleVideoPlayer/include/BattleVideoPlayerProcAppParam.h"

#include "NetApp/BattleVideoPlayer/include/BattleVideoPlayerBaseProc.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  //==============================================================================
  /**
   @class     Proc
   */
  //==============================================================================
  class LocalProc : public BaseProc
  {
    GFL_FORBID_COPY_AND_ASSIGN(LocalProc);

  public:
    LocalProc( PROC_APP_PARAM* pAppParam );
    virtual ~LocalProc();

    virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );

  private:
    //  UpdateFuncのシーケンス
    enum
    {
      //  バトルビデオリスト
      UPDATE_SEQ_END = UPDATE_SEQ_ROOT_OTHER,
    };
  };

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetApp)

#endif  // NIJI_PROJECT_BATTLE_VIDEO_PLYAER_LOCAL_PROC_H_INCLUDED

