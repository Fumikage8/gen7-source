//=============================================================================
/**
 * @file	 BattleVideoPlayerDataDelFlow.h
 * @brief	 データ削除の共通フロー
 * @author yuto_uchida
 * @date	 2015.10.02
 */
//=============================================================================
#ifndef BATTLE_VIDEO_PLAYER_DATA_DEL_FLOW_H_INCLUDED
#define BATTLE_VIDEO_PLAYER_DATA_DEL_FLOW_H_INCLUDED
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

//  nijiのインクルード
#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "AppLib/include/Tool/app_tool_MessageMenuView.h"

//  UIViewのインクルード
#include "NetApp/BattleVideoPlayer/source/BattleVideoPlayerUIViewPool.h"
#include "NetApp/BattleVideoPlayer/include/BattleVideoPlayerProcAppParam.h"

//  NetStaticのインクルード
#include "NetStatic/NetAppLib/include/BattleVideoPlayer/BattleVideoPlayerVideoDataManager.h"

GFL_NAMESPACE_BEGIN( NetApp )
GFL_NAMESPACE_BEGIN( BattleVideoPlayer )

  /** データ削除共通フロー */
  //  フレームではなく、クラスのフローとした。（フレームにすると汎用性が低くなる）
  class DataDelFlow
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(DataDelFlow);

  public:
    //  実行結果
    enum Result
    {
      RESULT_CANCEL = 0,
      RESULT_DEL_SUCCESS,
      RESULT_DEL_ERROR,
      RESULT_DEL_FATAL,
      RESULT_RUN,
    };

    struct PARAM
    {
      PARAM()
      {
        Reset();
      }

      void Reset()
      {
        pUIViewPool       = NULL;
        pVideoDataManager = NULL;
      }

      //  リストが参照するデータ
      NetAppLib::BattleVideoPlayer::VideoDataManager*               pVideoDataManager;

      //  UIViewプール
      UIViewPool* pUIViewPool;
    };

    //------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     */
    //------------------------------------------------------------------
    DataDelFlow();

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~DataDelFlow();

    //------------------------------------------------------------------
    /**
     * @brief	  PROCの初期化を行う関数
     */
    //------------------------------------------------------------------
    bool InitFunc( const PARAM& rParam );

    //------------------------------------------------------------------
    /**
     * @brief	  PROCの終了を行う関数
     */
    //------------------------------------------------------------------
    bool EndFunc();

    //  開始(削除フラグを設定している場合に呼ぶ 主に選択用)
    bool Start();

    //  開始(指定したリストデータを削除)
    bool Start( const NetAppLib::BattleVideoPlayer::VIDEO_DATA* pVideoData );

    //  スタート開始後の更新
    Result Run();

  protected:
    enum
    {
      //  複数ビデオデータ選択削除
      SEQ_DEL_SELECT_START  = 0,
      SEQ_DEL_SELECT_WAIT,

      //  削除確認
      SEQ_DEL_CONFIRM_START,
      SEQ_DEL_CONFIRM_WAIT,

      //  ビデオデータ削除
      SEQ_DEL_DATA_START,
      SEQ_DEL_DATA_WAIT,
      SEQ_DEL_DATA_NOTICE_START,
      SEQ_DEL_DATA_NOTICE_WAIT,
    };

    //  変数初期化
    void _Clear()
    {
      m_result        = RESULT_CANCEL;
      m_deleteResult  = NetAppLib::BattleVideoPlayer::VideoDataManager::DELETE_RESULT_SUCCESS;

      m_updateSeq = SEQ_DEL_SELECT_START;
    }

    PARAM  m_param;

    u32 m_updateSeq;
    Result  m_result;

    NetAppLib::BattleVideoPlayer::VideoDataManager::DeleteResult  m_deleteResult;
};

GFL_NAMESPACE_END( BattleVideoPlayer )
GFL_NAMESPACE_END( NetApp )

#endif // BATTLE_VIDEO_PLAYER_DATA_DEL_FLOW_H_INCLUDED
