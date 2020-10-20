#if !defined(NIJI_PROJECT_BATTLE_VIDEO_PLYAER_NET_PROC_H_INCLUDED)
#define NIJI_PROJECT_BATTLE_VIDEO_PLYAER_NET_PROC_H_INCLUDED
#pragma once

//==============================================================================
/**
 @file    BattleVideoPlayerNetProc.h
 @date    2015.11.16
 @note
          バトルビデオ録画プロセス(net版)
 */
//==============================================================================

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameProc.h>
#include <GameSys/include/GameManager.h>
#include <App/NumberInput/include/NumberInputAppParam.h>

//  アプリでの公開用
#include "NetApp/BattleVideoPlayer/include/BattleVideoPlayerProcAppParam.h"

#include "NetApp/BattleVideoPlayer/include/BattleVideoPlayerBaseProc.h"

//  フレーム
#include "NetApp/BattleVideoPlayer/source/Frame/BattleVideoPlayerNetTopSelectFrame.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  //==============================================================================
  /**
   @class     Proc
   */
  //==============================================================================
  class NetProc : public BaseProc
  {
    GFL_FORBID_COPY_AND_ASSIGN(NetProc);

  public:
    NetProc( PROC_APP_PARAM* pAppParam );
    virtual ~NetProc();

    virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
    virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );

  private:
    //  UpdateFuncのシーケンス
    enum
    {
      //  バトルビデオリスト
      //  シリアル入力か個人公開ビデオの選択のトップ画面
      UPDATE_SEQ_TOP  = UPDATE_SEQ_ROOT_OTHER,
      //  シリアルno入力
      UPDATE_SEQ_SERIAL_NO_INPUT,
      //  シリアルnoからデータ作成
      UPDATE_SEQ_SERIAL_NO_CREATE_DATA,

      //  バトルビデオ再生カウントアップ
      UPDATA_SEQ_INC_PLAY_COUNT,

      UPDATE_SEQ_END,
    };

    enum
    {
      UPDATE_SEQ_SERIAL_NO_INPUT_SUB_FADE_OUT_START = 0,
      UPDATE_SEQ_SERIAL_NO_INPUT_SUB_FADE_OUT_WAIT,
      UPDATE_SEQ_SERIAL_NO_INPUT_SUB_FRAME_CREATE,
      UPDATE_SEQ_SERIAL_NO_INPUT_SUB_FRAME_WAIT,
      UPDATE_SEQ_SERIAL_NO_INPUT_SUB_FADE_IN_START,
      UPDATE_SEQ_SERIAL_NO_INPUT_SUB_FADE_IN_WAIT,
      UPDATE_SEQ_SERIAL_NO_INPUT_SUB_END,
    };

    void _Clear()
    {
      m_initSeq   = 0;
      m_serialNo  = 0;
      m_subSeq    = 0;
      m_pNumInputModule = NULL;
    }

    //  フレーム管理のみの制御にするか
    virtual bool _GetFrameMangerProcOnly();

    virtual u32 _StartSeq( const u32 seq );

    //  復帰データからシーケンス開始
    virtual u32 _StartReturnSeq( const PROC_RUN_TYPE_RETURN_DATA& rReturnData );

    u32                            m_initSeq;
    u64                            m_serialNo;
    u32                            m_subSeq;
    NetTopSelectFrame::FRAME_PARAM m_netTopSelectFrameParam;

    App::NumberInput::APP_PARAM           m_mumberInputAppParam;

    gfl2::ro::Module*       m_pNumInputModule;    //!< NumberInput DLL化のため追加 
  };

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetApp)

#endif  // NIJI_PROJECT_BATTLE_VIDEO_PLYAER_NET_PROC_H_INCLUDED

