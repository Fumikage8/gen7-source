//=============================================================================
/**
 * @file	 BattleVideoPlayerDataListFrame.h
 * @brief	 ローカルデータ操作フレーム
 * @author yuto_uchida
 * @date	 2015.10.01
 */
//=============================================================================
#ifndef BATTLE_VIDEO_PLAYER_DATA_CTRL_LOCAL_FRAME_H_INCLUDED
#define BATTLE_VIDEO_PLAYER_DATA_CTRL_LOCAL_FRAME_H_INCLUDED
#pragma once

#include "NetApp/BattleVideoPlayer/source/Frame/BattleVideoPlayerDataCtrlBaseFrame.h"

GFL_NAMESPACE_BEGIN( NetApp )
GFL_NAMESPACE_BEGIN( BattleVideoPlayer )

  class UIViewPool;

  /** ローカルデータ操作フレーム */
  class DataCtrlLocalFrame : public DataCtrlBaseFrame
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(DataCtrlLocalFrame);

  public:
    //------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     */
    //------------------------------------------------------------------
    DataCtrlLocalFrame();

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~DataCtrlLocalFrame();

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
  };

GFL_NAMESPACE_END( BattleVideoPlayer )
GFL_NAMESPACE_END( NetApp )

#endif // BATTLE_VIDEO_PLAYER_DATA_CTRL_LOCAL_FRAME_H_INCLUDED
