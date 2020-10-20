//=============================================================================
/**
 * @file	 BattleVideoPlayerDataCtrlNetFrame.h
 * @brief	 通信データ操作フレーム
 * @author yuto_uchida
 * @date	 2015.11.18
 */
//=============================================================================
#ifndef BATTLE_VIDEO_PLAYER_DATA_CTRL_NET_FRAME_H_INCLUDED
#define BATTLE_VIDEO_PLAYER_DATA_CTRL_NET_FRAME_H_INCLUDED
#pragma once

#include "NetApp/BattleVideoPlayer/source/Frame/BattleVideoPlayerDataCtrlBaseFrame.h"

GFL_NAMESPACE_BEGIN( NetApp )
GFL_NAMESPACE_BEGIN( BattleVideoPlayer )

  class UIViewPool;

  /** 通信データ操作フレーム */
  class DataCtrlNetFrame : public DataCtrlBaseFrame
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(DataCtrlNetFrame);

  public:
    //------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     */
    //------------------------------------------------------------------
    DataCtrlNetFrame();

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~DataCtrlNetFrame();

    //------------------------------------------------------------------
    /**
     * @brief	  PROCの初期化を行う関数
     * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
     */
    //------------------------------------------------------------------
    virtual applib::frame::Result InitFunc();

    //------------------------------------------------------------------
    /**
     * @brief	  PROCのメイン更新を行う関数
     * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
     */
    //------------------------------------------------------------------
    virtual applib::frame::Result UpdateFunc();

  private:
    //  更新シーケンス
    enum
    {
      //  アップロード
      UPDATE_SEQ_UPLOAD_START  = UPDATE_ROOT_SEQ_OTHER,
      UPDATE_SEQ_UPLOAD_WAIT,
      UPDATE_SEQ_UPLOAD_END,

      //  アップロードキャンセル
      UPDATE_SEQ_UPLOAD_CANCEL_START,
      UPDATE_SEQ_UPLOAD_CANCEL_WAIT,
      UPDATE_SEQ_UPLOAD_CANCEL_END,

      //  模擬戦選択
      UPDATE_SEQ_DUEL_SELECT,
    };

    //  シーケンス設定
    virtual u32 _StartSeq( const u32 seq );

    //  画面更新
    void _RefrashView();

    void _Clear()
    {
      m_uploadResult  = NetAppLib::BattleVideoPlayer::VideoDataManager::UPLOAD_RESULT_ERROR;
      m_uploadCanelResult = NetAppLib::BattleVideoPlayer::VideoDataManager::UPLOAD_CANCEL_RESULT_ERROR;
    }

    NetAppLib::BattleVideoPlayer::VideoDataManager::UploadResult        m_uploadResult;
    NetAppLib::BattleVideoPlayer::VideoDataManager::UploadCancelResult  m_uploadCanelResult;
  };

GFL_NAMESPACE_END( BattleVideoPlayer )
GFL_NAMESPACE_END( NetApp )

#endif // BATTLE_VIDEO_PLAYER_DATA_CTRL_NET_FRAME_H_INCLUDED
